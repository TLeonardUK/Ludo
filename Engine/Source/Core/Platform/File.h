// Copyright 2016 TwinDrills
#pragma once

#include "Core/Platform/Path.h"

namespace Ludo {

/// \brief TODO
enum class FileFlags
{
	Read		= 1,
	Write		= 2,
	ReadWrite	= Read | Write,
};

/// \brief TODO
class File
{
private:
	class Impl;
	Impl* m_Impl;

public:

	/// \brief TODO
	File(const Path& path);

	/// \brief TODO
	~File();

	/// \brief TODO
	bool Open(FileFlags flags);

	/// \brief TODO
	void Close();

	/// \brief TODO
	bool IsOpen();

	/// \brief TODO
	bool Write(void* Buffer, int BufferLength);

	/// \brief TODO
	bool Read(void* Buffer, int BufferLength);

};

};