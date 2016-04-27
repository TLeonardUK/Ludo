// Copyright 2016 TwinDrills
#pragma once

#include "Core/Types/String.h"
#include "Core/Platform/Path.h"

namespace Ludo {
namespace Environment {

enum class CpuFeature
{
	MMX,
	SSE,
	SSE2,
	SSE3,
	SSSE3,
	SSE41,
	SSE42,
	AES,
	AVX,
	FMA3,
	RDRAND,
	AVX2,
	BMT1,
	BMT2,
	ADX,
	SHA,
	PREFETCHWT1,
	AVX512F,
	AVX512CD,
	AVX512PF,
	AVX512ER,
	AVX512VL,
	AVX512BW,
	AVX512DQ,
	AVX512IFMA,
	AVX512VBMI,
	X64,
	ABM,
	SSE4a,
	FMA4,
	XOP,

	Count
};

extern const char* CpuFeatureName[(int)CpuFeature::Count];

enum class SpecialPath
{
	ExecutableFile,		// Path to the executable path.
	TempDirectory,		// Path to a temporary directory that the local user can write to. 
	SaveDataDirectory,	// Path to a directory that save data should be stored in.
};

struct CpuInfo
{
	String				Name;
	int					CoreCount;
	int					ThreadCount;
	Array<CpuFeature>	Features;
};

struct GpuInfo
{
	String				Name;
	uint64_t			TotalVRam;
};

struct RamInfo
{
	uint64_t			TotalPhysicalRam;
	uint64_t			TotalVirtualRam;
	uint64_t			TotalPageRam;
};

/// \brief TODO
int GetCpuCount();

/// \brief TODO
int GetGpuCount();

/// \brief TODO
CpuInfo GetCpuInfo(const int CpuIndex);

/// \brief TODO
GpuInfo GetGpuInfo(const int GpuIndex);

/// \brief TODO
RamInfo GetRamInfo();

/// \brief TODO
int GetConcurrencyFactor();

/// \brief TODO
String GetPlatformName();

/// \brief TODO
String GetUsername();

/// \brief TODO
Path GetSpecialPath(SpecialPath Type);

}; // namespace Environment
}; // namespace Ludo