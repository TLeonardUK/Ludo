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

#include "Core-Windows/Private/Platform/Helper/TimeConversion.h"
#include "Core-Windows/Private/Platform/Helper/Errors.h"

using namespace Ludo;
using namespace Ludo::Windows;

namespace Ludo {

char Path::Seperator = '\\';

Array<Path> Path::GetFiles() const
{
	Array<Path> Result;

	WIN32_FIND_DATAA Data;
	HANDLE Handle;

	String Pattern = m_raw + Seperator + "*";

	Handle = FindFirstFileA(Pattern.Data(), &Data);
	if (Handle != INVALID_HANDLE_VALUE)
	{
		do
		{
			if ((Data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
			{
				if (strcmp(Data.cFileName, ".") != 0 &&
					strcmp(Data.cFileName, "..") != 0)
				{
					Result.Push(Data.cFileName);
				}
			}
		} while (FindNextFileA(Handle, &Data) != 0);

		FindClose(Handle);
	}

	return Result;
}

Array<Path> Path::GetDirectories() const
{
	Array<Path> Result;

	WIN32_FIND_DATAA Data;
	HANDLE Handle;

	String Pattern = m_raw + Seperator + "*";

	Handle = FindFirstFileA(Pattern.Data(), &Data);
	if (Handle != INVALID_HANDLE_VALUE)
	{
		do
		{
			if ((Data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
			{
				if (strcmp(Data.cFileName, ".") != 0 &&
					strcmp(Data.cFileName, "..") != 0)
				{
					Result.Push(Data.cFileName);
				}
			}
		} while (FindNextFileA(Handle, &Data) != 0);

		FindClose(Handle);
	}

	return Result;
}

bool Path::IsFile() const
{
	DWORD flags = GetFileAttributesA(m_raw.Data());

	if (flags == INVALID_FILE_ATTRIBUTES)
	{
		return false;
	}

	if ((flags & FILE_ATTRIBUTE_DIRECTORY) != 0)
	{
		return false;
	}

	return true;
}

bool Path::IsDirectory() const
{
	DWORD flags = GetFileAttributesA(m_raw.Data());

	if (flags == INVALID_FILE_ATTRIBUTES)
	{
		return false;
	}

	if ((flags & FILE_ATTRIBUTE_DIRECTORY) == 0)
	{
		return false;
	}

	return true;
}

bool Path::CreateAsDirectory() const
{
	Path ParentDir = GetDirectory();

	if (!ParentDir.IsEmpty() && !ParentDir.IsDirectory())
	{
		ParentDir.CreateAsDirectory();
	}

	BOOL Ret = CreateDirectoryA(m_raw.Data(), nullptr);
	if (Ret == 0)
	{
		LogLastSystemError("CreateDirectory");
	}

	return (Ret != 0);
}

bool Path::CreateAsFile() const
{
	Path ParentDir = GetDirectory();

	if (!ParentDir.IsEmpty() && !ParentDir.IsDirectory())
	{
		ParentDir.CreateAsDirectory();
	}

	HANDLE Ret = CreateFileA(
		m_raw.Data(),
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (Ret == INVALID_HANDLE_VALUE)
	{
		LogLastSystemError("CreateFile");
		return false;
	}
	else
	{
		CloseHandle(Ret);
		return true;
	}
}

bool Path::Exists() const
{
	DWORD flags = GetFileAttributesA(m_raw.Data());
	return !(flags == INVALID_FILE_ATTRIBUTES);
}

bool Path::Copy(const Path& Destination) const
{
	if (IsFile())
	{
		Path DestDirectory = Destination.GetDirectory();

		// If directory dosen't exist, get creating.
		if (!DestDirectory.Exists())
		{
			if (!DestDirectory.CreateAsDirectory())
			{
				return false;
			}
		}

		BOOL Ret = CopyFileA(m_raw.Data(),
			Destination.m_raw.Data(), false);

		if (Ret == 0)
		{
			LogLastSystemError("CopyFile");
			return false;
		}
	}
	else
	{
		Array<Path> SubDirs = GetDirectories();
		Array<Path> SubFiles = GetFiles();

		// If directory dosen't exist, get creating.
		if (!Destination.Exists())
		{
			if (!Destination.CreateAsDirectory())
			{
				return false;
			}
		}

		// Copy all sub-directories.
		for (Path& filename : SubDirs)
		{
			Path path = (*this) + filename;
			if (!path.Copy(path.ChangeDirectory(Destination)))
			{
				return false;
			}
		}

		// Copy all sub files.
		for (Path& filename : SubFiles)
		{
			Path path = (*this) + filename;
			if (!path.Copy(path.ChangeDirectory(Destination)))
			{
				return false;
			}
		}
	}

	return true;
}

bool Path::Delete() const
{
	if (IsFile())
	{
		BOOL Ret = DeleteFileA(m_raw.Data());
		if (Ret == 0)
		{
			LogLastSystemError("DeleteFile");
		}

		return (Ret != 0);
	}
	else if (IsDirectory())
	{
		Array<Path> SubDirs = GetDirectories();
		Array<Path> SubFiles = GetFiles();

		// Delete all sub directories.
		for (Path& path : SubDirs)
		{
			Path FullPath = *this + path;
			FullPath.Delete();
		}

		// Delete all sub files.
		for (Path& path : SubFiles)
		{
			Path FullPath = *this + path;
			FullPath.Delete();
		}

		// Delete the actual folder.
		BOOL Ret = RemoveDirectoryA(m_raw.Data());
		if (Ret == 0)
		{
			LogLastSystemError("RemoveDirectory");
		}

		return (Ret != 0);
	}

	return false;
}

Time Path::GetModifiedTime() const
{
	WIN32_FILE_ATTRIBUTE_DATA Attributes;
	BOOL Result = GetFileAttributesExA(m_raw.Data(), GetFileExInfoStandard, &Attributes);
	if (!Result)
	{
		return Time::Epoch();
	}
	else
	{
		return Time(Ludo::Windows::FileTimeToUnixTimestamp(Attributes.ftLastWriteTime));
	}
}

}; // namespace Ludo