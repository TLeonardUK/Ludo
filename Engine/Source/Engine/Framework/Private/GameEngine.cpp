/*
Ludo Game Engine
Copyright (C) 2016 TwinDrills

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "PCH.h"

#include "Framework/Public/GameEngine.h"

namespace Ludo {

extern Error CreateEngineCore(EngineCore** Result);
extern Error DeleteEngineCore(EngineCore** Result);

int LudoEntryPoint()
{
	GameEngine engine;
	return engine.EntryPoint();
}

void GameEngine::InitPlatform()
{
	Environment::Init();
}

void GameEngine::TermPlatform()
{
	Environment::Term();
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
	Environment::PreInit();

	StringArgumentList Args;
	Args.Add("Changelist", LD_BUILD_CHANGELIST);
	Args.Add("Branch", LD_BUILD_BRANCH);

	Log(LogEngine, LogInfo, "Ludo Engine");
	Log(LogEngine, LogInfo, "Copyright (C) TwinDrills, All Rights Reserved");
	Log(LogEngine, LogInfo, "");
	LogF(LogEngine, LogInfo, "Changelist {Changelist}, Branch {Branch}", Args);
	Log(LogEngine, LogInfo, "Compiled on " LD_BUILD_TIMESTAMP);
	Log(LogEngine, LogInfo, "");
}

void GameEngine::TermLogging()
{
}

// Pointer to the function that will load linked metadata, we do it this way to try and avoid
// compile errors if building a bootstrap build before the reflection generator has run.
typedef void (*RegisterLinkedMetadataFunction)();

void GameEngine::InitReflection()
{
	Log(LogEngine, LogInfo, "Loading engine metadata ...");

	// Register primitive types.
	Register_Primitive_Metadata();

	// Register project linked metadata.
	Module* Self = Module::Self();	
	RegisterLinkedMetadataFunction RegisterFunction = Self->GetFunction<RegisterLinkedMetadataFunction>("Register_Linked_Metadata");
	if (RegisterFunction != nullptr)
	{
		RegisterFunction();
	}
	else
	{
		Log(LogEngine, LogInfo, "Looks like no metadata has been linked. Bootstrap build?");
	}

	for (TypeModule* Mod : TypeModule::GetModules())
	{
		Mod->RegisterTypes();
	}
}

void GameEngine::TermReflection()
{
	for (TypeModule* Mod : TypeModule::GetModules())
	{
		Mod->UnregisterTypes();
	}
	for (TypeModule* Mod : TypeModule::GetModules())
	{
		TypeModule::UnregisterModule(Mod);
	}
}

void GameEngine::EmitBuildInfo()
{
	SizeStringArgumentFormatter SizeFormatter;

	StringArgumentList Args;
	Args.Add("ExecutablePath", Environment::GetSpecialPath(Environment::SpecialPath::ExecutableFile).ToString());
	Args.Add("Username", Environment::GetUsername());
	Args.Add("PlatformName", Environment::GetPlatformName());
	Args.Add("ConcurrencyFactor", Environment::GetConcurrencyFactor());

	Log(LogEngine, LogInfo, "Machine Specs:");
	LogF(LogEngine, LogInfo, "\tUsername: {Username}", Args);
	LogF(LogEngine, LogInfo, "\tExecutable Path: {ExecutablePath}", Args);
	LogF(LogEngine, LogInfo, "\tPlatform Name: {PlatformName}", Args);
	LogF(LogEngine, LogInfo, "\tConcurrency Factor: {ConcurrencyFactor}", Args);

	for (int i = 0; i < Environment::GetCpuCount(); i++)
	{
		Environment::CpuInfo CpuInfo = Environment::GetCpuInfo(i);

		String FeatureString;
		for (Environment::CpuFeature Feature : CpuInfo.Features)
		{
			if (!FeatureString.IsEmpty())
			{
				FeatureString += ", ";
			}
			FeatureString += Environment::CpuFeatureName[(int)Feature];
		}

		Args.Add("CpuIndex", i);
		Args.Add("CpuName", CpuInfo.Name);
		Args.Add("CpuCoreCount", CpuInfo.CoreCount);
		Args.Add("CpuThreadCount", CpuInfo.ThreadCount);
		Args.Add("CpuFeatures", FeatureString);
		LogF(LogEngine, LogInfo, "\tCPU[{CpuIndex}]: Name={CpuName}, Cores={CpuCoreCount}, Threads={CpuThreadCount}, Features={CpuFeatures}", Args);
	}

	for (int i = 0; i < Environment::GetGpuCount(); i++)
	{
		Environment::GpuInfo GpuInfo = Environment::GetGpuInfo(i);

		Args.Add("GpuIndex", i);
		Args.Add("GpuName", GpuInfo.Name);
		Args.Add("GpuTotalMemory", GpuInfo.TotalVRam, &SizeFormatter);
		LogF(LogEngine, LogInfo, "\tGPU[{GpuIndex}]: Name={GpuName}, VRam={GpuTotalMemory}", Args);
	}

	{
		Environment::RamInfo MemoryInfo = Environment::GetRamInfo();

		Args.Add("TotalPhysicalMemory", MemoryInfo.TotalPhysicalRam, &SizeFormatter);
		Args.Add("TotalVirtualMemory", MemoryInfo.TotalVirtualRam, &SizeFormatter);
		Args.Add("TotalPageMemory", MemoryInfo.TotalPageRam, &SizeFormatter);
		LogF(LogEngine, LogInfo, "\tRAM: Physical={TotalPhysicalMemory}, Virtual={TotalVirtualMemory}, PageFile={TotalPageMemory}", Args);
	}

	Log(LogEngine, LogInfo, "");
}

Error GameEngine::LoadEngineCore()
{
	Log(LogEngine, LogInfo, "Loading engine core.");

	Error Result = CreateEngineCore(&m_EngineCore);
	if (Result.Failed())
	{
		LogF(LogEngine, LogInfo, "Failed to load engine core CreateEngineCore returned %s.", Result.ToString());
		return Error(ErrorType::Failure);
	}

	return Error(ErrorType::Success);
}

void GameEngine::UnloadEngineCore()
{
	Log(LogEngine, LogInfo, "Unloading engine core.");

	Error Result = m_EngineCore->Term();
	if (Result.Failed())
	{
		LogF(LogEngine, LogInfo, "Failed to unload engine core EngineCore->Term returned %s.", Result.ToString());
		return;
	}

	Result = DeleteEngineCore(&m_EngineCore);
	if (Result.Failed())
	{
		LogF(LogEngine, LogInfo, "Failed to unload engine core DeleteEngineCore returned %s.", Result.ToString());
		return;
	}
}

Error GameEngine::UpdatePaths()
{
	m_RootDirectory = Environment::GetSpecialPath(Environment::SpecialPath::ExecutableFile).GetDirectory();
	while (true)
	{
		Path BinariesFolder = m_RootDirectory + String("Binaries");
		Path BuildFolder = m_RootDirectory + String("Build");
		Path EngineFolder = m_RootDirectory + String("Engine");
		Path ProjectsFolder = m_RootDirectory + String("Projects");
		Path ToolsFolder = m_RootDirectory + String("Tools");

		if (BinariesFolder.Exists() &&
			BuildFolder.Exists() &&
			EngineFolder.Exists() &&
			ProjectsFolder.Exists() &&
			ToolsFolder.Exists())
		{
			break;
		}

		if (m_RootDirectory.IsRoot())
		{
			LogF(LogEngine, LogError, "Failed to find root directory, unable to continue, please check installation is not corrupt.");
		}

		m_RootDirectory = m_RootDirectory.GetDirectory();
	}

	m_ProjectDirectory = Path(""); // TODO
	m_EngineDirectory = m_RootDirectory + String("Engine");

	return ErrorType::Success;
}

int GameEngine::EntryPoint()
{
	InitLogging();
	InitReflection();
	InitPlatform();

	Log(LogEngine, LogInfo, "");

	int ExitCode = 0;

	Error Result = UpdatePaths();
	if (Result.Succeeded())
	{
		Result = LoadEngineCore();
		if (Result.Succeeded())
		{
			if (!m_EngineCore->UseFastInit())
			{
				EmitBuildInfo();
			}

			Log(LogEngine, LogInfo, "Initializing engine core.");

			Result = m_EngineCore->Init(this);
			if (Result.Succeeded())
			{
				Log(LogEngine, LogInfo, "Successfully initialized engine core.");
				ExitCode = m_EngineCore->Run();
			}		
			else
			{
				LogF(LogEngine, LogInfo, "Failed to initialize engine core, EngineCore->Init returned %s.", Result.ToString());
			}

			UnloadEngineCore();
		}
	}

	TermPlatform();
	TermLogging();
	TermReflection();

	return ExitCode;
}

Path GameEngine::GetEnginePath(EnginePath PathType)
{
	switch (PathType)
	{
	case EnginePath::ExecutableFile:		return Environment::GetSpecialPath(Environment::SpecialPath::ExecutableFile);
	case EnginePath::TempDirectory:			return Environment::GetSpecialPath(Environment::SpecialPath::TempDirectory);
	case EnginePath::SaveDataDirectory:		return Environment::GetSpecialPath(Environment::SpecialPath::SaveDataDirectory);
	case EnginePath::ProjectDirectory:		return m_ProjectDirectory;
	case EnginePath::EngineDirectory:		return m_EngineDirectory;
	case EnginePath::RootDirectory:			return m_RootDirectory;
	default:
		{
			NotImplemented();
			return Path();
		}
	}
}

}; // namespace Ludo