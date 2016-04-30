// Copyright 2016 TwinDrills

#include "Core/Platform/Environment.h"
#include "Platforms/Windows/Helper/Errors.h"
#include "Platforms/Windows/Helper/Wmi.h"
#include "Core/Types/CachedValue.h"
#include "Core/Types/CachedValueArray.h"

#include <Windows.h>
#include <Lmcons.h>
#include <VersionHelpers.h>
#include <shlwapi.h>
#include <shlobj.h>

using namespace Ludo;
using namespace Ludo::Environment;
using namespace Ludo::Windows;

#if !defined(PRODUCT_EDUCATION)
#define PRODUCT_EDUCATION 0x00000079
#endif
#if !defined(PRODUCT_EDUCATION_N)
#define PRODUCT_EDUCATION_N 0x0000007A
#endif
#if !defined(PRODUCT_ENTERPRISE_S)
#define PRODUCT_ENTERPRISE_S 0x0000007D
#endif
#if !defined(PRODUCT_ENTERPRISE_S_EVALUATION)
#define PRODUCT_ENTERPRISE_S_EVALUATION 0x00000081
#endif
#if !defined(PRODUCT_ENTERPRISE_S_N)
#define PRODUCT_ENTERPRISE_S_N 0x0000007E
#endif
#if !defined(PRODUCT_ENTERPRISE_S_N_EVALUATION)
#define PRODUCT_ENTERPRISE_S_N_EVALUATION 0x00000082
#endif
#if !defined(PRODUCT_IOTUAP)
#define PRODUCT_IOTUAP 0x0000007B
#endif
#if !defined(PRODUCT_IOTUAPCOMMERCIAL)
#define PRODUCT_IOTUAPCOMMERCIAL 0x00000083
#endif
#if !defined(PRODUCT_MOBILE_ENTERPRISE)
#define PRODUCT_MOBILE_ENTERPRISE 0x00000085
#endif

namespace Ludo {
namespace Environment {

int GetCpuCount()
{
	static CachedValue<int> Value;

	return Value.Get([]()
	{
		WmiQuery Query("select * from Win32_Processor");
		WmiQueryResult Result;

		int CpuCount = 1;

		if (Query.Execute(&Result).Succeeded())
		{
			CpuCount = Result.RowCount();
		}

		return CpuCount;
	});
}

int GetGpuCount()
{
	static CachedValue<int> Value;

	return Value.Get([]()
	{
		WmiQuery Query("select * from Win32_VideoController");
		WmiQueryResult Result;

		int CpuCount = 1;

		if (Query.Execute(&Result).Succeeded())
		{
			CpuCount = Result.RowCount();
		}

		return CpuCount;
	});
}

int GetConcurrencyFactor()
{
	static CachedValue<int> Value;

	return Value.Get([]()
	{
		int ThreadCount = 0;

		for (int i = 0; i < GetCpuCount(); i++)
		{
			CpuInfo Info = GetCpuInfo(i);
			ThreadCount += Info.ThreadCount;
		}

		return ThreadCount;
	});
}

CpuInfo GetCpuInfo(const int CpuIndex)
{
	static CachedValueArray<CpuInfo> Value;

	return Value.Get(CpuIndex, [CpuIndex]()
	{
		WmiQuery Query("select * from Win32_Processor");
		WmiQueryResult Result;

		CpuInfo Info;
		Info.CoreCount = 1;
		Info.Features.Empty();
		Info.Name = "Unknown";

		if (Query.Execute(&Result).Succeeded())
		{
			WmiQueryRow Row = Result.GetRow(CpuIndex);
			Info.CoreCount = Row.GetInt("NumberOfCores");
			Info.ThreadCount = Row.GetInt("NumberOfLogicalProcessors");
			Info.Name = Row.GetString("Name").Replace("  ", " ");

#define CheckFeature(Feature, Value, Bit) \
			if ((CpuId[Value] & ((int)1 << Bit)) != 0) \
			{ \
				Info.Features.Add(CpuFeature::Feature); \
			} \

			// HACK: This is super gross as it depends what CPU we are executing on at the moment. If we have multiple processors this will 
			// almost guaranteeably return incorrect information for each processor. Fix plz.

			// Based on cpuid code from her:
			// http://stackoverflow.com/questions/6121792/how-to-check-if-a-cpu-supports-the-sse3-instruction-set
			int CpuId[4];
			__cpuid(CpuId, 0);
			int IdCount = CpuId[0];

			__cpuid(CpuId, 0x80000000);
			unsigned ExIdCount = CpuId[0];

			//  Detect Features
			if (IdCount >= 0x00000001)
			{
				__cpuid(CpuId, 0x00000001);

				CheckFeature(MMX, 3, 23);
				CheckFeature(SSE, 3, 25);
				CheckFeature(SSE2, 3, 26);
				CheckFeature(SSE3, 2, 0);
				CheckFeature(SSSE3, 2, 9);
				CheckFeature(SSE41, 2, 19);
				CheckFeature(SSE42, 2, 20);
				CheckFeature(AES, 2, 25);
				CheckFeature(AVX, 2, 28);
				CheckFeature(FMA3, 2, 12);
				CheckFeature(RDRAND, 2, 30);

			}
			if (IdCount >= 0x00000007)
			{
				__cpuid(CpuId, 0x00000007);

				CheckFeature(AVX2, 1, 5);
				CheckFeature(BMT1, 1, 3);
				CheckFeature(BMT2, 1, 8);
				CheckFeature(ADX, 1, 19);
				CheckFeature(ADX, 1, 29);
				CheckFeature(PREFETCHWT1, 2, 0);

				CheckFeature(AVX512F, 1, 16);
				CheckFeature(AVX512CD, 1, 28);
				CheckFeature(AVX512PF, 1, 26);
				CheckFeature(AVX512ER, 1, 27);
				CheckFeature(AVX512VL, 1, 31);
				CheckFeature(AVX512BW, 1, 30);
				CheckFeature(AVX512DQ, 1, 17);
				CheckFeature(AVX512IFMA, 1, 21);
				CheckFeature(AVX512VBMI, 2, 1);
			}
			if (ExIdCount >= 0x80000001)
			{
				__cpuid(CpuId, 0x80000001);

				CheckFeature(X64, 3, 29);
				CheckFeature(ABM, 2, 5);
				CheckFeature(SSE4a, 2, 6);
				CheckFeature(FMA4, 2, 16);
				CheckFeature(XOP, 2, 11);
			}

#undef CheckFeature
		}

		return Info;
	});
}

GpuInfo GetGpuInfo(const int GpuIndex)
{
	CachedValueArray<GpuInfo> Value;

	return Value.Get(GpuIndex, [GpuIndex] ()
	{
		WmiQuery Query("select * from Win32_VideoController");
		WmiQueryResult Result;

		GpuInfo Info;
		Info.Name = "Unknown";
		Info.TotalVRam = 0;

		if (Query.Execute(&Result).Succeeded())
		{
			WmiQueryRow Row = Result.GetRow(GpuIndex);
			Info.Name = Row.GetString("Name");
			Info.TotalVRam = Row.GetLong("AdapterRAM"); // uint32, does not support more than 4GiB, sigh.
		}

		return Info;
	});
}

RamInfo GetRamInfo()
{
	static CachedValue<RamInfo> Value;

	return Value.Get([]() 
	{
		MEMORYSTATUSEX MemInfo;
		MemInfo.dwLength = sizeof(MEMORYSTATUSEX);
		GlobalMemoryStatusEx(&MemInfo);

		RamInfo Info;
		Info.TotalPhysicalRam = MemInfo.ullTotalPhys;
		Info.TotalVirtualRam = MemInfo.ullTotalVirtual;
		Info.TotalPageRam = MemInfo.ullTotalPageFile;

		return Info;
	});
}

String GetPlatformName()
{
	static CachedValue<String> Value;

	return Value.Get([] () 
	{		
		SYSTEM_INFO SystemInfo;

		OSVERSIONINFOEXA VersionInfo;
		VersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXA);

		DWORD ProductType;

		GetSystemInfo(&SystemInfo);

		if (!GetVersionExA((OSVERSIONINFOA*)&VersionInfo))
		{
			LogLastSystemError("GetVersionEx");
			return String("Unknown");
		}

		if (!GetProductInfo(VersionInfo.dwMajorVersion, VersionInfo.dwMinorVersion, 0, 0, &ProductType))
		{
			LogLastSystemError("GetProductInfo");
			return String("Unknown");
		}

		String Result = "Unknown";

		if (VER_PLATFORM_WIN32_NT == VersionInfo.dwPlatformId && VersionInfo.dwMajorVersion > 4)
		{
			Result = "Microsoft ";

			if (VersionInfo.dwMajorVersion >= 6)
			{
				if (VersionInfo.dwMajorVersion == 6 && VersionInfo.dwMinorVersion == 0)
				{
					if (VersionInfo.wProductType == VER_NT_WORKSTATION)
					{
						Result += "Windows Vista";
					}
					else
					{
						Result += "Windows Server 2008";
					}
				}
				else if (VersionInfo.dwMinorVersion == 0 || VersionInfo.dwMinorVersion == 1 || VersionInfo.dwMinorVersion == 2 || VersionInfo.dwMinorVersion == 3)
				{
					if (VersionInfo.dwMajorVersion == 6)
					{
						if (VersionInfo.wProductType == VER_NT_WORKSTATION && VersionInfo.dwMinorVersion == 1)
						{
							Result += "Windows 7";
						}
						else if (VersionInfo.wProductType == VER_NT_WORKSTATION && VersionInfo.dwMinorVersion == 2)
						{
							Result += "Windows 8";
						}
						else if (VersionInfo.wProductType == VER_NT_WORKSTATION && VersionInfo.dwMinorVersion == 3)
						{
							Result += "Windows 8.1";
						}
						else
						{
							Result += "Windows Server 2008 R2";
						}
					}
					else if (VersionInfo.dwMajorVersion == 10)
					{
						if (VersionInfo.wProductType == VER_NT_WORKSTATION)
						{
							Result += "Windows 10";
						}
						else
						{
							Result += "Windows Server 2016";
						}
					}
					else
					{
						Result += "Unknown Windows Version";
					}

					Result += " ";

					switch (ProductType)
					{
					case PRODUCT_BUSINESS:							Result += "Business"; break;
					case PRODUCT_BUSINESS_N:						Result += "Business N"; break;
					case PRODUCT_CLUSTER_SERVER:					Result += "HPC Edition"; break;
					case PRODUCT_CLUSTER_SERVER_V:					Result += "Server Hyper Core V"; break;
					case PRODUCT_CORE:								Result += "Home"; break;
					case PRODUCT_CORE_COUNTRYSPECIFIC:				Result += "Home China"; break;
					case PRODUCT_CORE_N:							Result += "Home N"; break;
					case PRODUCT_CORE_SINGLELANGUAGE:				Result += "Home Single Language"; break;
					case PRODUCT_DATACENTER_EVALUATION_SERVER:		Result += "Server Datacenter(evaluation installation)"; break;
					case PRODUCT_DATACENTER_SERVER:					Result += "Server Datacenter(full installation)"; break;
					case PRODUCT_DATACENTER_SERVER_CORE:			Result += "Server Datacenter(core installation)"; break;
					case PRODUCT_DATACENTER_SERVER_CORE_V:			Result += "Server Datacenter without Hyper - V(core installation)"; break;
					case PRODUCT_DATACENTER_SERVER_V:				Result += "Server Datacenter without Hyper - V(full installation)"; break;
					case PRODUCT_EDUCATION:							Result += "Education"; break;
					case PRODUCT_EDUCATION_N:						Result += "Education N"; break;
					case PRODUCT_ENTERPRISE:						Result += "Enterprise"; break;
					case PRODUCT_ENTERPRISE_E:						Result += "Enterprise E"; break;
					case PRODUCT_ENTERPRISE_EVALUATION:				Result += "Enterprise Evaluation"; break;
					case PRODUCT_ENTERPRISE_N:						Result += "Enterprise N"; break;
					case PRODUCT_ENTERPRISE_N_EVALUATION:			Result += "Enterprise N Evaluation"; break;
					case PRODUCT_ENTERPRISE_S:						Result += "Enterprise 2015 LTSB"; break;
					case PRODUCT_ENTERPRISE_S_EVALUATION:			Result += "Enterprise 2015 LTSB Evaluation"; break;
					case PRODUCT_ENTERPRISE_S_N:					Result += "Enterprise 2015 LTSB N"; break;
					case PRODUCT_ENTERPRISE_S_N_EVALUATION:			Result += "Enterprise 2015 LTSB N Evaluation"; break;
					case PRODUCT_ENTERPRISE_SERVER:					Result += "Server Enterprise(full installation)"; break;
					case PRODUCT_ENTERPRISE_SERVER_CORE:			Result += "Server Enterprise(core installation)"; break;
					case PRODUCT_ENTERPRISE_SERVER_CORE_V:			Result += "Server Enterprise without Hyper - V(core installation)"; break;
					case PRODUCT_ENTERPRISE_SERVER_IA64:			Result += "Server Enterprise for Itanium - based Systems"; break;
					case PRODUCT_ENTERPRISE_SERVER_V:				Result += "Server Enterprise without Hyper - V(full installation)"; break;
					case PRODUCT_ESSENTIALBUSINESS_SERVER_ADDL:		Result += "Windows Essential Server Solution Additional"; break;
					case PRODUCT_ESSENTIALBUSINESS_SERVER_ADDLSVC:	Result += "Windows Essential Server Solution Additional SVC"; break;
					case PRODUCT_ESSENTIALBUSINESS_SERVER_MGMT:		Result += "Windows Essential Server Solution Management"; break;
					case PRODUCT_ESSENTIALBUSINESS_SERVER_MGMTSVC:	Result += "Windows Essential Server Solution Management SVC"; break;
					case PRODUCT_HOME_BASIC:						Result += "Home Basic"; break;
					case PRODUCT_HOME_BASIC_E:						Result += "Not supported (Home Basic E)"; break;
					case PRODUCT_HOME_BASIC_N:						Result += "Home Basic N"; break;
					case PRODUCT_HOME_PREMIUM:						Result += "Home Premium"; break;
					case PRODUCT_HOME_PREMIUM_E:					Result += "Not supported (Home Premium E)"; break;
					case PRODUCT_HOME_PREMIUM_N:					Result += "Home Premium N"; break;
					case PRODUCT_HOME_PREMIUM_SERVER:				Result += "Windows Home Server 2011"; break;
					case PRODUCT_HOME_SERVER:						Result += "Windows Storage Server 2008 R2 Essentials"; break;
					case PRODUCT_HYPERV:							Result += "Microsoft Hyper - V Server"; break;
					case PRODUCT_IOTUAP:							Result += "IoT Core"; break;
					case PRODUCT_IOTUAPCOMMERCIAL:					Result += "IoT Core Commercial"; break;
					case PRODUCT_MEDIUMBUSINESS_SERVER_MANAGEMENT:	Result += "Windows Essential Business Server Management Server"; break;
					case PRODUCT_MEDIUMBUSINESS_SERVER_MESSAGING:	Result += "Windows Essential Business Server Messaging Server"; break;
					case PRODUCT_MEDIUMBUSINESS_SERVER_SECURITY:	Result += "Windows Essential Business Server Security Server"; break;
					case PRODUCT_MOBILE_CORE:						Result += "Mobile"; break;
					case PRODUCT_MOBILE_ENTERPRISE:					Result += "Mobile Enterprise"; break;
					case PRODUCT_MULTIPOINT_PREMIUM_SERVER:			Result += "Windows MultiPoint Server Premium(full installation)"; break;
					case PRODUCT_MULTIPOINT_STANDARD_SERVER:		Result += "Windows MultiPoint Server Standard(full installation)"; break;
					case PRODUCT_PROFESSIONAL:						Result += "Pro"; break;
					case PRODUCT_PROFESSIONAL_E:					Result += "Not supported (Pro E)"; break;
					case PRODUCT_PROFESSIONAL_N:					Result += "Pro N"; break;
					case PRODUCT_PROFESSIONAL_WMC:					Result += "Professional with Media Center"; break;
					case PRODUCT_SB_SOLUTION_SERVER:				Result += "Windows Small Business Server 2011 Essentials"; break;
					case PRODUCT_SB_SOLUTION_SERVER_EM:				Result += "Server For SB Solutions EM"; break;
					case PRODUCT_SERVER_FOR_SB_SOLUTIONS:			Result += "Server For SB Solutions"; break;
					case PRODUCT_SERVER_FOR_SB_SOLUTIONS_EM:		Result += "Server For SB Solutions EM"; break;
					case PRODUCT_SERVER_FOR_SMALLBUSINESS:			Result += "Windows Server 2008 for Windows Essential Server Solutions"; break;
					case PRODUCT_SERVER_FOR_SMALLBUSINESS_V:		Result += "Windows Server 2008 without Hyper - V for Windows Essential Server Solutions"; break;
					case PRODUCT_SERVER_FOUNDATION:					Result += "Server Foundation"; break;
					case PRODUCT_SMALLBUSINESS_SERVER:				Result += "Windows Small Business Server"; break;
					case PRODUCT_SMALLBUSINESS_SERVER_PREMIUM:		Result += "Small Business Server Premium"; break;
					case PRODUCT_SMALLBUSINESS_SERVER_PREMIUM_CORE:	Result += "Small Business Server Premium(core installation)"; break;
					case PRODUCT_SOLUTION_EMBEDDEDSERVER:			Result += "Windows MultiPoint Server"; break;
					case PRODUCT_STANDARD_EVALUATION_SERVER:		Result += "Server Standard(evaluation installation)"; break;
					case PRODUCT_STANDARD_SERVER:					Result += "Server Standard"; break;
					case PRODUCT_STANDARD_SERVER_CORE:				Result += "Server Standard(core installation)"; break;
					case PRODUCT_STANDARD_SERVER_CORE_V:			Result += "Server Standard without Hyper - V(core installation)"; break;
					case PRODUCT_STANDARD_SERVER_V:					Result += "Server Standard without Hyper - V"; break;
					case PRODUCT_STANDARD_SERVER_SOLUTIONS:			Result += "Server Solutions Premium"; break;
					case PRODUCT_STANDARD_SERVER_SOLUTIONS_CORE:	Result += "Server Solutions Premium(core installation)"; break;
					case PRODUCT_STARTER:							Result += "Starter"; break;
					case PRODUCT_STARTER_E:							Result += "Not supported (Starter E)"; break;
					case PRODUCT_STARTER_N:							Result += "Starter N"; break;
					case PRODUCT_STORAGE_ENTERPRISE_SERVER:			Result += "Storage Server Enterprise"; break;
					case PRODUCT_STORAGE_ENTERPRISE_SERVER_CORE:	Result += "Storage Server Enterprise(core installation)"; break;
					case PRODUCT_STORAGE_EXPRESS_SERVER:			Result += "Storage Server Express"; break;
					case PRODUCT_STORAGE_EXPRESS_SERVER_CORE:		Result += "Storage Server Express(core installation)"; break;
					case PRODUCT_STORAGE_STANDARD_EVALUATION_SERVER:Result += "Storage Server Standard(evaluation installation)"; break;
					case PRODUCT_STORAGE_STANDARD_SERVER:			Result += "Storage Server Standard"; break;
					case PRODUCT_STORAGE_STANDARD_SERVER_CORE:		Result += "Storage Server Standard(core installation)"; break;
					case PRODUCT_STORAGE_WORKGROUP_EVALUATION_SERVER:Result += "Storage Server Workgroup(evaluation installation)"; break;
					case PRODUCT_STORAGE_WORKGROUP_SERVER:			Result += "Storage Server Workgroup"; break;
					case PRODUCT_STORAGE_WORKGROUP_SERVER_CORE:		Result += "Storage Server Workgroup(core installation)"; break;
					case PRODUCT_ULTIMATE:							Result += "Ultimate"; break;
					case PRODUCT_ULTIMATE_E:						Result += "Not supported (Ultimate E)"; break;
					case PRODUCT_ULTIMATE_N:						Result += "Ultimate N"; break;
					case PRODUCT_UNDEFINED:							Result += "An unknown product"; break;
					case PRODUCT_WEB_SERVER:						Result += "Web Server(full installation)"; break;
					case PRODUCT_WEB_SERVER_CORE:					Result += "Web Server(core installation)"; break;
					default:										Result += "Unknown product type"; break;
					}
				}
			}
			else if (VersionInfo.dwMajorVersion == 5 && VersionInfo.dwMinorVersion == 2)
			{
				if (GetSystemMetrics(SM_SERVERR2))
				{
					Result += "Windows Server 2003 R2";
				}
				else if (VersionInfo.wSuiteMask & VER_SUITE_STORAGE_SERVER)
				{
					Result += "Windows Storage Server 2003";
				}
				else if (VersionInfo.wSuiteMask & VER_SUITE_WH_SERVER)
				{
					Result += "Windows Home Server";
				}
				else if (VersionInfo.wProductType & VER_NT_WORKSTATION && SystemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64)
				{
					Result += "Windows XP Professional x64 Edition";
				}
				else
				{
					Result += "Windows Server 2003";
				}

				if (VersionInfo.wProductType != VER_NT_WORKSTATION)
				{
					if (SystemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64)
					{
						if (VersionInfo.wSuiteMask & VER_SUITE_DATACENTER)
						{
							Result += " Datacenter Edition for Itanium-based Systems";
						}
						else if (VersionInfo.wSuiteMask & VER_SUITE_ENTERPRISE)
						{
							Result += " Enterprise Edition for Itanium-based Systems";
						}
					}
					else if (SystemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
					{
						if (VersionInfo.wSuiteMask & VER_SUITE_DATACENTER)
						{
							Result += " Datacenter x64 Edition";
						}
						else if (VersionInfo.wSuiteMask & VER_SUITE_ENTERPRISE)
						{
							Result += " Enterprise x64 Edition";
						}
						else
						{
							Result += " Standard x64 Edition";
						}
					}
					else
					{
						if (VersionInfo.wSuiteMask & VER_SUITE_COMPUTE_SERVER)
						{
							Result += " Compute Cluster Edition";
						}
						else if (VersionInfo.wSuiteMask & VER_SUITE_DATACENTER)
						{
							Result += " Datacenter Edition";
						}
						else if (VersionInfo.wSuiteMask & VER_SUITE_ENTERPRISE)
						{
							Result += " Enterprise Edition";
						}
						else if (VersionInfo.wSuiteMask & VER_SUITE_BLADE)
						{
							Result += " Web Edition";
						}
						else
						{
							Result += " Standard Edition";
						}
					}
				}
			}
			else if (VersionInfo.dwMajorVersion == 5 && VersionInfo.dwMinorVersion == 1)
			{
				Result += "Windows XP";
				if (VersionInfo.wSuiteMask & VER_SUITE_PERSONAL)
				{
					Result += " Home Edition";
				}
				else
				{
					Result += " Professional";
				}
			}
			else if (VersionInfo.dwMajorVersion == 5 && VersionInfo.dwMinorVersion == 0)
			{
				Result += "Windows 2000 ";

				if (VersionInfo.wProductType == VER_NT_WORKSTATION)
				{
					Result += " Professional";
				}
				else
				{
					if (VersionInfo.wSuiteMask & VER_SUITE_DATACENTER)
					{
						Result += " Datacenter Server";
					}
					else if (VersionInfo.wSuiteMask & VER_SUITE_ENTERPRISE)
					{
						Result += " Advanced Server";
					}
					else
					{
						Result += " Server";
					}
				}
			}

			if (strlen(VersionInfo.szCSDVersion) > 0)
			{
				Result += " ";
				Result += VersionInfo.szCSDVersion;
			}

			Result += String(" (Build ") + (int)VersionInfo.dwBuildNumber + String(")");

			if (VersionInfo.dwMajorVersion >= 6)
			{
				if (SystemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
				{
					Result += ", 64-bit";
				}
				else if (SystemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL)
				{
					Result += ", 32-bit";
				}
			}
		}

		return Result;
	});
}

String GetUsername()
{
	static CachedValue<String> Value;

	return Value.Get([]() 
	{
		char Buffer[UNLEN + 1];
		Buffer[UNLEN] = '\0';

		DWORD BufferLen = UNLEN;

		if (0 == GetUserNameA(Buffer, &BufferLen))
		{
			LogLastSystemError("GetUsername");
			return String("Unknown");
		}
		else
		{
			return String(Buffer);
		}
	});
}

Path GetSpecialPath(SpecialPath Type)
{
	switch (Type)
	{
	case SpecialPath::ExecutableFile:
		{
			static CachedValue<Path> CachedPath;

			return CachedPath.Get([]()
			{
				HMODULE hModule = GetModuleHandle(NULL);
				if (hModule)
				{
					char ModulePath[MAX_PATH + 1];
					GetModuleFileNameA(hModule, ModulePath, sizeof(ModulePath));

					return Path(ModulePath);
				}
				else
				{
					return Path("");
				}
			});
		}
	case SpecialPath::TempDirectory:
		{
			static CachedValue<Path> CachedPath;

			return CachedPath.Get([]()
			{
				char TempPath[MAX_PATH + 1];
				if (!GetTempPathA(sizeof(TempPath), TempPath))
				{
					LogLastSystemError("GetTempPath");
					return Path("");
				}

				return Path(TempPath);
			});
		}
	case SpecialPath::SaveDataDirectory:
		{
			static CachedValue<Path> CachedPath;

			return CachedPath.Get([]()
			{
				char TempPath[MAX_PATH + 1];

				if (SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, TempPath) != S_OK)
				{
					LogLastSystemError("SHGetFolderPath");
					return Path("");
				}

				return Path(TempPath);
			});
		}
	default:
		{
			ConstantAssert();
			return Path("");
		}
	}
}

}; // namespace Environment
}; // namespace Ludo