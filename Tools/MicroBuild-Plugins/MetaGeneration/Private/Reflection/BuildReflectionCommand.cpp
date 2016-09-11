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

#include "BuildTool/Private/Reflection/BuildReflectionCommand.h"

namespace Ludo {

BuildReflectionCommand::BuildReflectionCommand(GameEngine* Engine)
	: m_Engine(Engine)
{
}

String BuildReflectionCommand::GetName()
{
	return "BuildReflection";
}

void BuildReflectionCommand::PrintHelp()
{
	Log(LogTool, LogInfo, "\tBuilder BuildReflection <Build|Rebuild> <ProjectName> <SourceFolder> <OutputFolder>");
}

bool BuildReflectionCommand::ValidateArgs(const Array<String>& Args)
{
	if (Args.Length() < 5)
	{
		return false;
	}

	String Command = Args[0];
	Path SourceFolder = Args[2];
	Path OutputFolder = Args[3];
	Path PchFile = Args[4];

	if (Command != "Build" && Command != "Rebuild")
	{
		LogF(LogTool, LogError, "Command '%s' does not exist, did you mean Build or Rebuild?", Command.Data());
		return false;
	}


	if (!SourceFolder.Exists() ||
		!SourceFolder.IsDirectory())
	{
		LogF(LogTool, LogError, "Source folder '%s' does not exist.", SourceFolder.ToString().Data());
		return false;
	}


	if (OutputFolder.Exists() &&
		OutputFolder.IsFile())
	{
		LogF(LogTool, LogError, "Output folder '%s' looks to be invalid.", OutputFolder.ToString().Data());
		return false;
	}


	if (!PchFile.Exists() ||
		!PchFile.IsFile())
	{
		LogF(LogTool, LogError, "PCH file '%s' looks to be invalid.", PchFile.ToString().Data());
		return false;
	}

	return true;
}

bool BuildReflectionCommand::BuildFolder(const Path& SourceFolder, const Path& OutputFolder, const Path& BaseOutputFolder, const String& FolderId)
{
	Array<Path> Files = SourceFolder.GetFiles();
	Array<Path> Dirs = SourceFolder.GetDirectories();

	if (!OutputFolder.Exists())
	{
		if (!OutputFolder.CreateAsDirectory())
		{
			LogF(LogTool, LogError, "Output folder '%s' could not be created.", OutputFolder.ToString().Data());
			return false;
		}
	}

	for (Path BuildFile : Files)
	{
		Path SourceFilePath = SourceFolder + BuildFile;
		Path OutputFilePath = OutputFolder + BuildFile.ChangeExtension("generated.cpp");
		Path DependentsOutputFilePath = OutputFolder + BuildFile.ChangeExtension("generated.deps");

		String SanitizedFileId = (FolderId + "_" + BuildFile.ToString()).Filter("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_", "_");
		
		if (m_SourceExtensions.Contains(SourceFilePath.GetExtension()))
		{
			m_Files.Add(SanitizedFileId);

			bool bOutOfDate = false;

			if (!DependentsOutputFilePath.Exists() ||
				!OutputFilePath.Exists())
			{
				bOutOfDate = true;
			}

			else 
			{
				Time SourceFileTime = SourceFilePath.GetModifiedTime();
				Time OutputFileTIme = OutputFilePath.GetModifiedTime();
				if (SourceFileTime > OutputFileTIme)
				{
					bOutOfDate = true;
				}
				else
				{
					// Any dependencies out of date?
					File DependenciesFile;
					if (DependenciesFile.Open(DependentsOutputFilePath, FileFlags::Read).Succeeded())
					{
						TextStream DependenciesFileStream(&DependenciesFile);
						while (!DependenciesFileStream.AtEnd())
						{
							String DependentPathString;
							if (DependenciesFileStream.ReadLine(DependentPathString).Failed())
							{
								bOutOfDate = true;
								break;
							}
							else
							{
								Path DependentPath(DependentPathString);
								if (DependentPath.GetModifiedTime() > OutputFileTIme)
								{
									bOutOfDate = true;
									break;
								}
							}
						}
					}
					else
					{
						bOutOfDate = true;
					}
				}
			}
			
			if (bOutOfDate)
			{
				PendingBuildCommand Cmd;
				Cmd.SourcePath = SourceFilePath;
				Cmd.OutputPath = OutputFilePath;
				Cmd.DependentsOutputPath = DependentsOutputFilePath;
				Cmd.FileId = FolderId + "_" + BuildFile.ToString();

				m_PendingBuilds.Add(Cmd);
			}
		}
	}
	for (Path Dir : Dirs)
	{
		Path SourceFilePath = SourceFolder + Dir;
		Path OutputFilePath = OutputFolder + Dir;

		if (SourceFilePath == BaseOutputFolder)
		{
			continue;
		}

		if (!BuildFolder(SourceFilePath, OutputFilePath, BaseOutputFolder, FolderId + "_" + Dir.ToString()))
		{
			return false;
		}
	}

	return true;
}

bool BuildReflectionCommand::BuildFile(const Path& SourceFile, const Path& OutputFile, const Path& DependentsOutputPath, const String& FileId, bool bTreatAsPch)
{
	if (!bTreatAsPch)
	{
		LogF(LogTool, LogInfo, "Scanning: %s", SourceFile.ToString().Data());
	}
	else
	{
		LogF(LogTool, LogInfo, "Building PCH: %s", SourceFile.ToString().Data());
	}

	String SanitizedFileId = FileId.Filter("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_", "_");

	ReflectionFileScanner Scanner(SourceFile, OutputFile, DependentsOutputPath, SanitizedFileId, m_CompileArgs, true);
	if (Scanner.Scan().Succeeded())
	{
		if (!OutputFile.Exists())
		{
			m_NewFiles++;
		}

		if (!bTreatAsPch)
		{
			if (Scanner.Save().Succeeded())
			{
				m_CompiledFiles++;
				return true;
			}
		}
		else
		{
			m_CompiledFiles++;
			return true;
		}
	}

	m_FailedFiles++;
	return false;
}

void BuildReflectionCommand::WriteDatabase(const Path& OutputPath, const String& ProjectName, const Array<String>& Files)
{
	Log(LogTool, LogInfo, "Writing out database file ...");

	Path OutputDatabaseCppPath = OutputPath + String::Format("/%s.generated.cpp", ProjectName.Data());

	// Dump into metadata file.
	File SourceFile;

	Error Result = SourceFile.Open(OutputDatabaseCppPath, FileFlags::Write);
	if (Result.Succeeded())
	{
		TextStream SourceWriter(&SourceFile);

		SourceWriter.WriteLine("/*");
		SourceWriter.WriteLine("Ludo Game Engine");
		SourceWriter.WriteLine("Copyright (C) 2016 TwinDrills");
		SourceWriter.WriteLine("");
		SourceWriter.WriteLine("This program is free software: you can redistribute it and/or modify");
		SourceWriter.WriteLine("it under the terms of the GNU General Public License as published by");
		SourceWriter.WriteLine("the Free Software Foundation, either version 3 of the License, or");
		SourceWriter.WriteLine("(at your option) any later version.");
		SourceWriter.WriteLine("");
		SourceWriter.WriteLine("This program is distributed in the hope that it will be useful,");
		SourceWriter.WriteLine("but WITHOUT ANY WARRANTY; without even the implied warranty of");
		SourceWriter.WriteLine("MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the");
		SourceWriter.WriteLine("GNU General Public License for more details.");
		SourceWriter.WriteLine("");
		SourceWriter.WriteLine("You should have received a copy of the GNU General Public License");
		SourceWriter.WriteLine("along with this program.  If not, see <http://www.gnu.org/licenses/>.");
		SourceWriter.WriteLine("*/");
		SourceWriter.WriteLine("");
		SourceWriter.WriteLine("// Warning:");
		SourceWriter.WriteLine("//   This file is automatically generated. Do not modify directly, instead");
		SourceWriter.WriteLine("//   change the source file or modify the code in ReflectionFileScanner.cpp.");
		SourceWriter.WriteLine("//   Any changes you make will be lost on the next build.");
		SourceWriter.WriteLine("");
		SourceWriter.WriteLine("#include \"Core/Reflection/TypeModule.h\"");
		SourceWriter.WriteLine("");
		SourceWriter.WriteLine("namespace Ludo {");
		SourceWriter.WriteLine("namespace Meta {");
		SourceWriter.WriteLine("");
		for (String FileId : Files)
		{
			SourceWriter.WriteLine("extern void RegisterTypes_%s();", FileId.Data());
			SourceWriter.WriteLine("extern void UnregisterTypes_%s();", FileId.Data());
		}
		SourceWriter.WriteLine("");
		SourceWriter.WriteLine("class TypeModule_%s : public TypeModule", ProjectName.Data());
		SourceWriter.WriteLine("{");
		SourceWriter.WriteLine("public:");
		SourceWriter.Indent();

			SourceWriter.WriteLine("TypeModule_%s()", ProjectName.Data());
			SourceWriter.WriteLine("{");
			SourceWriter.Indent();
				SourceWriter.WriteLine("RegisterModule(this);");
			SourceWriter.Undent();
			SourceWriter.WriteLine("}");
			SourceWriter.WriteLine("");

			SourceWriter.WriteLine("virtual ~TypeModule_%s()", ProjectName.Data());
			SourceWriter.WriteLine("{");
			SourceWriter.Indent();
				SourceWriter.WriteLine("UnregisterModule(this);");
			SourceWriter.Undent();
			SourceWriter.WriteLine("}");
			SourceWriter.WriteLine("");

			SourceWriter.WriteLine("virtual void RegisterTypes() override", ProjectName.Data());
			SourceWriter.WriteLine("{");
			SourceWriter.Indent();
			for (String FileId : Files)
			{
				SourceWriter.WriteLine("RegisterTypes_%s();", FileId.Data());
			}
			SourceWriter.Undent();
			SourceWriter.WriteLine("}");
			SourceWriter.WriteLine("");
			SourceWriter.WriteLine("virtual void UnregisterTypes() override", ProjectName.Data());
			SourceWriter.WriteLine("{");
			SourceWriter.Indent();
			for (String FileId : Files)
			{
				SourceWriter.WriteLine("UnregisterTypes_%s();", FileId.Data());
			}
			SourceWriter.Undent();
			SourceWriter.WriteLine("}");
			SourceWriter.WriteLine("");
			SourceWriter.WriteLine("virtual StringId GetName() const override", ProjectName.Data());
			SourceWriter.WriteLine("{");
			SourceWriter.Indent();
				SourceWriter.WriteLine("static StringId Name = StringId::Create(\"%s\");", ProjectName.Data());
				SourceWriter.WriteLine("return Name;");
				SourceWriter.Undent();
			SourceWriter.WriteLine("}");

		SourceWriter.Undent();
		SourceWriter.WriteLine("};");
		SourceWriter.WriteLine("");
		SourceWriter.WriteLine("void Load_%s_Metadata() { static TypeModule_%s g_TypeModuleRegister_%s; }", ProjectName.Data(), ProjectName.Data(), ProjectName.Data());
		SourceWriter.WriteLine("");
		SourceWriter.WriteLine("} // namespace Meta");
		SourceWriter.WriteLine("} // namespace Ludo");
	}
}

String BuildReflectionCommand::GenerateConfigurationHash(const String& ProjectName, const Path& OutputPath, const Array<String>& Configuration)
{
	String Concat = ProjectName;
	Concat += String(Version);
	Concat += OutputPath.ToString();
	for (const String& Config : Configuration)
	{
		Concat += Config;
	}
	return String::HexToString(Concat.ToHash());
}

bool BuildReflectionCommand::CheckConfigurationMatches(const String& ProjectName, const Path& OutputPath, const Array<String>& Configuration)
{
	Log(LogTool, LogInfo, "Reading configuration meta file ...");

	Path OutputConfigPath = OutputPath + String::Format("/%s.generated.hash", ProjectName.Data());

	String ConfigurationHash = GenerateConfigurationHash(ProjectName, OutputPath, Configuration);

	File ConfigFile;

	Error Result = ConfigFile.Open(OutputConfigPath, FileFlags::Read);
	if (Result.Succeeded())
	{
		TextStream Stream(&ConfigFile);
		String Line;

		if (Stream.ReadLine(Line).Succeeded() && Line == ConfigurationHash)
		{
			return true;
		}
	}

	Log(LogTool, LogError, "Failed to read configuration file.");
	return false;
}

void BuildReflectionCommand::WriteConfiguration(const String& ProjectName, const Path& OutputPath, const Array<String>& Configuration)
{
	Log(LogTool, LogInfo, "Writing configuration file ...");

	Path OutputConfigPath = OutputPath + String::Format("/%s.generated.hash", ProjectName.Data());

	String ConfigurationHash = GenerateConfigurationHash(ProjectName, OutputPath, Configuration);

	File ConfigFile;

	Error Result = ConfigFile.Open(OutputConfigPath, FileFlags::Write);
	if (Result.Succeeded())
	{
		TextStream Stream(&ConfigFile);
		Stream.WriteLine("%s", ConfigurationHash.Data());
		return;
	}

	Log(LogTool, LogError, "Failed to write configuration file.");
}

bool BuildReflectionCommand::Run(const Array<String>& Args)
{
	String BuildCommand = Args[0];
	String ProjectName = Args[1];
	Path SourceFolder = Args[2];
	Path OutputFolder = Args[3];
	Path PchFile = Args[4];

	m_CompiledFiles = 0;
	m_FailedFiles = 0;
	m_NewFiles = 0;

	m_SourceExtensions.Add("hpp");
	m_SourceExtensions.Add("h");

	m_CompileArgs.Add("-Wmicrosoft");
	m_CompileArgs.Add("-Wunknown-pragmas");
	m_CompileArgs.Add("-Wno-pragma-once-outside-header");
	m_CompileArgs.Add("-DLD_REFLECTION_GENERATOR=1");
	m_CompileArgs.Add("-fms-compatibility-version=19.00.22609");
	m_CompileArgs.Add("-xc++-header");
	m_CompileArgs.Add("-std=c++14"); // Rather not do this, but VS has a bunch of headers that require C++14.

//	m_CompileArgs.Push("-Weverything");

	for (int i = 5; i < Args.Length(); i++)
	{
		m_CompileArgs.Add(Args[i]);
	}

	Log(LogTool, LogInfo, "");
	Log(LogTool, LogInfo, "Building reflection for:");
	LogF(LogTool, LogInfo, "\tCommand: %s", BuildCommand.Data());
	LogF(LogTool, LogInfo, "\tProject: %s", ProjectName.Data());
	LogF(LogTool, LogInfo, "\tSource: %s", SourceFolder.ToString().Data());
	LogF(LogTool, LogInfo, "\tOutput: %s", OutputFolder.ToString().Data());
	LogF(LogTool, LogInfo, "\tPCH File: %s", PchFile.ToString().Data());
	for (int i = 0; i < m_CompileArgs.Length(); i++)
	{
		LogF(LogTool, LogInfo, "\tCompile Args[%.2i]: %s", i, m_CompileArgs[i].Data());
	}
	Log(LogTool, LogInfo, "");

	// Check if we have changed configuration since last time the data was built.
	if (BuildCommand != "Rebuild")
	{
		if (OutputFolder.Exists())
		{
			if (!CheckConfigurationMatches(ProjectName, OutputFolder, m_CompileArgs))
			{
				LogF(LogTool, LogWarning, "Build configuration has changed since last build, doing full rebuild.");
				BuildCommand = "Rebuild";
			}
		}
	}

	if (BuildCommand == "Rebuild")
	{
		LogF(LogTool, LogInfo, "Deleting existing reflection data.");
		if (OutputFolder.Exists())
		{
			OutputFolder.Delete();
		}
	}

	Log(LogTool, LogInfo, "");

	// Figure out what we need to build.
	LogF(LogTool, LogInfo, "Gathering files to scan.");
	if (!BuildFolder(SourceFolder, OutputFolder, OutputFolder, ProjectName))
	{
		return false;
	}

	// No files?
	if (m_PendingBuilds.Length() == 0)
	{
		LogF(LogTool, LogSuccess, "All files up to date.");
		return true;
	}
	else
	{
		LogF(LogTool, LogInfo, "%i files out of date.", m_PendingBuilds.Length());
	}

	// Create a job manager to build with.
	JobScheduler* BuildPool = LD_ARENA_NEW(LibCMemoryArena, JobScheduler, Environment::GetConcurrencyFactor());

	int Counter = 0;
	TimeSpan Start = Time::AppDuration();
	bool bFailedToBuild = false;

	// Build individual files multi-threaded and pass in the PCH output.
	if (!bFailedToBuild)
	{
		ParallelFor<PendingBuildCommand>(BuildPool, m_PendingBuilds, [this, &Counter, &bFailedToBuild] (PendingBuildCommand& Cmd) {
			Atomic::Increment(&Counter);
			if (BuildFile(Cmd.SourcePath, Cmd.OutputPath, Cmd.DependentsOutputPath, Cmd.FileId, false))
			{
				return ParallelForTaskResult::Continue;
			}		
			else
			{
				bFailedToBuild = true;
				return ParallelForTaskResult::Break;
			}
		});
	}

	TimeSpan End = Time::AppDuration() - Start;
	if (bFailedToBuild)
	{
		LogF(LogTool, LogError, "Scanned %i files in %.2fs, but some failed to compile.", Counter, End.GetTotalMilliseconds() / 1000.0f);
	}
	else
	{
		LogF(LogTool, LogSuccess, "Scanned %i files in %.2fs.", Counter, End.GetTotalMilliseconds() / 1000.0f);
	}

	LD_ARENA_DELETE(LibCMemoryArena, BuildPool);

	// We failed :(
	if (bFailedToBuild)
	{
		return false;
	}

	// Write out a "global" meta file database for the project.
	if (m_CompiledFiles > 0)
	{
		Start = Time::AppDuration();
		WriteDatabase(OutputFolder, ProjectName, m_Files);
		WriteConfiguration(ProjectName, OutputFolder, m_CompileArgs);
		End = Time::AppDuration() - Start;

		LogF(LogTool, LogSuccess, "Database and config written in %.2fs.", End.GetTotalMilliseconds() / 1000.0f);
	}

	// If we have any new files we need to update the solution before building.
	if (m_NewFiles > 0)
	{
		LogF(LogTool, LogInfo, "Regenerating project files due to newly created meta files ...");

		Start = Time::AppDuration();

		Path BuildDirectory = m_Engine->GetEnginePath(EnginePath::RootDirectory) + String("Build");

		FileMutex lock_file(BuildDirectory + String("lock"));

		Process Proc;
#if defined(LD_PLATFORM_WINDOWS)
		Error GenerateResult = Proc.Open(BuildDirectory + String("GenerateProject.Windows.bat"), BuildDirectory, {}, false);
#elif  defined(LD_PLATFORM_LINUX)
		Error GenerateResult = Proc.Open(BuildDirectory + String("GenerateProject.Linux.sh"), BuildDirectory, {}, false);
#elif  defined(LD_PLATFORM_MACOS)
		Error GenerateResult = Proc.Open(BuildDirectory + String("GenerateProject.Macos.sh"), BuildDirectory, {}, false);
#endif

		bool bSuccess = false;

		if (GenerateResult.Succeeded())
		{
			Proc.Wait();
			if (Proc.GetExitCode() == 0)
			{
				bSuccess = true;
			}
		}

		if (bSuccess)
		{
			End = Time::AppDuration() - Start;
			LogF(LogTool, LogSuccess, "Regenerated project files in %.2fs.", End.GetTotalMilliseconds() / 1000.0f);
		}
		else
		{
			LogF(LogTool, LogError, "Failed to regenerate project files.");
			return false;
		}
	}

	return true;
}

}; // namespace Ludo