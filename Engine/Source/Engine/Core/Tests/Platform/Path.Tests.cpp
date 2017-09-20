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
#pragma once

#include "PCH.h"

using namespace Ludo;

// ************************************************************************************************

TEST(PathTests, Constructors) 
{
	Path TestFile("C:/FolderA/.File");
	EXPECT_TRUE(TestFile.ToString() == (String("C:") + Path::Seperator + "FolderA" + Path::Seperator + ".File"));
}

// ************************************************************************************************

TEST(PathTests, ToString)
{
	Path TestAbs("C:\\FolderA/FolderB\\File.Ext");
	Path TestRel("//.Compiler/Bleh.ext2.compiled");

	String TestAbsToString = TestAbs.ToString();
	String TestRelToString = TestRel.ToString();

	EXPECT_TRUE(TestAbsToString == (String("C:") + Path::Seperator + "FolderA" + Path::Seperator + "FolderB" + Path::Seperator + "File.Ext"));
	EXPECT_TRUE(TestRelToString == (String(".Compiler") + Path::Seperator + "Bleh.ext2.compiled"));
}

// ************************************************************************************************

TEST(PathTests, ElementChange)
{
	Path TestAbs("C:\\FolderA/FolderB\\File.Ext");

	String ChangedExtension = TestAbs.ChangeExtension("Derp").ToString();
	String ChangedBasename = TestAbs.ChangeBaseName("Derp").ToString();
	String ChangedFilename = TestAbs.ChangeFilename("Derp.Herp").ToString();
	String ChangedDirectory = TestAbs.ChangeDirectory(Path("D:/Test/1")).ToString();
	String ChangedMount = TestAbs.ChangeMount("X").ToString();

	EXPECT_TRUE(ChangedExtension == (String("C:") + Path::Seperator + "FolderA" + Path::Seperator + "FolderB" + Path::Seperator + "File.Derp"));
	EXPECT_TRUE(ChangedBasename == (String("C:") + Path::Seperator + "FolderA" + Path::Seperator + "FolderB" + Path::Seperator + "Derp.Ext"));
	EXPECT_TRUE(ChangedFilename == (String("C:") + Path::Seperator + "FolderA" + Path::Seperator + "FolderB" + Path::Seperator + "Derp.Herp"));
	EXPECT_TRUE(ChangedDirectory == (String("D:") + Path::Seperator + "Test" + Path::Seperator + "1" + Path::Seperator + "File.Ext"));
	EXPECT_TRUE(ChangedMount == (String("X:") + Path::Seperator + "FolderA" + Path::Seperator + "FolderB" + Path::Seperator + "File.Ext"));
}

// ************************************************************************************************

TEST(PathTests, Concatination)
{
	Path TestAbs("C:\\FolderA/FolderB\\File.Ext");
	Path TestRel("//.Compiler/Bleh.ext2.compiled");

	Path Concatinated = TestAbs + TestRel;
	EXPECT_TRUE(Concatinated.ToString() == (String("C:") + Path::Seperator + "FolderA" + Path::Seperator + "FolderB" + Path::Seperator + "File.Ext" + Path::Seperator + ".Compiler" + Path::Seperator + "Bleh.ext2.compiled"));
}

// ************************************************************************************************

TEST(PathTests, Cracking)
{
	Path TestAbs("C:\\FolderA/FolderB\\File.Ext");
	Path TestRel("//.Compiler/Bleh.ext2.compiled");

	String StringDirectory = TestAbs.GetDirectory().ToString();
	String StringDirectoryCompare = (String("C:") + Path::Seperator + "FolderA" + Path::Seperator + "FolderB");

	EXPECT_TRUE(TestAbs.GetMount() == "C");
	EXPECT_TRUE(TestAbs.GetBaseName() == "File");
	EXPECT_TRUE(TestAbs.GetExtension() == "Ext");
	EXPECT_TRUE(TestAbs.GetFilename() == "File.Ext");
	EXPECT_TRUE(StringDirectory == StringDirectoryCompare);
	
	EXPECT_TRUE(TestRel.GetBaseName() == "Bleh.ext2");
	EXPECT_TRUE(TestRel.GetExtension() == "compiled");
	EXPECT_TRUE(TestRel.GetFilename() == "Bleh.ext2.compiled");
	EXPECT_TRUE(TestRel.GetDirectory().ToString() == (String(".Compiler")));
	EXPECT_TRUE(!TestRel.IsAbsolute());
	EXPECT_TRUE(TestRel.IsRelative());
}

// ************************************************************************************************

TEST(PathTests, State)
{
	Path TestAbs("C:\\FolderA/FolderB\\File.Ext");

	EXPECT_TRUE(TestAbs.IsAbsolute());
	EXPECT_TRUE(!TestAbs.IsRelative());
}

// ************************************************************************************************

TEST(PathTests, FileEnumeration)
{
#ifdef LD_PLATFORM_WINDOWS
	Array<Path> Files = Path("C:/Windows").GetFiles();
	EXPECT_TRUE(Files.Length() > 0);
#endif
}

// ************************************************************************************************

TEST(PathTests, DirectoryEnumeration)
{
#ifdef LD_PLATFORM_WINDOWS
	Array<Path> Dirs = Path("C:/Windows").GetDirectories();
	EXPECT_TRUE(Dirs.Length() > 0);
#endif
}

// ************************************************************************************************

TEST(PathTests, FileExistance)
{
#ifdef LD_PLATFORM_WINDOWS
	EXPECT_TRUE(Path("C:/Windows").Exists());
	EXPECT_TRUE(Path("C:/Windows/System.ini").Exists());
#endif
}

// ************************************************************************************************

TEST(PathTests, FileType)
{
#ifdef LD_PLATFORM_WINDOWS
	EXPECT_TRUE(Path("C:/Windows/System.ini").IsFile());
	EXPECT_TRUE(!Path("C:/Windows/System.ini").IsDirectory());
	EXPECT_TRUE(!Path("C:/Windows").IsFile());
	EXPECT_TRUE(Path("C:/Windows").IsDirectory());
#endif
}

// ************************************************************************************************

TEST(PathTests, DirectoryCreationAndDeletion)
{
	Path("TestFolder/TestFolder2").CreateAsDirectory();
	EXPECT_TRUE(Path("TestFolder/TestFolder2").IsDirectory());

	Path("TestFolder/TestFolder2").Delete();
	EXPECT_TRUE(!Path("TestFolder/TestFolder2").Exists());

	Path("TestFolder").Delete();
	EXPECT_TRUE(!Path("TestFolder").Exists());
}

// ************************************************************************************************

TEST(PathTests, FileCreationAndDeletion)
{
	Path("TestFile").CreateAsFile();
	EXPECT_TRUE(Path("TestFile").IsFile());

	Path("TestFile").Delete();
	EXPECT_TRUE(!Path("TestFile").Exists());
}

// ************************************************************************************************

TEST(PathTests, FolderCopying)
{
	Path("TestFolder").CreateAsDirectory();
	EXPECT_TRUE(Path("TestFolder").IsDirectory());

	Path("TestFolder/TestFile").CreateAsFile();
	EXPECT_TRUE(Path("TestFolder/TestFile").IsFile());

	Path("TestFolder").Copy(Path("TestFolder2"));
	EXPECT_TRUE(Path("TestFolder2").Exists());
	EXPECT_TRUE(Path("TestFolder2").IsDirectory());

	Path("TestFolder").Delete();
	EXPECT_TRUE(!Path("TestFolder").Exists());

	Path("TestFolder2").Delete();
	EXPECT_TRUE(!Path("TestFolder2").Exists());
}

// ************************************************************************************************