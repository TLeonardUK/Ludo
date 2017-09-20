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

#include "Core.Windows/Private/Platform/Helper/Errors.h"

using namespace Ludo;
using namespace Ludo::Windows;

namespace Ludo {

// ************************************************************************************************

class FileImpl
{
private:
	FILE* m_File;
	uint64 m_Length;
	bool m_CanRead;
	bool m_CanWrite;

public:
	FileImpl()
		: m_File(nullptr)
		, m_CanRead(false)
		, m_CanWrite(false)
	{
	}

	~FileImpl()
	{
		if (m_File != nullptr)
		{
			Close();
		}
	}

	Error Open(const Path& path, FileFlags flags) 
	{
		LD_ASSERT(!IsOpen());

		bool bExists = path.Exists();

		String Mode = "rb";
		if (flags == FileFlags::Read)
		{
			Mode = "rb";

			if (!bExists)
			{
				return ErrorType::Failure;
			}

			m_CanRead = true;
			m_CanWrite = false;
		}
		else if (flags == FileFlags::Write)
		{
			if (bExists)
			{
				Mode = "wb+";
			}
			else
			{
				Mode = "wb";
			}

			m_CanRead = false;
			m_CanWrite = true;
		}
		else if (flags == FileFlags::ReadWrite)
		{
			if (bExists)
			{
				Mode = "rb+";
			}
			else
			{
				Mode = "wb+";
			}

			m_CanRead = true;
			m_CanWrite = true;
		}

		m_File = fopen(path.ToString().Data(), Mode.Data());
		if (m_File == nullptr)
		{
			return ErrorType::Failure;
		}

		// Precalulate length.
		_fseeki64(m_File, 0, SEEK_END);
		m_Length = _ftelli64(m_File);
		fseek(m_File, 0, SEEK_SET);

		return ErrorType::Success;
	}

	void Close()
	{
		LD_ASSERT(IsOpen());

		fclose(m_File);
		m_File = nullptr;
	}

	bool IsOpen() 
	{
		return (m_File != nullptr);
	}

	Error Write(void* Buffer, uint64 BufferLength) 
	{
		LD_ASSERT(IsOpen());

		size_t Result = fwrite(Buffer, 1, (size_t)BufferLength, m_File);
		if (Result != BufferLength)
		{
			return ErrorType::Failure;
		}

		return ErrorType::Success;
	}

	Error Read(void* Buffer, uint64_t BufferLength) 
	{
		LD_ASSERT(IsOpen());

		size_t Result = fread(Buffer, 1, (size_t)BufferLength, m_File);
		if (Result != BufferLength)
		{
			if (feof(m_File))
			{
				return ErrorType::AtEnd;
			}
			else
			{
				return ErrorType::Failure;
			}
		}

		return ErrorType::Success;
	}

	uint64 Position() 
	{
		LD_ASSERT(IsOpen());

		return _ftelli64(m_File);
	}

	uint64 Length() 
	{
		return m_Length;
	}

	void Seek(uint64 Position)
	{
		LD_ASSERT(IsOpen());

		_fseeki64(m_File, Position, SEEK_SET);
	}
	
	bool AtEnd()
	{
		LD_ASSERT(IsOpen());

		return Position() >= Length();
	}

	void Flush() 
	{
		LD_ASSERT(IsOpen());

		fflush(m_File);
	}

	bool CanRead() 
	{
		return IsOpen() && m_CanRead;
	}

	bool CanWrite()
	{
		return IsOpen() && m_CanWrite;
	}
};

// ************************************************************************************************

File::File()
{
	CreateImpl<PlatformMemoryArena, FileImpl>();
}

// ************************************************************************************************

File::~File()
{
	DestroyImpl<PlatformMemoryArena, FileImpl>();
}

// ************************************************************************************************

Error File::Open(const Path& path, FileFlags flags)
{
	return GetImpl<FileImpl>()->Open(path, flags);
}

// ************************************************************************************************

void File::Close()
{
	GetImpl<FileImpl>()->Close();
}

// ************************************************************************************************

bool File::IsOpen()
{
	return GetImpl<FileImpl>()->IsOpen();
}

// ************************************************************************************************

Error File::Write(void* Buffer, uint64 BufferLength)
{
	return GetImpl<FileImpl>()->Write(Buffer, BufferLength);
}

// ************************************************************************************************

Error File::Read(void* Buffer, uint64 BufferLength)
{
	return GetImpl<FileImpl>()->Read(Buffer, BufferLength);
}

// ************************************************************************************************

uint64 File::Position()
{
	return GetImpl<FileImpl>()->Position();
}

// ************************************************************************************************

uint64 File::Length()
{
	return GetImpl<FileImpl>()->Length();
}

// ************************************************************************************************

void File::Seek(uint64 Position)
{
	GetImpl<FileImpl>()->Seek(Position);
}

// ************************************************************************************************

bool File::AtEnd()
{
	return GetImpl<FileImpl>()->AtEnd();
}

// ************************************************************************************************

void File::Flush()
{
	GetImpl<FileImpl>()->Flush();
}

// ************************************************************************************************

bool File::CanRead()
{
	return GetImpl<FileImpl>()->CanRead();
}

// ************************************************************************************************

bool File::CanWrite()
{
	return GetImpl<FileImpl>()->CanWrite();
}

// ************************************************************************************************

}; // namespace Ludo