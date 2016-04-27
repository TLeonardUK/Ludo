// Copyright 2016 TwinDrills
#pragma once

#include "Core/Containers/Array.h"

#include <string>
#include <vector>
#include <cstdarg>

namespace Ludo {

class String;
struct StringArgument;
struct StringArgumentList;
struct StringArgumentFormatter;

/// \brief TODO
class String
{
public:

	/// \brief TODO
	typedef char CharType;

	/// \brief TODO
	struct MemoryBuffer
	{
		int		RefCount;
		char*	Buffer;
		int		Size;
		bool	Persistent;

		/// This should really be exposed as a template parameter, but for the time being I'm just hard coding
		/// it here to avoid having to inline all the string functions.
		static ArenaAllocator<StringMemoryArena> m_Allocator;

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

private:
	MemoryBuffer* m_Buffer;

public:

	/// \brief TODO
	static const int Infinite = 0x7FFFFFFF;

	/// \brief TODO
	static const int NotFound = -1;

	/// \brief TODO
	static const int NoIndex = -1;

	// -----------------------------------------------------------------------------
	// Constructors
	// -----------------------------------------------------------------------------
	~String();
	String();
	String(const char Value, const int Size);
	String(const int Value);
	String(const float Value);
	String(const char* Value);
	String(const char* Value, const int Length);
	String(const String& Value);
	String(String::MemoryBuffer* Value);
	String(const char Value);
	String(const Array<char>& Value);

	// -----------------------------------------------------------------------------
	// Operator overloads.
	// -----------------------------------------------------------------------------
	int Compare(const String& Other) const;

	String& operator =(const String& Other);
	String  operator +(const String& Other) const;
	String  operator +(const char* Other) const;
	String& operator +=(const String& Other);

	friend String operator+(const char* A, const String& B);

	char operator [](const int Index) const;
	bool operator ==(const String& Other) const;
	bool operator !=(const String& Other) const;
	bool operator <(const String& Other) const;
	bool operator >(const String& Other) const;
	bool operator <=(const String& Other) const;
	bool operator >=(const String& Other) const;

	// -----------------------------------------------------------------------------
	// Properties
	// -----------------------------------------------------------------------------
	int Length() const;
	const char* Data() const;

	// -----------------------------------------------------------------------------
	// Conversion / Formatting
	// -----------------------------------------------------------------------------
	int ToInt() const;
	bool ToBool() const;
	float ToFloat() const;

	bool IsNumeric() const;
	bool IsHex() const;

	bool IsEmpty() const;

	/// \brief TODO
	static String HexToString(const int Value);
	int IntFromHex() const;

	/// \brief TODO
	String Format(const StringArgumentList& Arguments) const;
	static String Format(const String& Format, const StringArgumentList& Arguments) ;

	// -----------------------------------------------------------------------------
	// Searching
	// -----------------------------------------------------------------------------
	bool Contains(const String& Needle) const;
	bool ContainsAny(const Array<String>& Needles) const;

	int IndexOf(const String& Needle, const int StartIndex = NoIndex) const;
	int IndexOfAny(const Array<String>& Needles, const int StartIndex = NoIndex) const;
	int LastIndexOf(const String& Needle, const int EndIndex = NoIndex) const;
	int LastIndexOfAny(const Array<String>& Needles, const int EndIndex = NoIndex) const;

	bool StartsWith(const String& What) const;
	bool EndsWith(const String& What) const;

	// -----------------------------------------------------------------------------
	// Splitting / Joining
	// -----------------------------------------------------------------------------
	Array<String> Split(const String& Seperator, const int MaxSplits, const bool RemoveDuplicates) const;
	void SplitOnIndex(int Index, String& LeftOut, String& RightOut) const;
	String Join(const Array<String>& Fragments) const;
	String Filter(const String& AllowedChars, const String& Replacement) const;

	// -----------------------------------------------------------------------------
	// State changing
	// -----------------------------------------------------------------------------
	String ToLower() const;
	String ToUpper() const;

	// -----------------------------------------------------------------------------
	// General manipulation.
	// -----------------------------------------------------------------------------
	String Replace(const int StartIndex, const int Length, const String& Replacement) const;
	String Replace(const String& What, const String& With) const;
	String Insert(const String& What, const int Offset) const;
	String Remove(const int Start, const int Length) const;

	String Reverse() const;

	String LimitEnd(const int Length, const String& Postfix) const;
	String LimitStart(const int Length, const String& Prefix) const;

	String PadLeft(const int Length, const String& Padding) const;
	String PadRight(const int Length, const String& Padding) const;

	String SubString(const int Offset, const int Count = Infinite) const;

	String Trim(const String& Chars) const;
	String TrimStart(const String& Chars) const;
	String TrimEnd(const String& Chars) const;

	String Slice(const int StartIndex) const;
	String Slice(const int StartIndex, int EndIndex) const;

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
		int64_t		IntValue;
		uint64_t	UnsignedIntValue;
		float		FloatValue;
		double		DoubleValue;
	};

	String StringValue;

	StringArgumentValue(int64_t Value)
		: Type(StringArgumentValueType::Int)
		, IntValue(Value)
	{
	}

	StringArgumentValue(uint64_t Value)
		: Type(StringArgumentValueType::UnsignedInt)
		, UnsignedIntValue(Value)
	{
	}

	StringArgumentValue(int Value)
		: Type(StringArgumentValueType::Int)
		, IntValue((int64_t)Value)
	{
	}

	StringArgumentValue(unsigned int Value)
		: Type(StringArgumentValueType::UnsignedInt)
		, UnsignedIntValue((uint64_t)Value)
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