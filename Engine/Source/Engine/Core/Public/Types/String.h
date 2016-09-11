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

#include "Core/Public/Build/Common.h"
#include "Core/Public/Types/Array.h"
#include "Core/Public/Types/Error.h"

#include <string>
#include <vector>
#include <cstdarg>

namespace Ludo {

class ITextEncoding;

class String;
struct StringArgument;
struct StringArgumentList;
struct StringArgumentFormatter;

// A byte is not the same as a char
// A char is not the same as a grapheme
// todo utf normalisation

/// \brief TODO
class String
{
public:

	typedef char CharType;
	
	/// \brief TODO
	struct MemoryBuffer
	{
		int			RefCount;
		CharType*	Buffer;
		int			Size;
		int			CharSize;
		int			Iteration;
		bool		Persistent;

	public:
		/// \brief TODO
		MemoryBuffer();

		/// \brief TODO
		void Retain();

		/// \brief TODO
		void Release();

		/// \brief TODO
		static MemoryBuffer* Allocate(int size);
	};

	/// \brief TODO
	struct CharIterator
	{
	private:
		friend class String;

		int m_StartByte;
		int m_NextByte;
		int m_Iteration;
		int m_CharIndex;
		MemoryBuffer* m_Buffer;

	public:
		/// \brief TODO
		CharIterator();

		/// \brief TODO
		CharIterator(const CharIterator& Iter);

		/// \brief TODO
		CharIterator(const String& Source, int StartByte, int EndByte, int CharIndex);

		/// \brief TODO
		virtual ~CharIterator();

		/// \brief TODO
		CharIterator& operator =(const CharIterator& Other);

		/// \brief TODO
		bool ValidFor(const String& Other) const;

		/// \brief TODO
		bool IsValid() const;

		/// \brief TODO
		bool Exists() const;

		/// \brief TODO
		int CharIndex() const;

		/// \brief TODO
		const CharIterator& operator++();

		/// \brief TODO
		const CharIterator& operator--();

		/// \brief TODO
		const CharIterator operator++(int);

		/// \brief TODO
		const CharIterator operator--(int);

		/// \brief TODO
		CharIterator& operator+=(int CharOffset);

		/// \brief TODO
		CharIterator& operator-=(int CharOffset);

		/// \brief TODO
		CharIterator operator+(int CharOffset) const;

		/// \brief TODO
		CharIterator operator-(int CharOffset) const;

		/// \brief TODO
		bool operator< (const CharIterator& rhs) const;

		/// \brief TODO
		bool operator> (const CharIterator& rhs) const;

		/// \brief TODO
		bool operator<=(const CharIterator& rhs) const;

		/// \brief TODO
		bool operator>=(const CharIterator& rhs) const;

		/// \brief TODO
		bool operator==(const CharIterator& Other) const;

		/// \brief TODO
		bool operator!=(const CharIterator& Other) const;

		/// \brief TODO
		String operator*();

		/// \brief TODO
		operator bool() const;

		/// \brief TODO
		void Advance(int Offset);
	};

	/// \brief TODO
	struct GraphemeIterator
	{
		// Returns chariterator of start/end.
		//CharIterator
	};

private:
	MemoryBuffer* m_Buffer;

	/// \brief TODO
	static int GetBufferCharLength(const CharType* Start, const CharType* End);

public:

	/// \brief TODO
	static const int Infinite = 0x7FFFFFFF;

	// -----------------------------------------------------------------------------
	// Constructors
	// -----------------------------------------------------------------------------

	~String();
	String();

	/// \brief TODO
	String(const CharType Value, const int Size);

	/// \brief TODO
	String(const int Value);

	/// \brief TODO
	String(const float Value);

	/// \brief TODO
	String(const CharType* Value);

	/// \brief TODO
	String(const CharType* Value, const int Length);

	/// \brief TODO
	String(const String& Value);

	/// \brief TODO
	String(String::MemoryBuffer* Value);

	/// \brief TODO
	String(const CharType Value);

	/// \brief TODO
	String(const Array<CharType>& Value);

	/// \brief TODO
	String(std::initializer_list<CharType> List);

	// -----------------------------------------------------------------------------
	// Operator overloads.
	// -----------------------------------------------------------------------------

	/// \brief TODO
	int Compare(const String& Other) const;

	String& operator =(const String& Other);
	String  operator +(const String& Other) const;
	String  operator +(const CharType* Other) const;
	String& operator +=(const String& Other);

	friend String operator+(const char* A, const String& B);

	String operator [](const int Index) const;
	bool operator ==(const String& Other) const;
	bool operator !=(const String& Other) const;
	bool operator <(const String& Other) const;
	bool operator >(const String& Other) const;
	bool operator <=(const String& Other) const;
	bool operator >=(const String& Other) const;

	// -----------------------------------------------------------------------------
	// Iteration 
	// -----------------------------------------------------------------------------

	/// \brief TODO
	int CharLength() const;

	/// \brief TODO
	int ByteLength() const;

	/// \brief TODO
	const CharType* Data() const;

	/// \brief TODO
	CharIterator GetCharIterator(int CharOffset) const;

	/// \brief TODO
	CharIterator BeginCharIterator() const;

	/// \brief TODO
	CharIterator EndCharIterator() const;
	
	/// \brief TODO
	CharIterator Begin() const;

	/// \brief TODO
	CharIterator End() const;

	/*
	/// \brief TODO
	GraphemeIterator GetGraphemeIterator() const;

	/// \brief TODO
	GraphemeIterator GetGraphemeIterator(CharIterator Start) const;
	*/

	/*
	CharIterator BeginGraphemes();

	CharIterator EndGraphemes();

	ConstCharIterator BeginGraphemes() const;

	ConstCharIterator EndGraphemes() const;
	*/

	// Shouldn't use these directly, only here to support for-range loops.
	CharIterator begin();
	CharIterator end();

	CharIterator begin() const;
	CharIterator end() const;

	// -----------------------------------------------------------------------------
	// Conversion / Formatting
	// -----------------------------------------------------------------------------

	/// \brief TODO
	unsigned int ToHash() const;

	/// \brief TODO
	int ToInt() const;

	/// \brief TODO
	bool ToBool() const;

	/// \brief TODO
	float ToFloat() const;

	/// \brief TODO
	bool IsNumeric() const;

	/// \brief TODO
	bool IsAlpha() const;

	/// \brief TODO
	bool IsAlphaNumeric() const;

	/// \brief TODO
	bool IsHex() const;

	/// \brief TODO
	bool IsEmpty() const;

	/// \brief TODO
	static String HexToString(const int Value);

	/// \brief TODO
	int IntFromHex() const;

	/// \brief TODO
	static String Format(String Format, ...);

	/// \brief TODO
	String FormatArgs(const StringArgumentList& Arguments) const;

	/// \brief TODO
	static String FormatArgs(const String& Format, const StringArgumentList& Arguments);

	/// \brief TODO
	String FormatVA(va_list list) const;

	/// \brief TODO
	static String FormatVA(const String& Format, va_list list);

	// -----------------------------------------------------------------------------
	// Searching
	// -----------------------------------------------------------------------------

	/// \brief TODO
	bool Contains(const String& Needle) const;

	/// \brief TODO
	bool ContainsAny(const Array<String>& Needles) const;

	/// \brief TODO
	CharIterator IndexOf(const String& Needle) const;

	/// \brief TODO
	CharIterator IndexOf(const String& Needle, const CharIterator& Start) const;

	/// \brief TODO
	CharIterator IndexOfAny(const Array<String>& Needles) const;

	/// \brief TODO
	CharIterator IndexOfAny(const Array<String>& Needles, const CharIterator& Start) const;

	/// \brief TODO
	CharIterator LastIndexOf(const String& Needle) const;

	/// \brief TODO
	CharIterator LastIndexOf(const String& Needle, const CharIterator& End) const;

	/// \brief TODO
	CharIterator LastIndexOfAny(const Array<String>& Needles) const;

	/// \brief TODO
	CharIterator LastIndexOfAny(const Array<String>& Needles, const CharIterator& End) const;

	/// \brief TODO
	bool StartsWith(const String& What) const;

	/// \brief TODO
	bool EndsWith(const String& What) const;

	// -----------------------------------------------------------------------------
	// Splitting / Joining
	// -----------------------------------------------------------------------------

	/// \brief TODO
	Array<String> Split(const String& Seperator, const int MaxSplits, const bool RemoveDuplicates) const;

	/// \brief TODO
	void SplitOnIndex(const CharIterator& Index, String& LeftOut, String& RightOut) const;

	/// \brief TODO
	String Join(const Array<String>& Fragments) const;

	/// \brief TODO
	String Filter(const String& AllowedChars, const String& Replacement) const;

	// -----------------------------------------------------------------------------
	// State changing
	// -----------------------------------------------------------------------------

	/// \brief TODO
	String ToLower() const;

	/// \brief TODO
	String ToUpper() const;

	/// \brief TODO
	String Escape() const;

	/// \brief TODO
	String DoubleQuoted() const;

	// -----------------------------------------------------------------------------
	// General manipulation.
	// -----------------------------------------------------------------------------

	/// \brief TODO
	String Replace(const CharIterator& Start, const int CharLength, const String& Replacement) const;

	/// \brief TODO
	String Replace(const CharIterator& Start, const CharIterator& End, const String& Replacement) const;

	/// \brief TODO
	String Replace(const String& What, const String& With) const;

	/// \brief TODO
	String Insert(const String& What, const CharIterator& Offset) const;

	/// \brief TODO
	String Remove(const CharIterator& Start, const int CharLength) const;

	/// \brief TODO
	String Remove(const CharIterator& Start, const CharIterator& End) const;

	/// \brief TODO
	String Reverse() const;

	/// \brief TODO
	String LimitEnd(const int CharLength, const String& Postfix) const;

	/// \brief TODO
	String LimitStart(const int CharLength, const String& Prefix) const;

	/// \brief TODO
	String PadLeft(const int CharLength, const String& Padding) const;

	/// \brief TODO
	String PadRight(const int CharLength, const String& Padding) const;

	/// \brief TODO
	String SubString(const CharIterator& Start, const int CharCount = Infinite) const;

	/// \brief TODO
	String SubString(const CharIterator& Start, const CharIterator& End) const;

	/// \brief TODO
	String Trim(const String& Chars) const;

	/// \brief TODO
	String TrimStart(const String& Chars) const;

	/// \brief TODO
	String TrimEnd(const String& Chars) const;

	/// \brief TODO
	String Slice(const CharIterator& Start) const;

	/// \brief TODO
	String Slice(const CharIterator& Start, const CharIterator& End) const;

};

/// \brief TODO
struct StringArgumentFormatter
{
public:

	/// \brief TODO
	virtual void Format(const StringArgument& Argument, String& Output) = 0;
};

/// \brief TODO
enum class StringArgumentValueType
{
	Int,
	UnsignedInt,
	Float,
	Double,
	String
};

/// \brief TODO
struct StringArgumentValue
{
public:
	StringArgumentValueType Type;

	union
	{
		int64		IntValue;
		uint64		UnsignedIntValue;
		float		FloatValue;
		double		DoubleValue;
	};

	String StringValue;

	StringArgumentValue(int64 Value)
		: Type(StringArgumentValueType::Int)
		, IntValue(Value)
	{
	}

	StringArgumentValue(uint64 Value)
		: Type(StringArgumentValueType::UnsignedInt)
		, UnsignedIntValue(Value)
	{
	}

	StringArgumentValue(int Value)
		: Type(StringArgumentValueType::Int)
		, IntValue((int64)Value)
	{
	}

	StringArgumentValue(unsigned int Value)
		: Type(StringArgumentValueType::UnsignedInt)
		, UnsignedIntValue((uint64)Value)
	{
	}

	StringArgumentValue(float Value)
		: Type(StringArgumentValueType::Float)
		, FloatValue(Value)
	{
	}

	StringArgumentValue(double Value)
		: Type(StringArgumentValueType::Double)
		, DoubleValue(Value)
	{
	}

	StringArgumentValue(String Value)
		: Type(StringArgumentValueType::String)
		, StringValue(Value)
	{
	}

	StringArgumentValue(const char* Value)
		: Type(StringArgumentValueType::String)
		, StringValue(Value)
	{
	}

};

/// \brief TODO
struct StringArgument
{
public:
	String Tag;
	StringArgumentValue Value;
	StringArgumentFormatter* Formatter;

	/// \brief TODO
	StringArgument();
	StringArgument(String InTag, StringArgumentValue InValue, StringArgumentFormatter* InFormatter);
};

/// \brief TODO
struct StringArgumentList
{
public:
	Array<StringArgument> Arguments;

	/// \brief TODO
	void Add(String Tag, StringArgumentValue Value, StringArgumentFormatter* Formatter = nullptr);
};

};