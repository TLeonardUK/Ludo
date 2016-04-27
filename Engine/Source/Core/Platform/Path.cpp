// Copyright 2016 TwinDrills

#include "Core/Platform/Path.h"

namespace Ludo {

Path::Path()
	: m_Mount("")
	, m_Filename("")
	, m_Extension("")
{
}

Path::Path(const String& InValue)
	: m_Mount("")
	, m_Filename("")
	, m_Extension("")
{
	Array<String> Seperators;
	Seperators.Add("/");
	Seperators.Add("\\");

	String Value = InValue;

	// Parse out the mount.
	int ColonIndex = Value.IndexOf(":");
	if (ColonIndex != String::NotFound)
	{
		Value.SplitOnIndex(ColonIndex, m_Mount, Value);
	}

	// Parse out the extension.
	int PeriodIndex = Value.LastIndexOf(".");
	if (PeriodIndex != String::NotFound)
	{
		int LastSeperatorIndex = Value.LastIndexOfAny(Seperators);
		if (LastSeperatorIndex == String::NotFound || LastSeperatorIndex < PeriodIndex)
		{
			Value.SplitOnIndex(PeriodIndex, Value, m_Extension);
		}
	}

	// Split up directories/filename.	
	while (!Value.IsEmpty())
	{
		int SliceIndex = Value.IndexOfAny(Seperators);
		if (SliceIndex != String::NotFound)
		{
			String Segment;
			Value.SplitOnIndex(SliceIndex, Segment, Value);
			if (!Segment.IsEmpty())
			{
				m_Directories.Add(Segment);
			}
		}
		else
		{
			if (!Value.IsEmpty())
			{
				m_Directories.Add(Value);
			}
			break;
		}
	}

	if (m_Directories.Length() >= 1)
	{
		m_Filename = m_Directories.Pop();
	}

	UpdateCachedString();
}

void Path::UpdateCachedString()
{
	String Result = "";

	if (!m_Mount.IsEmpty())
	{
		Result = m_Mount + ":";
		Result += Seperator;
	}

	if (!m_Directories.IsEmpty())
	{
		for (String Directory : m_Directories)
		{
			Result += Directory + Seperator;
		}
	}

	Result += m_Filename;

	if (!m_Extension.IsEmpty())
	{
		Result += "." + m_Extension;
	}

	m_CachedString = Result;
}

String Path::ToString() const
{
	return m_CachedString;
}

Path Path::operator +(const Path& Other)
{
	AssertMsg(Other.IsRelative(), "Attempted to append absolute path to absolute path.");

	Path Result;
	Result.m_Mount = m_Mount;
	Result.m_Directories = m_Directories;

	if (!m_Filename.IsEmpty())
	{
		if (!m_Extension.IsEmpty())
		{
			Result.m_Directories.Add(m_Filename + "." + m_Extension);
		}
		else
		{
			Result.m_Directories.Add(m_Filename);
		}
	}
	else
	{
		m_Filename = "";
	}

	Result.m_Directories.Append(Other.m_Directories);
	Result.m_Filename = Other.m_Filename;
	Result.m_Extension = Other.m_Extension;

	Result.UpdateCachedString();

	return Result;
}

String Path::GetBaseName() const
{
	return m_Filename;
}

String Path::GetExtension() const
{
	return m_Extension;
}

String Path::GetMount() const
{
	return m_Mount;
}

String Path::GetFilename() const
{
	if (!m_Extension.IsEmpty())
	{
		return m_Filename + "." + m_Extension;
	}
	else
	{
		return m_Filename;
	}
}

Path Path::GetDirectory() const
{
	Path Result;
	Result.m_Mount = m_Mount;
	Result.m_Directories = m_Directories;

	if (Result.m_Directories.Length() > 0)
	{
		Result.m_Filename = Result.m_Directories.Pop();
		Result.m_Extension = "";

		// Parse out the extension.
		int PeriodIndex = Result.m_Filename.LastIndexOf(".");
		if (PeriodIndex != String::NotFound)
		{
			Result.m_Filename.SplitOnIndex(PeriodIndex, Result.m_Filename, Result.m_Extension);
		}
	}
	else
	{
		Result.m_Filename = "";
		Result.m_Extension = "";
	}

	Result.UpdateCachedString();
	return Result;
}

Path Path::ChangeExtension(const String& Value) const
{
	Path Result = *this;
	Result.m_Extension = Value;
	Result.UpdateCachedString();
	return Result;
}

Path Path::ChangeBaseName(const String& Value) const
{
	Path Result = *this;
	Result.m_Filename = Value;
	Result.UpdateCachedString();
	return Result;
}

Path Path::ChangeMount(const String& Value) const
{
	Path Result = *this;
	Result.m_Mount = Value;
	Result.UpdateCachedString();
	return Result;
}

Path Path::ChangeFilename(const String& Value) const
{
	Path Result = *this;

	int PeriodIndex = Value.LastIndexOf(".");
	if (PeriodIndex != String::NotFound)
	{
		Value.SplitOnIndex(PeriodIndex, Result.m_Filename, Result.m_Extension);
	}
	else
	{
		Result.m_Filename = Value;
		Result.m_Extension = "";
	}

	Result.UpdateCachedString();
	return Result;
}

Path Path::ChangeDirectory(const Path& Value) const
{
	Path Result = *this;

	Result.m_Mount = Value.m_Mount;
	Result.m_Directories = Value.m_Directories;

	if (!Value.m_Filename.IsEmpty())
	{
		if (!Value.m_Extension.IsEmpty())
		{
			Result.m_Directories.Add(Value.m_Filename + "." + Value.m_Extension);
		}
		else
		{
			Result.m_Directories.Add(Value.m_Filename);
		}
	}

	Result.UpdateCachedString();
	return Result;
}

bool Path::IsEmpty() const
{
	return m_Mount.IsEmpty() && m_Filename.IsEmpty() && m_Extension.IsEmpty() && m_Directories.IsEmpty();
}

bool Path::IsRelative() const
{
	return m_Mount.IsEmpty();
}

bool Path::IsAbsolute() const
{
	return !IsRelative();
}

}; // namespace Ludo