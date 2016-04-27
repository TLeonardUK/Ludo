// Copyright 2016 TwinDrills
#pragma once

#include "Core/Testing/Platform/Test_Path.h"
#include "Core/Platform/Path.h"

namespace Ludo {

void TestPath::Run() const
{
	Path TestAbs("C:\\FolderA/FolderB\\File.Ext");
	Path TestRel("//.Compiler/Bleh.ext2.compiled");

	String TestAbsToString = TestAbs.ToString();
	String TestRelToString = TestRel.ToString();

	Assert(TestAbsToString == (String("C:") + Path::Seperator + "FolderA" + Path::Seperator + "FolderB" + Path::Seperator + "File.Ext"));
	Assert(TestRelToString == (String(".Compiler") + Path::Seperator + "Bleh.ext2.compiled"));

	String ChangedExtension = TestAbs.ChangeExtension("Derp").ToString();
	String ChangedBasename = TestAbs.ChangeBaseName("Derp").ToString();
	String ChangedFilename = TestAbs.ChangeFilename("Derp.Herp").ToString();
	String ChangedDirectory = TestAbs.ChangeDirectory(Path("D:/Test/1")).ToString();
	String ChangedMount = TestAbs.ChangeMount("X").ToString();

	Assert(ChangedExtension == (String("C:") + Path::Seperator + "FolderA" + Path::Seperator + "FolderB" + Path::Seperator + "File.Derp"));
	Assert(ChangedBasename == (String("C:") + Path::Seperator + "FolderA" + Path::Seperator + "FolderB" + Path::Seperator + "Derp.Ext"));
	Assert(ChangedFilename == (String("C:") + Path::Seperator + "FolderA" + Path::Seperator + "FolderB" + Path::Seperator + "Derp.Herp"));
	Assert(ChangedDirectory == (String("D:") + Path::Seperator + "Test" + Path::Seperator + "1" + Path::Seperator + "File.Ext"));
	Assert(ChangedMount == (String("X:") + Path::Seperator + "FolderA" + Path::Seperator + "FolderB" + Path::Seperator + "File.Ext"));

	Path Concatinated = TestAbs + TestRel;
	Assert(Concatinated.ToString() == (String("C:") + Path::Seperator + "FolderA" + Path::Seperator + "FolderB" + Path::Seperator + "File.Ext" + Path::Seperator + ".Compiler" + Path::Seperator + "Bleh.ext2.compiled"));

	String StringDirectory = TestAbs.GetDirectory().ToString();
	String StringDirectoryCompare = (String("C:") + Path::Seperator + "FolderA" + Path::Seperator + "FolderB");

	Assert(TestAbs.GetMount() == "C");
	Assert(TestAbs.GetBaseName() == "File");
	Assert(TestAbs.GetExtension() == "Ext");
	Assert(TestAbs.GetFilename() == "File.Ext");
	Assert(StringDirectory == StringDirectoryCompare);
	Assert(TestAbs.IsAbsolute());
	Assert(!TestAbs.IsRelative());

	Assert(TestRel.GetBaseName() == "Bleh.ext2");
	Assert(TestRel.GetExtension() == "compiled");
	Assert(TestRel.GetFilename() == "Bleh.ext2.compiled");
	Assert(TestRel.GetDirectory().ToString() == (String(".Compiler")));
	Assert(!TestRel.IsAbsolute());
	Assert(TestRel.IsRelative());

#ifdef LD_PLATFORM_WINDOWS
	Array<Path> Files = Path("C:/Windows").GetFiles();
	Assert(Files.Length() > 0);

	Array<Path> Dirs = Path("C:/Windows").GetDirectories();
	Assert(Dirs.Length() > 0);

	Assert(Path("C:/Windows").Exists());
	Assert(Path("C:/Windows/System.ini").Exists());

	Assert(Path("C:/Windows/System.ini").IsFile());
	Assert(!Path("C:/Windows/System.ini").IsDirectory());
	Assert(!Path("C:/Windows").IsFile());
	Assert(Path("C:/Windows").IsDirectory());
	 
	Path("TestFolder/TestFolder2").CreateAsDirectory();
	Assert(Path("TestFolder/TestFolder2").IsDirectory());
	Path("TestFolder/TestFile").CreateAsFile();
	Assert(Path("TestFolder/TestFile").IsFile());

	Path("TestFolder/TestFile").Delete();
	Assert(!Path("TestFolder/TestFile").Exists());
	Path("TestFolder/TestFile").CreateAsFile();
	Assert(Path("TestFolder/TestFile").IsFile());

	Path("TestFolder").Copy(Path("TestFolder2"));
	Assert(Path("TestFolder2").Exists());
	Assert(Path("TestFolder2").IsDirectory());

	Path("TestFolder").Delete();
	Assert(!Path("TestFolder").Exists());

	Path("TestFolder2").Delete();
	Assert(!Path("TestFolder2").Exists());
#endif
}

}; // namespace Ludo
