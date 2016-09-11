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

#include "Core/Public/IO/Path.h"
#include "Core/Public/Environment/Environment.h"
#include "Core/Public/Types/String.h"
#include "Core/Public/Time/Time.h"

namespace Ludo {

Path::Path()
	: m_raw("")
{
}
Path::Path(const char* InValue)
	: m_raw(InValue)
{
	Normalize();
}

Path::Path(const String& InValue)
	: m_raw(InValue)
{
	Normalize();
}

String Path::ToString() const
{
	return m_raw;
}

Path Path::operator +(const Path& Other) const
{
	return String::Format("%s%c%s", m_raw.Data(), Seperator, Other.m_raw.Data());
}

Path Path::operator +(const String& Other) const
{
	return String::Format("%s%c%s", m_raw.Data(), Seperator, Other.Data());
}

bool Path::operator ==(const Path& Other) const
{
	return (m_raw == Other.m_raw);
}

bool Path::operator !=(const Path& Other) const
{
	return !(operator ==(Other));
}

String Path::GetBaseName() const
{
	String result = m_raw;

	auto lastDirOffset = result.LastIndexOf(Seperator);
	if (lastDirOffset)
	{
		result = result.Remove(result.Begin(), lastDirOffset + 1);
	}

	auto lastDotOffset = result.LastIndexOf('.');
	if (lastDotOffset)
	{
		result.Remove(lastDotOffset, result.End());
	}

	return result;
}

String Path::GetExtension() const
{
	auto lastDotOffset = m_raw.LastIndexOf('.');
	if (lastDotOffset)
	{
		return m_raw.SubString(lastDotOffset + 1);
	}

	return "";
}

String Path::GetMount() const
{
	if (!m_raw.IsEmpty())
	{
		if (m_raw[0] == Seperator)
		{
			return{ Seperator };
		}
		else if (m_raw.CharLength() > 1 && m_raw[1] == ':')
		{
			return m_raw.SubString(m_raw.Begin(), 2);
		}
	}

	return "";
}

String Path::GetFilename() const
{
	auto lastDirOffset = m_raw.LastIndexOf(Seperator);
	if (lastDirOffset)
	{
		return m_raw.SubString(lastDirOffset + 1);
	}

	return m_raw;
}

Path Path::GetDirectory() const
{
	auto lastDirOffset = m_raw.LastIndexOf(Seperator);
	if (lastDirOffset)
	{
		return m_raw.SubString(m_raw.Begin(), lastDirOffset);
	}

	return "";
}

Path Path::ChangeExtension(const String& Value) const
{
	auto lastDotOffset = m_raw.LastIndexOf('.');
	if (lastDotOffset)
	{
		return String::Format(
			"%s.%s",
			m_raw.SubString(m_raw.Begin(), lastDotOffset).Data(),
			Value.Data()
		);
	}
	else
	{
		return String::Format(
			"%s.%s",
			m_raw.Data(),
			Value.Data()
		);
	}
}

Path Path::ChangeBaseName(const String& Value) const
{
	String result = m_raw;

	auto leftOffset = result.Begin();
	auto rightOffset = result.Begin();

	auto lastDirOffset = result.LastIndexOf(Seperator);
	if (lastDirOffset)
	{
		leftOffset = lastDirOffset + 1;
	}
	else
	{
		leftOffset = result.Begin();
	}

	auto lastDotOffset = result.LastIndexOf('.');
	if (lastDotOffset)
	{
		rightOffset = lastDotOffset;
	}
	else
	{
		rightOffset = result.End();
	}

	return result.Replace(
		leftOffset,
		rightOffset,
		Value
	);
}

Path Path::ChangeMount(const String& Value) const
{
	if (!m_raw.IsEmpty())
	{
		if (m_raw[0] == Seperator)
		{
			return String::Format("%s%s",
				m_raw.SubString(m_raw.Begin() + 1).Data(),
				Value.Data()
			);
		}
		else if (m_raw.CharLength() > 1 && m_raw[1] == ':')
		{
			return String::Format("%s%s",
				m_raw.SubString(m_raw.Begin() + 2).Data(),
				Value.Data()
			);
		}
	}

	return *this;
}

Path Path::ChangeFilename(const String& Value) const
{
	auto lastDirOffset = m_raw.LastIndexOf(Seperator);
	if (lastDirOffset)
	{
		return m_raw.SubString(m_raw.Begin(), lastDirOffset + 1) + Value;
	}
	else
	{
		return Value;
	}
}

Path Path::ChangeDirectory(const Path& Value) const
{
	auto lastDirOffset = m_raw.LastIndexOf(Seperator);
	if (lastDirOffset)
	{
		return String::Format("%s%c%s",
			Value.m_raw.Data(),
			Seperator,
			m_raw.SubString(lastDirOffset).Data()
		);
	}
	else
	{
		return String::Format("%s%c%s",
			Value.m_raw.Data(),
			Seperator,
			m_raw.Data()
		);
	}
}

Path Path::AppendFragment(const String& Value, bool bAddDeliminator) const
{
	if (bAddDeliminator)
	{
		return String::Format("%s%c%s", m_raw.Data(), Seperator, Value.Data());

	}
	else
	{
		return String::Format("%s%s", m_raw.Data(), Value.Data());
	}
}

bool Path::IsEmpty() const
{
	return m_raw.IsEmpty();
}

bool Path::IsRelative() const
{
	return GetMount() == "";
}

bool Path::IsAbsolute() const
{
	return !IsRelative();
}

bool Path::IsRoot() const
{
	if (m_raw.CharLength() == 1 && m_raw[0] == Seperator)
	{
		return true;
	}
	else if (m_raw.CharLength() == 2 && m_raw[1] == ':')
	{
		return true;
	}
	return false;
}

Array<String> Path::GetFragments() const
{
	return m_raw.Split(Seperator, 0, false);
}

void Path::Normalize()
{
	String original = m_raw;

	if (m_raw.CharLength() == 0)
	{
		return;
	}

	// Replace all environment variables.
	for (;;)
	{
		auto startOffset = m_raw.IndexOf('%');
		if (startOffset )
		{
			auto endOffset = m_raw.IndexOf('%', startOffset + 1);
			if (endOffset)
			{
				String tag = m_raw.SubString(startOffset + 1, endOffset);
				String value = Environment::GetEnvironmentVariable(tag);

				m_raw = m_raw.Replace(startOffset, endOffset + 1, value);
			}
			else
			{
				break;
			}

		}
		else
		{
			break;
		}
	}

	char correctSeperator = Seperator;
	char incorrectSeperator = '/';

	if (correctSeperator == incorrectSeperator)
	{
		incorrectSeperator = '\\';
	}

	// Replace incorrect directory seperators.
	for (;;)
	{
		auto incorrectOffset = m_raw.IndexOf(incorrectSeperator);
		if (incorrectOffset)
		{
			m_raw = m_raw.Replace(incorrectOffset, 1, correctSeperator);
		}
		else
		{
			break;
		}
	}

	// Replace trailing slash.
	if (m_raw[m_raw.CharLength() - 1] == correctSeperator)
	{
		m_raw = m_raw.SubString(m_raw.Begin(), m_raw.End() - 1);
	}

	// Replace repeated directory seperators.
	String repeatedSeperator = { correctSeperator, correctSeperator };
	for (;;)
	{
		auto offset = m_raw.IndexOf(repeatedSeperator);
		if (offset)
		{
			m_raw = m_raw.Remove(offset, offset + 1);
		}
		else
		{
			break;
		}
	}

	// Replace all parent seperators.
	char seperatorSequence[3] = { Seperator, '.', '\0' };

	if (IsAbsolute() && m_raw.IndexOf(seperatorSequence))
	{
		String result = "";
		String fragment = "";
		int skipCount = 0;

		for (int i = (int)m_raw.CharLength() - 1; i >= 0; i--)
		{
			String chr = m_raw[i];
			if (chr == correctSeperator)
			{
				if (fragment == "..")
				{
					skipCount++;
				}
				else if (fragment == ".")
				{
					// Don't skip anything, but don't append fragment.
				}
				else
				{
					if (skipCount > 0)
					{
						skipCount--;
					}
					else
					{
						if (result.IsEmpty())
						{
							result = fragment;
						}
						else
						{
							result = fragment + correctSeperator + result;
						}
					}
				}
				fragment = "";

				// If we are the first character, make sure to append the seperator so linux paths work.
				if (i == 0)
				{
					result = chr + result;
				}
			}
			else
			{
				fragment = chr + fragment;
			}
		}

		if (!fragment.IsEmpty())
		{
			if (skipCount > 0)
			{
				LogF(LogCore, 
					LogWarning,
					"Path '%s' attempted to reference directory above root!",
					m_raw.Data());
			}
			result = fragment + correctSeperator + result;
		}

		m_raw = result;
	}

	//printf("original=%s raw=%s\n", original.c_str(), m_raw.c_str());
}

Path Path::RelativeTo(const Path& Destination) const
{
	// If its a different mount, we have to be absolute.
	if (GetMount() != Destination.GetMount())
	{
		return Destination;
	}

	// If either of us is relative we have no root to determine relative
	// paths from, so its going to have to be absolute.
	if (IsRelative() || Destination.IsRelative())
	{
		return Destination;
	}

	Array<String> fragments = GetFragments();
	Array<String> destFragments = Destination.GetFragments();

	int fragmentsDirCount = fragments.Length();
	int destFragmentsDirCount = destFragments.Length();

	// Work out which directories are matching.
	int matchingDirs = 0;
	int minDirCount = Math::Min(fragmentsDirCount, destFragmentsDirCount);

	for (int i = 0; i < minDirCount; i++)
	{
		if (fragments[i] == destFragments[i])
		{
			matchingDirs++;
		}
		else
		{
			break;
		}
	}

	String result = "";

	int DirsToGoUp = fragmentsDirCount - matchingDirs;
	for (int i = 0; i < DirsToGoUp; i++)
	{
		if (!result.IsEmpty())
		{
			result += Seperator;
		}
		result += "..";
	}

	int dirsToAdd = destFragmentsDirCount - matchingDirs;
	for (int i = 0; i < dirsToAdd; i++)
	{
		if (!result.IsEmpty())
		{
			result += Seperator;
		}
		result += destFragments[matchingDirs + i];
	}

	return result;
}

bool Path::GetCommonPath(Array<Path>& paths, Path& result)
{
	if (paths.Length() == 0)
	{
		result = "";
		return false;
	}

	int maxOffset = INT_MAX;
	for (Path& path : paths)
	{
		maxOffset = Math::Min(maxOffset, path.ToString().CharLength());
	}

	String matchPath = "";
	int matchOffset = 0;

	for (;;)
	{
		// Find the next seperator.
		String pathValue = paths[0].ToString();
		auto offset = pathValue.IndexOf(Seperator, pathValue.Begin() + matchOffset);
		if (!offset)
		{
			offset = pathValue.End();
		}

		// The segment to match against the other paths is everything we haven't
		// matched so far.
		String main = pathValue.SubString(pathValue.Begin(), offset);

		bool bMatching = true;

		// Check each path matches the current part we are checking.
		for (int i = 1; i < paths.Length(); i++)
		{
			String subPathValue = paths[i].ToString();

			String sub = subPathValue.SubString(subPathValue.begin(), offset.CharIndex());
			auto subOffset = subPathValue.IndexOf(Seperator, subPathValue.Begin() + matchOffset);
			if (!subOffset)
			{
				subOffset = subPathValue.End();
			}

			if (offset != subOffset || sub != main)
			{
				bMatching = false;
				break;
			}
		}

		if (bMatching)
		{
			matchPath = main;
			matchOffset = offset + 1;

			if (offset.CharIndex() == maxOffset)
			{
				break;
			}
		}
		else
		{
			break;
		}
	}

	result = matchPath;
	return true;
}

Path Path::GetUncommonPath(Path& commonPath)
{
	String pathString = commonPath.ToString();
	Path uncommon = m_raw.SubString(pathString.End());
	return uncommon;
}

struct PathMatchFragment
{
	String full;
	String remaining;
	bool valid;
	bool isDirectory;
};

void MatchFilter_GetDirectories_r(const Path& base, Array<Path>& results)
{
	static String seperatorString(Path::Seperator, 1);

	Array<Path> dirs = base.GetDirectories();

	results.Reserve(results.Length() + dirs.Length());

	for (auto path : dirs)
	{
		Path fullPath = base + path;
		results.Push(fullPath);
		MatchFilter_GetDirectories_r(fullPath, results);
	}
}

Array<Path> MatchFilter_r(
	const Path& base,
	const Array<String>& matches)
{
	Array<Path> result;

	String seperatorString(Path::Seperator, 1);

	String matchType = matches[0];

	// Nothing to match again? Game over.
	if (matches.IsEmpty())
	{
		if (!base.IsDirectory())
		{
			result.Push(base);
		}
		return result;
	}

	// Try and match up as much as possible.
	else
	{
		Array<PathMatchFragment> potentialMatches;

		Array<Path> dirs = base.GetDirectories();
		Array<Path> files = base.GetFiles();

		potentialMatches.Reserve(dirs.Length() + files.Length());

		for (auto path : dirs)
		{
			PathMatchFragment frag;
			frag.full = path.ToString();
			frag.remaining = path.ToString();
			frag.valid = true;
			frag.isDirectory = true;

			potentialMatches.Push(frag);
		}

		for (auto path : files)
		{
			PathMatchFragment frag;
			frag.full = path.ToString();
			frag.remaining = path.ToString();
			frag.valid = true;
			frag.isDirectory = false;

			potentialMatches.Push(frag);
		}

		bool bFinished = false;

		Array<String> matchStack(matches);
		while (matchStack.Length() > 0 && !bFinished)
		{
			String match = matchStack[0];
			matchStack.RemoveAt(0);

			// Non-recursive match.
			if (match == "*")
			{
				// If we are at the end, or next is a directory, we can 
				// just accept all potentials.
				if (matchStack.Length() == 0 ||
					matchStack[0] == seperatorString)
				{
					for (int i = 0; i < potentialMatches.Length(); i++)
					{
						PathMatchFragment& frag = potentialMatches[i];
						frag.remaining = "";
					}
				}
				else
				{
					String nextFragment = matchStack[0];

					// Try and match up any potentials.
					for (int i = 0; i < potentialMatches.Length(); i++)
					{
						PathMatchFragment& frag = potentialMatches[i];
						if (frag.valid)
						{
							auto offset = frag.remaining.IndexOf(
								nextFragment
							);

							if (offset)
							{
								frag.remaining.Remove(
									frag.remaining.Begin(),
									offset);
							}
							else
							{
								potentialMatches[i].valid = false;
							}
						}
					}
				}
			}

			// Recursive match.
			else if (match == "**")
			{
				// Get a list of all directories below this sub-directory, 
				// try and perform a match for the reset of the stack on each
				// of these directories.
				Array<Path> subDirs;

				MatchFilter_GetDirectories_r(base, subDirs);

				// If we have a seperator next, we don't want
				// to pass in the base path as matches have to be at least
				// one directory level down.
				if (matchStack.Length() > 0 &&
					matchStack[0] == seperatorString)
				{
					matchStack.RemoveAt(0);
				}
				else
				{
					subDirs.Push(base);
				}

				// Add a non-recursive match to check against for each directory.
				matchStack.Insert("*", 0);

				for (int i = 0; i < subDirs.Length(); i++)
				{
					Array<Path> subResult =
						MatchFilter_r(subDirs[i], matchStack);

					result.Reserve(subResult.Length() + result.Length());

					for (Path& path : subResult)
					{
						result.Push(path);
					}
				}

				// No more matching at this level.
				bFinished = true;
			}

			// Expected directory.
			else if (match == seperatorString)
			{
				// Recurse into each directory.
				for (int i = 0; i < potentialMatches.Length(); i++)
				{
					PathMatchFragment& frag = potentialMatches[i];
					if (frag.remaining.IsEmpty() && frag.valid)
					{
						if (frag.isDirectory)
						{
							Array<Path> subResult =
								MatchFilter_r(base + seperatorString + frag.full, matchStack);

							for (Path& subPath : subResult)
							{
								result.Push(subPath);
							}
						}
					}
				}

				// No more matching at this level.
				bFinished = true;
			}

			// Literal match.
			else
			{
				for (int i = 0; i < potentialMatches.Length(); i++)
				{
					PathMatchFragment& frag = potentialMatches[i];

					if (frag.remaining.CharLength() >= match.CharLength() &&
						frag.remaining.SubString(frag.remaining.Begin(), frag.remaining.Begin() + match.CharLength()) == match)
					{
						frag.remaining = frag.remaining.Remove(
							frag.remaining.Begin(),
							frag.remaining.Begin() + match.CharLength());
					}
					else
					{
						potentialMatches[i].valid = false;
					}
				}
			}
		}

		// If we are not finished, but we are out of matches, see if any 
		// potential matches are now empty.
		if (!bFinished)
		{
			result.Reserve(potentialMatches.Length());

			for (int i = 0; i < potentialMatches.Length(); i++)
			{
				PathMatchFragment& frag = potentialMatches[i];
				if (frag.remaining.IsEmpty() && frag.valid && !frag.isDirectory)
				{
					result.Push(base.AppendFragment(frag.full, true));
				}
			}
		}
	}

	return result;
}

bool SplitIntoMatchStack(
	String pathString,
	Array<String>& matchStack,
	bool bMergeSeperators = false)
{
	int offset = 0;

	// Split up into * ** and path fragments
	while (offset < pathString.CharLength())
	{
		auto seperatorIndex = pathString.IndexOf(Path::Seperator, pathString.Begin() + offset);
		auto matchIndex = pathString.IndexOf('*', pathString.Begin() + offset);

		auto splitIndex = matchIndex;
		if (!bMergeSeperators)
		{
			if (!splitIndex || seperatorIndex < matchIndex)
			{
				splitIndex = seperatorIndex;
			}
		}

		if (!splitIndex)
		{
			matchStack.Add(pathString.SubString(pathString.Begin() + offset));
			break;
		}
		else
		{
			String split = pathString.SubString(splitIndex, 1);

			String fragment = pathString.SubString(pathString.Begin() + offset, splitIndex - offset);
			if (!fragment.IsEmpty())
			{
				matchStack.Push(fragment);
			}

			offset = splitIndex + 1;

			if (split == "*")
			{
				if (matchStack.Length() > 0)
				{
					String lastValue = matchStack[matchStack.Length() - 1];
					if (lastValue == "*" || lastValue == "**")
					{
						LogF(LogCore,
							LogError,
							"Wildcard followed by another wildcard in value '%s', "
							"this is ambiguous and cannot be expanded.",
							pathString.Data());

						return false;
					}
				}

				if (offset < pathString.CharLength())
				{
					if (pathString[offset] == '*')
					{
						split += "*";
						offset++;
					}
				}
			}

			if (!split.IsEmpty())
			{
				matchStack.Push(split);
			}
		}
	}

	return true;
}

void TrimMatchStackDownToFirstWildcard(
	Array<String>& matchStack,
	String& subValue)
{
	// Start at the first directory before a match value.
	int firstValidStartIndex = 0;
	String seperatorString(1, Path::Seperator);

	for (int i = 0; i < matchStack.Length(); i++)
	{
		String sub = matchStack[i];
		if (sub == seperatorString)
		{
			firstValidStartIndex = i;
		}
		else if (sub == "*" || sub == "**")
		{
			break;
		}
	}

	for (int i = 0; i <= firstValidStartIndex; i++)
	{
		subValue += matchStack[i];
	}

	matchStack.RemoveRange(0, firstValidStartIndex + 1);
}

Array<Path> Path::MatchFilter(const Path& path)
{
	Array<Path> result;

	// No match filters, early-out.
	if (!path.ToString().IndexOf('*'))
	{
		result.Push(path);
		return result;
	}

	// Split into wildcards and fragments.
	Array<String> matchStack;
	if (!SplitIntoMatchStack(path.ToString(), matchStack))
	{
		result.Push(path);
		return result;
	}

	// If we only have one split, we are done.
	if (matchStack.Length() == 1)
	{
		result.Push(matchStack[0]);
	}
	else
	{
		String subValue = "";
		TrimMatchStackDownToFirstWildcard(matchStack, subValue);
		result = MatchFilter_r(subValue, matchStack);
	}

	return result;
}

Path Path::GetWorkingDirectory()
{
	char buffer[2048];
	char* result = getcwd(buffer, 2048);
	Assert(result != nullptr);
	return result;
}

void Path::SetWorkingDirectory(const Path& other)
{
	int result = chdir(other.m_raw.Data());
	Assert(result == 0);
}

bool MatchEatNeedle(const char*& remaining, const char*& match)
{
	while (true)
	{
		// No characters remaining in needle
		// or remaining buffer? Done.
		if (remaining[0] == '\0' &&
			match[0] == '\0')
		{
			return true;
		}

		// Ran out of match characters, Done.
		if (match[0] == '\0')
		{
			return true;
		}

		// Ran out of characters. No match.
		if (remaining[0] == '\0')
		{
			return false;
		}

		// No recursive matching please.
		if (remaining[0] == Path::Seperator)
		{
			return false;
		}

		// Check needle.
		char needleChr = *remaining;
		char matchChr = *match;

		if (needleChr == matchChr)
		{
			remaining++;
			match++;
		}
		else
		{
			return false;
		}
	}

	return false;
}

bool DoesMatchStartWith(const char* data, String& needle)
{
	for (int i = 0; i < needle.CharLength(); i++)
	{
		if (data[i] == '\0')
		{
			return false;
		}
		if (String(data[i]) != needle[i])
		{
			return false;
		}
	}
	return true;
}

bool Matches_r(
	Array<String>& matchStack,
	int matchStackIndex,
	const char* remaining,
	String& matched,
	String& matchedSegment)
{
	static String seperatorString(1, Path::Seperator);

	// Out of both stacks? We're done!
	if (matchStackIndex >= matchStack.Length() && remaining[0] == '\0')
	{
		return true;
	}

	// Out of matches, but still have path? No match.
	// Out of path, but still have matches? No match.
	else if (matchStackIndex >= matchStack.Length() || remaining[0] == '\0')
	{
		return false;
	}

	String match = matchStack[matchStackIndex++];

	// Non-recursive/recursive match.
	if (match == "*" || match == "**")
	{
		bool bIsRecursive = (match == "**");

		// Search forward until next match.
		if (matchStackIndex < matchStack.Length())
		{
			String& needle = matchStack[matchStackIndex++];

			while (true)
			{
				// Ran out of characters. No match.
				if (remaining[0] == '\0')
				{
					return false;
				}

				// Check if next part of data matches needle. If it does
				// we are done.
				if (DoesMatchStartWith(remaining, needle))
				{
					if (needle[0] == Path::Seperator)
					{
						matched += matchedSegment;
						matched += Path::Seperator;
						matchedSegment = "";
					}

					remaining += needle.CharLength();
					break;
				}

				// Keep track of matched path.
				matchedSegment += remaining[0];
				if (remaining[0] == Path::Seperator)
				{
					matched += matchedSegment;
					matchedSegment = "";
				}

				// No recursive matching please. We have finished matching this
				// wildcard.
				if (remaining[0] == Path::Seperator && !bIsRecursive)
				{
					break;
				}

				remaining++;
			}
		}

		// Accept anything remaining in this directory.
		else
		{
			while (true)
			{
				// Got to end of data, we're done.
				if (remaining[0] == '\0')
				{
					return true;
				}

				// Keep track of matched path.
				matchedSegment += remaining[0];
				if (remaining[0] == Path::Seperator)
				{
					matched += matchedSegment;
					matchedSegment = "";
				}

				// Got to a new directory, no match.
				if (remaining[0] == Path::Seperator && !bIsRecursive)
				{
					return false;
				}

				remaining++;
			}
		}
	}

	// Directory split match.
	else if (match == seperatorString)
	{
		if (remaining[0] != Path::Seperator)
		{
			return false;
		}
		remaining++;
	}

	// Literal match.
	else
	{
		const char* matchPtr = match.Data();
		if (!MatchEatNeedle(remaining, matchPtr))
		{
			return false;
		}
	}

	return Matches_r(matchStack, matchStackIndex, remaining, matched, matchedSegment);
}

bool Path::Matches(const Path& other, Path* matched)
{
	// Split into wildcards and fragments.
	Array<String> matchStack;
	if (!SplitIntoMatchStack(other.ToString(), matchStack, true))
	{
		return false;
	}

	// No wildcards? Straight match.
	if (matchStack.Length() == 1)
	{
		return (*this == other);
	}

	// Time to do matching shenanigans.
	else
	{
		String basePath = "";
		TrimMatchStackDownToFirstWildcard(matchStack, basePath);

		// Base paths match?
		if (m_raw.CharLength() < basePath.CharLength() ||
			m_raw.SubString(m_raw.Begin(), m_raw.Begin() + basePath.CharLength()) == basePath)
		{
			String remaining = m_raw.SubString(m_raw.Begin() + basePath.CharLength());

			String matchedStr = "";
			String matchedSegmentStr = "";
			bool result = Matches_r(matchStack, 0, remaining.Data(), matchedStr, matchedSegmentStr);

			if (matched != nullptr)
			{
				*matched = matchedStr;
			}

			return result;
		}

		// Nope not a match then.
		else
		{
			return false;
		}
	}

	return true;
}

}; // namespace Ludo
