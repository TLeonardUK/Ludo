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

#include "BuildTool/Private/Reflection/LinkReflectionCommand.h"

namespace Ludo {

LinkReflectionCommand::LinkReflectionCommand(GameEngine* Engine)
	: m_Engine(Engine)
{
}

String LinkReflectionCommand::GetName()
{
	return "LinkReflection";
}

void LinkReflectionCommand::PrintHelp()
{
	Log(LogTool, LogInfo, "\tBuilder LinkReflection <Link|Relink> <ProjectName> <SourceFolder> <OutputFolder> <LinkedProjectNames>");
}

bool LinkReflectionCommand::ValidateArgs(const Array<String>& Args)
{
	if (Args.Length() < 4)
	{
		return false;
	}

	String Command = Args[0];
	if (Command != "Link" && Command != "Relink")
	{
		LogF(LogTool, LogError, "Command '%s' does not exist, did you mean Link or Relink?", Command.Data());
		return false;
	}

	Path SourceFolder = Args[2];

	if (!SourceFolder.Exists() ||
		!SourceFolder.IsDirectory())
	{
		LogF(LogTool, LogError, "Source folder '%s' does not exist.", SourceFolder.ToString().Data());
		return false;
	}

	Path OutputFolder = Args[3];

	if (OutputFolder.Exists() &&
		OutputFolder.IsFile())
	{
		LogF(LogTool, LogError, "Output folder '%s' looks to be invalid.", OutputFolder.ToString().Data());
		return false;
	}

	return true;
}

String LinkReflectionCommand::GenerateConfigurationHash(const String& ProjectName, const Path& OutputPath, const Array<String>& Configuration)
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

bool LinkReflectionCommand::CheckConfigurationMatches(const String& ProjectName, const Path& OutputPath, const Array<String>& Configuration)
{
	Log(LogTool, LogInfo, "Reading configuration meta file ...");

	Path OutputConfigPath = OutputPath + String::Format("/%s.generated.linked.hash", ProjectName.Data());

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

void LinkReflectionCommand::WriteConfiguration(const String& ProjectName, const Path& OutputPath, const Array<String>& Configuration)
{
	Log(LogTool, LogInfo, "Writing configuration file ...");

	Path OutputConfigPath = OutputPath + String::Format("/%s.generated.linked.hash", ProjectName.Data());

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

Error LinkReflectionCommand::WriteDatabase(const String& ProjectName, const Path& OutputPath, const Array<String>& LinkedProjects)
{
	File DatabaseFile;

	Error Result = DatabaseFile.Open(OutputPath, FileFlags::Write);
	if (Result.Succeeded())
	{
		TextStream SourceWriter(&DatabaseFile);

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
		SourceWriter.WriteLine("extern void Load_%s_Metadata();", ProjectName.Data());
		for (const String& Project : LinkedProjects)
		{
			SourceWriter.WriteLine("extern void Load_%s_Metadata();", Project.Data());
		}
		SourceWriter.WriteLine("");
		SourceWriter.WriteLine("extern \"C\"");
		SourceWriter.WriteLine("{");
		SourceWriter.Indent();
			SourceWriter.WriteLine("DLLEXPORT void Register_Linked_Metadata()");
			SourceWriter.WriteLine("{");
			SourceWriter.Indent();
			SourceWriter.WriteLine("Load_%s_Metadata();", ProjectName.Data());
			for (const String& Project : LinkedProjects)
			{
				SourceWriter.WriteLine("Load_%s_Metadata();", Project.Data());
			}
			SourceWriter.Undent();
			SourceWriter.WriteLine("}");
		SourceWriter.Undent();
		SourceWriter.WriteLine("}");
		SourceWriter.WriteLine("");
		SourceWriter.WriteLine("} // namespace Meta");
		SourceWriter.WriteLine("} // namespace Ludo");

		return ErrorType::Success;
	}
	else
	{
		return Result;
	}
}

bool LinkReflectionCommand::Run(const Array<String>& Args)
{
	String ProjectName = Args[1];
	String BuildCommand = Args[0];
	Path SourceFolder = Args[2];
	Path OutputFolder = Args[3];
	Array<String> LinkedProjects = Args.Slice(4, Args.Length());

	Log(LogTool, LogInfo, "");
	Log(LogTool, LogInfo, "Linking reflection for:");
	LogF(LogTool, LogInfo, "\tCommand: %s", BuildCommand.Data());
	LogF(LogTool, LogInfo, "\tProject: %s", ProjectName.Data());
	LogF(LogTool, LogInfo, "\tSource: %s", SourceFolder.ToString().Data());
	LogF(LogTool, LogInfo, "\tOutput: %s", OutputFolder.ToString().Data());
	LogF(LogTool, LogInfo, "\tLinked Projects: %s", String(",").Join(LinkedProjects));
	for (int i = 0; i < m_CompileArgs.Length(); i++)
	{
		LogF(LogTool, LogInfo, "\tCompile Args[%.2i]: %s", i, m_CompileArgs[i].Data());
	}
	Log(LogTool, LogInfo, "");

	// Check if we have changed configuration since last time the data was built.
	if (BuildCommand != "Relink")
	{
		if (OutputFolder.Exists())
		{
			if (!CheckConfigurationMatches(ProjectName, OutputFolder, m_CompileArgs))
			{
				LogF(LogTool, LogWarning, "Build configuration has changed since last build, doing full relink.");
				BuildCommand = "Relink";
			}
		}
	}

	if (BuildCommand != "Relink")
	{
		LogF(LogTool, LogSuccess, "All files up to date.");
		return true;
	}

	Log(LogTool, LogInfo, "");

	// Dump out our config file.
	Path OutputLinkFile = OutputFolder + String::Format("/%s.generated.linked.cpp", ProjectName.Data());

	bool bRequireProjectFileRebuild = !OutputLinkFile.Exists();

	if (WriteDatabase(ProjectName, OutputLinkFile, LinkedProjects).Succeeded())
	{
		LogF(LogTool, LogSuccess, "Linked successfully.");
	}
	else
	{
		LogF(LogTool, LogError, "Link failed.");
		return false;
	}

	// Write config hash.
	WriteConfiguration(ProjectName, OutputFolder, m_CompileArgs);

	// If we have any new files we need to update the solution before building.
	if (bRequireProjectFileRebuild)
	{
		LogF(LogTool, LogInfo, "Regenerating project files due to newly created meta files ...");

		TimeSpan Start = Time::AppDuration();

		Path BuildDirectory = m_Engine->GetEnginePath(EnginePath::RootDirectory) + String("Build");

		FileMutex lock_file(BuildDirectory + String("lock"));

		Process Proc;
#if defined(LD_PLATFORM_WINDOWS)
		Error Result = Proc.Open(BuildDirectory + String("GenerateProject.Windows.bat"), BuildDirectory, {}, false);
#elif  defined(LD_PLATFORM_LINUX)
		Error Result = Proc.Open(BuildDirectory + String("GenerateProject.Linux.sh"), BuildDirectory, {}, false);
#elif  defined(LD_PLATFORM_MACOS)
		Error Result = Proc.Open(BuildDirectory + String("GenerateProject.Macos.sh"), BuildDirectory, {}, false);
#endif

		bool bSuccess = false;

		if (Result.Succeeded())
		{
			Proc.Wait();
			if (Proc.GetExitCode() == 0)
			{
				bSuccess = true;
			}
		}

		if (bSuccess)
		{
			TimeSpan End = Time::AppDuration() - Start;
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