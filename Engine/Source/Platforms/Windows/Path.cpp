// Copyright 2016 TwinDrills

#include "Core/Platform/Path.h"

#include "Platforms/Windows/Helper/Errors.h"

using namespace Ludo;
using namespace Ludo::Windows;

namespace Ludo {

char Path::Seperator = '\\';

Array<Path> Path::GetFiles() const
{
	Array<Path> Result;

	WIN32_FIND_DATAA Data;
	HANDLE Handle;

	String Pattern = m_CachedString + Seperator + "*";

	Handle = FindFirstFileA(Pattern.Data(), &Data);
	if (Handle != INVALID_HANDLE_VALUE)
	{
		do
		{
			String Filename = Data.cFileName;
			Path FullPath = m_CachedString + Seperator + Filename;

			if (Filename != "." && Filename != "..")
			{
				if (FullPath.IsFile())
				{
					Result.Add(FullPath);
				}
			}
		}
		while (FindNextFileA(Handle, &Data) != 0);

		FindClose(Handle);
	}

	return Result;
}

Array<Path> Path::GetDirectories() const
{
	Array<Path> Result;

	WIN32_FIND_DATAA Data;
	HANDLE Handle;

	String Pattern = m_CachedString + Seperator + "*";

	Handle = FindFirstFileA(Pattern.Data(), &Data);
	if (Handle != INVALID_HANDLE_VALUE)
	{
		do
		{
			String Filename = Data.cFileName;
			Path FullPath = m_CachedString + Seperator + Filename;

			if (Filename != "." && Filename != "..")
			{
				if (FullPath.IsDirectory())
				{
					Result.Add(FullPath);
				}
			}
		}
		while (FindNextFileA(Handle, &Data) != 0);

		FindClose(Handle);
	}

	return Result;
}

bool Path::IsFile() const
{
	DWORD flags = GetFileAttributesA(m_CachedString.Data());

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
	DWORD flags = GetFileAttributesA(m_CachedString.Data());

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

	BOOL Ret = CreateDirectoryA(m_CachedString.Data(), NULL);
	if (Ret == 0)
	{
		LogLastSystemError("CreateDirectory");
	}

	return (Ret != 0);
}

bool Path::CreateAsFile() const
{
	Path ParentDir = GetDirectory();

	if (!ParentDir.IsDirectory())
	{
		ParentDir.CreateAsDirectory();
	}

	HANDLE Ret = CreateFileA(
		m_CachedString.Data(), 
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
	DWORD flags = GetFileAttributesA(m_CachedString.Data());
	return !(flags == INVALID_FILE_ATTRIBUTES);
}

bool Path::Copy(const Path& Destination) const
{
	if (IsFile())
	{
		BOOL Ret = CopyFileA(m_CachedString.Data(), Destination.m_CachedString.Data() , false);
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
		for (Path& path : SubDirs)
		{
			if (!path.Copy(path.ChangeDirectory(Destination)))
			{
				return false;
			}
		}

		// Copy all sub files.
		for (Path& path : SubFiles)
		{
			if (!path.Copy( path.ChangeDirectory(Destination) ))
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
		BOOL Ret = DeleteFileA(m_CachedString.Data());
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
			path.Delete();
		}

		// Delete all sub files.
		for (Path& path : SubFiles)
		{
			path.Delete();
		}

		// Delete the actual folder.
		BOOL Ret = RemoveDirectoryA(m_CachedString.Data());
		if (Ret == 0)
		{
			LogLastSystemError("RemoveDirectory");
		}

		return (Ret != 0);
	}

	return false;
}

}; // namespace Ludo