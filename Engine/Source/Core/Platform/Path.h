// Copyright 2016 TwinDrills
#pragma once

#include "Core/Types/String.h"
#include "Core/Containers/Array.h"

namespace Ludo {

/// \brief TODO
class Path
{
private:
	String m_Mount;
	Array<String> m_Directories;
	String m_Filename;
	String m_Extension;

	String m_CachedString;

protected:
	void UpdateCachedString();

public:
	/// \brief TODO
	static char Seperator;

	/// \brief TODO
	Path();

	/// \brief TODO
	Path(const String& Value);

	/// \brief TODO
	String ToString() const;

	/// \brief TODO
	Path operator +(const Path& Other);

	/// \brief TODO
	String GetBaseName() const;

	/// \brief TODO
	String GetExtension() const;

	/// \brief TODO
	String GetFilename() const;

	/// \brief TODO
	String GetMount() const;

	/// \brief TODO
	Path GetDirectory() const;

	/// \brief TODO
	Path ChangeExtension(const String& Value) const;

	/// \brief TODO
	Path ChangeBaseName(const String& Value) const;

	/// \brief TODO
	Path ChangeFilename(const String& Value) const;

	/// \brief TODO
	Path ChangeMount(const String& Value) const;

	/// \brief TODO
	Path ChangeDirectory(const Path& Value) const;

	/// \brief TODO
	Array<Path> GetFiles() const;

	/// \brief TODO
	Array<Path> GetDirectories() const;

	/// \brief TODO
	bool IsEmpty() const;

	/// \brief TODO
	bool IsFile() const;

	/// \brief TODO
	bool IsDirectory() const;

	/// \brief TODO
	bool IsRelative() const;

	/// \brief TODO
	bool IsAbsolute() const;

	/// \brief TODO
	bool CreateAsDirectory() const;

	/// \brief TODO
	bool CreateAsFile() const;

	/// \brief TODO
	bool Exists() const;

	/// \brief TODO
	bool Copy(const Path& Destination) const;

	/// \brief TODO
	bool Delete() const;

};

};