// Copyright 2016 TwinDrills

#include "Framework/GameEngine.h"
#include "Core/Testing/Test.h"
#include "Core/Debug/LogStdOutput.h"
#include "Core/Platform/Platform.h"
#include "Core/Platform/Environment.h"
#include "Core/Types/String.h"
#include "Core/Types/StringFormatters/SizeStringArgumentFormatter.h"

namespace Ludo {

Error LudoEntryPoint()
{
	GameEngine engine;
	return engine.EntryPoint();
}

void GameEngine::InitPlatform()
{
	Platform::Init();
}

void GameEngine::TermPlatform()
{
	Platform::Term();
}

void GameEngine::InitLogging()
{
#if !defined(LD_SHIPPING_BUILD)
	// Everything during development.
	LogOutput::SetLogVerbosity(MAX_LOG_VERBOSITY); 
#else
	// Errors only during shipping.
	LogOutput::SetLogVerbosity(1); 
#endif

	// Get the std output up and running.
	m_StdLoggingOutput.Register();

	// Get the std output up and running.
	Platform::PreInit();

	StringArgumentList Args;
	Args.Add("Changelist", LD_BUILD_CHANGELIST);
	Args.Add("Branch", LD_BUILD_BRANCH);

	Log(LogEngine, LogInfo, "Ludo Engine");
	Log(LogEngine, LogInfo, "Copyright (C) TwinDrills, All Rights Reserved");
	Log(LogEngine, LogInfo, "");
	Log(LogEngine, LogInfo, "Changelist {Changelist}, Branch {Branch}", Args);
	Log(LogEngine, LogInfo, "Compiled on " LD_BUILD_TIMESTAMP);
	Log(LogEngine, LogInfo, "");
}

void GameEngine::TermLogging()
{
}

void GameEngine::EmitBuildInfo()
{
	SizeStringArgumentFormatter SizeFormatter;

	StringArgumentList Args;
	Args.Add("ExecutablePath", Environment::GetSpecialPath(SpecialPath::ExecutableFile).ToString());
	Args.Add("Username", Environment::GetUsername());
	Args.Add("PlatformName", Environment::GetPlatformName());
	Args.Add("ConcurrencyFactor", Environment::GetConcurrencyFactor());

	Log(LogEngine, LogInfo, "");
	Log(LogEngine, LogInfo, "Machine Specs:");
	Log(LogEngine, LogInfo, "\tUsername: {Username}", Args);
	Log(LogEngine, LogInfo, "\tExecutable Path: {ExecutablePath}", Args);
	Log(LogEngine, LogInfo, "\tPlatform Name: {PlatformName}", Args);
	Log(LogEngine, LogInfo, "\tConcurrency Factor: {ConcurrencyFactor}", Args);

	for (int i = 0; i < Environment::GetCpuCount(); i++)
	{
		CpuInfo CpuInfo = Environment::GetCpuInfo(i);

		String FeatureString;
		for (CpuFeature Feature : CpuInfo.Features)
		{
			if (!FeatureString.IsEmpty())
			{
				FeatureString += ", ";
			}
			FeatureString += CpuFeatureName[(int)Feature];
		}

		Args.Add("CpuIndex", i);
		Args.Add("CpuName", CpuInfo.Name);
		Args.Add("CpuCoreCount", CpuInfo.CoreCount);
		Args.Add("CpuThreadCount", CpuInfo.ThreadCount);
		Args.Add("CpuFeatures", FeatureString);
		Log(LogEngine, LogInfo, "\tCPU[{CpuIndex}]: Name={CpuName}, Cores={CpuCoreCount}, Threads={CpuThreadCount}, Features={CpuFeatures}", Args);
	}

	for (int i = 0; i < Environment::GetGpuCount(); i++)
	{
		GpuInfo GpuInfo = Environment::GetGpuInfo(i);

		Args.Add("GpuIndex", i);
		Args.Add("GpuName", GpuInfo.Name);
		Args.Add("GpuTotalMemory", GpuInfo.TotalVRam, &SizeFormatter);
		Log(LogEngine, LogInfo, "\tGPU[{GpuIndex}]: Name={GpuName}, VRam={GpuTotalMemory}", Args);
	}

	{
		RamInfo MemoryInfo = Environment::GetRamInfo();

		Args.Add("TotalPhysicalMemory", MemoryInfo.TotalPhysicalRam, &SizeFormatter);
		Args.Add("TotalVirtualMemory", MemoryInfo.TotalVirtualRam, &SizeFormatter);
		Args.Add("TotalPageMemory", MemoryInfo.TotalPageRam, &SizeFormatter);
		Log(LogEngine, LogInfo, "\tRAM: Physical={TotalPhysicalMemory}, Virtual={TotalVirtualMemory}, PageFile={TotalPageMemory}", Args);
	}

	Log(LogEngine, LogInfo, "");
}

Error GameEngine::EntryPoint()
{
	InitLogging();
	InitPlatform();
	EmitBuildInfo();

#if !defined(LD_SHIPPING_BUILD)
	Test::RunAll();
#endif

	TermPlatform();
	TermLogging();

	return ErrorType::Success;
}

}; // namespace Ludo