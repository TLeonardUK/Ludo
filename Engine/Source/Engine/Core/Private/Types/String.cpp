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

#include "Core/Public/Types/String.h"
#include "Core/Public/Types/Formatters/SimpleStringFormatter.h"
#include "Core/Public/Encodings/ITextEncoding.h"

#include "ThirdParty/Utf8Cpp/source/utf8.h"

namespace Ludo {

// -----------------------------------------------------------------------------
// String Arguments
// -----------------------------------------------------------------------------

StringArgument::StringArgument()
	: Tag("")
	, Value("")
	, Formatter(nullptr)
{
}

StringArgument::StringArgument(String InTag, StringArgumentValue InValue, StringArgumentFormatter* InFormatter)
	: Tag(InTag)
	, Value(InValue)
	, Formatter(InFormatter)
{
}

void StringArgumentList::Add(String Tag, StringArgumentValue Value, StringArgumentFormatter* Formatter)
{
	StringArgument Argument(Tag, Value, Formatter);

	Arguments.RemoveMatches(
		[Tag] (const StringArgument* A) {
			return A->Tag == Tag;
		}
	);

	Arguments.Add(Argument);
}

struct GlobalStrings
{
	String::MemoryBuffer* m_EmptyBuffer;

	GlobalStrings()
	{
		m_EmptyBuffer = LD_ARENA_NEW(StringMemoryArena, String::MemoryBuffer);
		m_EmptyBuffer->Buffer = LD_ARENA_NEW_ARRAY(StringMemoryArena, String::CharType, 1);
		m_EmptyBuffer->Buffer[0] = '\0';
		m_EmptyBuffer->Size = 0;
		m_EmptyBuffer->RefCount = 1;
		m_EmptyBuffer->Persistent = true;
		m_EmptyBuffer->CharSize = 0;
		m_EmptyBuffer->Iteration = 0;
	}
};

GlobalStrings gGlobalStrings;

// -----------------------------------------------------------------------------
// Memory Buffer
// -----------------------------------------------------------------------------

String::MemoryBuffer::MemoryBuffer()
	: RefCount(1)
	, Size(0)
	, Buffer(nullptr)
	, Persistent(false)
{
}

void String::MemoryBuffer::Retain()
{
	RefCount++;
}

void String::MemoryBuffer::Release()
{
	RefCount--;

	if (RefCount <= 0)
	{
		if (Persistent == false)
		{
			LD_ARENA_DELETE_ARRAY(StringMemoryArena, Buffer);
			Buffer = nullptr;
			LD_ARENA_DELETE(StringMemoryArena, this);
		}
	}
}

String::MemoryBuffer* String::MemoryBuffer::Allocate(int Size)
{
	if (Size == 0)
	{
		return gGlobalStrings.m_EmptyBuffer;
	}

	MemoryBuffer* Buffer = LD_ARENA_NEW(StringMemoryArena, String::MemoryBuffer);
	Buffer->Buffer = LD_ARENA_NEW_ARRAY(StringMemoryArena, String::CharType, Size + 1);
	Buffer->Buffer[Size] = '\0';
	Buffer->Size = Size;
	Buffer->RefCount = 1;
	Buffer->CharSize = 0;
	Buffer->Iteration = 0;

	return Buffer;
}


// -----------------------------------------------------------------------------
// Iterators
// -----------------------------------------------------------------------------
String::CharIterator::CharIterator()
	: m_StartByte(-1)
	, m_NextByte(-1)
	, m_Iteration(-1)
	, m_CharIndex(-1)
	, m_Buffer(nullptr)
{
}

String::CharIterator::CharIterator(const String& Source, int StartByte, int NextByte, int CharIndex)
{
	m_StartByte = StartByte;
	m_NextByte = NextByte;
	m_Iteration = Source.m_Buffer->Iteration;
	m_CharIndex = CharIndex;
	m_Buffer = Source.m_Buffer;
	if (m_Buffer != nullptr)
	{
		m_Buffer->Retain();
	}
}

String::CharIterator::CharIterator(const CharIterator& Iter)
{
	m_StartByte = Iter.m_StartByte;
	m_NextByte = Iter.m_NextByte;
	m_Iteration = Iter.m_Iteration;
	m_CharIndex = Iter.m_CharIndex;
	m_Buffer = Iter.m_Buffer;
	if (m_Buffer != nullptr)
	{
		m_Buffer->Retain();
	}
}

String::CharIterator::~CharIterator()
{
	if (m_Buffer != nullptr)
	{
		m_Buffer->Release();
	}
}

String::CharIterator& String::CharIterator::CharIterator::operator =(const CharIterator& Iter)
{
	m_StartByte = Iter.m_StartByte;
	m_NextByte = Iter.m_NextByte;
	m_Iteration = Iter.m_Iteration;
	m_CharIndex = Iter.m_CharIndex;

	if (Iter.m_Buffer != nullptr)
	{
		Iter.m_Buffer->Retain();
	}
	if (m_Buffer != nullptr)
	{
		m_Buffer->Release();
	}
	m_Buffer = Iter.m_Buffer;

	return *this;
}

bool String::CharIterator::ValidFor(const String& Other) const
{
	return IsValid() && (Other.m_Buffer == m_Buffer);
}

int String::CharIterator::CharIndex() const
{
	return m_CharIndex;
}

bool String::CharIterator::IsValid() const
{
	return (m_StartByte >= 0 && m_NextByte >= m_StartByte && m_CharIndex >= 0 && m_CharIndex <= m_Buffer->CharSize && m_Iteration == m_Buffer->Iteration);
}

bool String::CharIterator::Exists() const
{
	return (m_StartByte >= 0 && m_NextByte > m_StartByte && m_CharIndex >= 0 && m_CharIndex < m_Buffer->CharSize && m_Iteration == m_Buffer->Iteration);
}

const String::CharIterator& String::CharIterator::operator++()
{
	AssertMsg(m_Iteration == m_Buffer->Iteration, "Character iterator is no longer valid, string buffer has been changed.");
	Advance(1);
	return *this;
}

const String::CharIterator& String::CharIterator::operator--()
{
	AssertMsg(m_Iteration == m_Buffer->Iteration, "Character iterator is no longer valid, string buffer has been changed.");
	Advance(-1);
	return *this;
}

const String::CharIterator String::CharIterator::operator++(int)
{
	AssertMsg(m_Iteration == m_Buffer->Iteration, "Character iterator is no longer valid, string buffer has been changed.");
	Advance(1);
	return *this;
}

const String::CharIterator String::CharIterator::operator--(int)
{
	AssertMsg(m_Iteration == m_Buffer->Iteration, "Character iterator is no longer valid, string buffer has been changed.");
	Advance(-1);
	return *this;
}

String::CharIterator& String::CharIterator::operator+=(int CharOffset)
{
	AssertMsg(m_Iteration == m_Buffer->Iteration, "Character iterator is no longer valid, string buffer has been changed.");
	Advance(CharOffset);
	return *this;
}

String::CharIterator& String::CharIterator::operator-=(int CharOffset)
{
	AssertMsg(m_Iteration == m_Buffer->Iteration, "Character iterator is no longer valid, string buffer has been changed.");
	Advance(-CharOffset);
	return *this;
}

String::CharIterator String::CharIterator::operator+(int CharOffset) const
{
	AssertMsg(m_Iteration == m_Buffer->Iteration, "Character iterator is no longer valid, string buffer has been changed.");

	String::CharIterator Copy = *this;
	Copy.Advance(CharOffset);

	return Copy;
}

String::CharIterator String::CharIterator::operator-(int CharOffset) const
{
	AssertMsg(m_Iteration == m_Buffer->Iteration, "Character iterator is no longer valid, string buffer has been changed.");

	String::CharIterator Copy = *this;
	Copy.Advance(-CharOffset);

	return Copy;
}

void String::CharIterator::Advance(int Offset)
{
	try
	{
		if (Offset > 0)
		{
			CharType* OctetIterator = m_Buffer->Buffer + m_NextByte;

			do
			{
				m_StartByte = m_NextByte;
				if (m_NextByte >= m_Buffer->Size)
				{
					m_StartByte = m_Buffer->Size;
					m_NextByte = m_Buffer->Size;
					m_CharIndex = m_Buffer->CharSize;
					break;
				}

				utf8::next(OctetIterator, m_Buffer->Buffer + m_Buffer->Size);
				m_NextByte = (int)(OctetIterator - m_Buffer->Buffer);
				m_CharIndex++;
			} while (--Offset > 0);
		}
		else if (Offset < 0)
		{
			CharType* OctetIterator = m_Buffer->Buffer + m_StartByte;

			do
			{
				m_NextByte = m_StartByte;
				if (m_NextByte <= 0)
				{
					m_StartByte = 0;
					m_NextByte = 0;
					m_CharIndex = m_Buffer->CharSize;
					break;
				}

				utf8::prior(OctetIterator, m_Buffer->Buffer);
				m_StartByte = (int)(OctetIterator - m_Buffer->Buffer);
				m_CharIndex--;
			} while (--Offset > 0);
		}
	}
	catch (utf8::exception)
	{
		// :( Invalid utf.
	}
}

bool String::CharIterator::operator< (const CharIterator& rhs) const
{
	AssertMsg(m_Iteration == rhs.m_Iteration && m_Buffer == rhs.m_Buffer, "Comparison of incompatible character iterators.");
	return m_CharIndex < rhs.m_CharIndex;
}

bool String::CharIterator::operator> (const CharIterator& rhs) const
{
	AssertMsg(m_Iteration == rhs.m_Iteration && m_Buffer == rhs.m_Buffer, "Comparison of incompatible character iterators.");
	return m_CharIndex > rhs.m_CharIndex;
}

bool String::CharIterator::operator<=(const CharIterator& rhs) const
{
	AssertMsg(m_Iteration == rhs.m_Iteration && m_Buffer == rhs.m_Buffer, "Comparison of incompatible character iterators.");
	return m_CharIndex <= rhs.m_CharIndex;
}

bool String::CharIterator::operator>=(const CharIterator& rhs) const
{
	AssertMsg(m_Iteration == rhs.m_Iteration && m_Buffer == rhs.m_Buffer, "Comparison of incompatible character iterators.");
	return m_CharIndex >= rhs.m_CharIndex;
}

bool String::CharIterator::operator==(const CharIterator& rhs) const
{
	AssertMsg(m_Iteration == rhs.m_Iteration && m_Buffer == rhs.m_Buffer, "Comparison of incompatible character iterators.");
	return m_StartByte == rhs.m_StartByte;
}

bool String::CharIterator::operator!=(const CharIterator& Other) const
{
	return !operator==(Other);
}

String String::CharIterator::operator*()
{
	AssertMsg(m_StartByte < m_Buffer->Size, "Attempt to dereference out of range (probably end()?) iterator.");
	return String(m_Buffer->Buffer + m_StartByte, m_NextByte - m_StartByte);
}


String::CharIterator::operator bool() const
{
	return m_CharIndex >= 0 && m_CharIndex < m_Buffer->CharSize;
}

// -----------------------------------------------------------------------------
// Constructors
// -----------------------------------------------------------------------------

String::~String()
{
	m_Buffer->Release();
}

String::String()
{
	m_Buffer = gGlobalStrings.m_EmptyBuffer;
}

String::String(const CharType Value, const int Size)
{
	m_Buffer = MemoryBuffer::Allocate(Size);
	memset(m_Buffer->Buffer, Value, Size);

	m_Buffer->CharSize = GetBufferCharLength(m_Buffer->Buffer, m_Buffer->Buffer + m_Buffer->Size);
	m_Buffer->Iteration++;
}

String::String(const int Value)
{
	char Buffer[128];
	sprintf(Buffer, "%i", Value);

	int Length = (int)strlen(Buffer);
	m_Buffer = MemoryBuffer::Allocate(Length);
	memcpy(m_Buffer->Buffer, Buffer, Length);

	m_Buffer->CharSize = GetBufferCharLength(m_Buffer->Buffer, m_Buffer->Buffer + m_Buffer->Size);
	m_Buffer->Iteration++;
}

String::String(const float Value)
{
	char Buffer[128];
	sprintf(Buffer, "%f", Value);

	int Length = (int)strlen(Buffer);
	m_Buffer = MemoryBuffer::Allocate(Length);
	memcpy(m_Buffer->Buffer, Buffer, Length);

	m_Buffer->CharSize = GetBufferCharLength(m_Buffer->Buffer, m_Buffer->Buffer + m_Buffer->Size);
	m_Buffer->Iteration++;
}

String::String(const CharType* Value)
{
	int Length = (int)strlen(Value);
	m_Buffer = MemoryBuffer::Allocate(Length);
	memcpy(m_Buffer->Buffer, Value, Length);

	m_Buffer->CharSize = GetBufferCharLength(m_Buffer->Buffer, m_Buffer->Buffer + m_Buffer->Size);
	m_Buffer->Iteration++;
}

String::String(const CharType* Value, const int Length)
{
	m_Buffer = MemoryBuffer::Allocate(Length);
	memcpy(m_Buffer->Buffer, Value, Length);

	int realLen = (int)strlen(m_Buffer->Buffer);
	Assert(realLen == m_Buffer->Size);

	m_Buffer->CharSize = GetBufferCharLength(m_Buffer->Buffer, m_Buffer->Buffer + m_Buffer->Size);
	m_Buffer->Iteration++;
}

String::String(String::MemoryBuffer* Value)
{
	m_Buffer = Value;
}

String::String(const String& Value)
{
	m_Buffer = Value.m_Buffer;
	m_Buffer->Retain();
}

String::String(const CharType Value)
{
	m_Buffer = MemoryBuffer::Allocate(1);
	m_Buffer->Buffer[0] = Value;

	m_Buffer->CharSize = GetBufferCharLength(m_Buffer->Buffer, m_Buffer->Buffer + m_Buffer->Size);
	m_Buffer->Iteration++;
}

String::String(const Array<CharType>& Value)
{
	m_Buffer = MemoryBuffer::Allocate(Value.Length());
	for (int i = 0; i < Value.Length(); i++)
	{
		m_Buffer->Buffer[i] = Value[i];
	}

	m_Buffer->CharSize = GetBufferCharLength(m_Buffer->Buffer, m_Buffer->Buffer + m_Buffer->Size);
	m_Buffer->Iteration++;
}

String::String(std::initializer_list<CharType> List)
{

	m_Buffer = MemoryBuffer::Allocate((int)List.size());
	for (size_t i = 0; i < List.size(); i++)
	{
		m_Buffer->Buffer[i] = *(List.begin() + i);
	}

	m_Buffer->CharSize = GetBufferCharLength(m_Buffer->Buffer, m_Buffer->Buffer + m_Buffer->Size);
	m_Buffer->Iteration++;
}

// -----------------------------------------------------------------------------
// Operator overloads.
// -----------------------------------------------------------------------------

int String::Compare(const String& Other) const
{
	int MinSize = (Other.m_Buffer->Size < m_Buffer->Size ?
		Other.m_Buffer->Size :
		m_Buffer->Size);

	for (int i = 0; i < MinSize; i++)
	{
		int Diff = (m_Buffer->Buffer[i] - Other.m_Buffer->Buffer[i]);
		if (Diff != 0)
		{
			return Diff;
		}
	}

	return (Other.m_Buffer->Size - m_Buffer->Size);
}

String& String::operator =(const String& Other)
{
	Other.m_Buffer->Retain();
	m_Buffer->Release();
	m_Buffer = Other.m_Buffer;
	return *this;
}

String String::operator +(const String& Other) const
{
	if (m_Buffer->Size == 0)
	{
		return Other;
	}
	if (Other.m_Buffer->Size == 0)
	{
		return *this;
	}

	MemoryBuffer* NewBuffer = MemoryBuffer::Allocate(m_Buffer->Size + Other.m_Buffer->Size);

	memcpy(NewBuffer->Buffer,
		m_Buffer->Buffer,
		m_Buffer->Size);

	memcpy(NewBuffer->Buffer + m_Buffer->Size,
		Other.m_Buffer->Buffer,
		Other.m_Buffer->Size);

	NewBuffer->CharSize = m_Buffer->CharSize + Other.m_Buffer->CharSize;
	NewBuffer->Iteration++;

	return String(NewBuffer);
}

String String::operator +(const char* Other) const
{
	int OtherLen = (int)strlen(Other);

	if (m_Buffer->Size == 0)
	{
		return Other;
	}
	if (OtherLen == 0)
	{
		return *this;
	}

	MemoryBuffer* NewBuffer = MemoryBuffer::Allocate(m_Buffer->Size + OtherLen);

	memcpy(NewBuffer->Buffer,
		m_Buffer->Buffer,
		m_Buffer->Size);

	memcpy(NewBuffer->Buffer + m_Buffer->Size,
		Other,
		OtherLen);

	NewBuffer->CharSize = m_Buffer->CharSize + GetBufferCharLength(Other, Other + OtherLen);
	NewBuffer->Iteration++;

	return String(NewBuffer);
}

String& String::operator +=(const String& Other)
{
	return operator =(*this + Other);
}

String operator+(const char* A, const String& B)
{
	return String(A) + B;
}

String String::operator [](const int Index) const
{
	Assert(Index >= 0 && Index < m_Buffer->CharSize);

	CharIterator Iterator = GetCharIterator(Index);
	
	return String(m_Buffer->Buffer + Iterator.m_StartByte, Iterator.m_NextByte - Iterator.m_StartByte);
}

bool String::operator ==(const String& Other) const
{
	if (m_Buffer->Size != Other.m_Buffer->Size)
	{
		return false;
	}
	return (memcmp(m_Buffer->Buffer, Other.m_Buffer->Buffer, m_Buffer->Size) == 0);
}

bool String::operator !=(const String& Other) const
{
	return !(operator ==(Other));
}

bool String::operator <(const String& Other) const
{
	return Compare(Other) < 0;
}

bool String::operator >(const String& Other) const
{
	return Compare(Other) > 0;
}

bool String::operator <=(const String& Other) const
{
	return Compare(Other) <= 0;
}

bool String::operator >=(const String& Other) const
{
	return Compare(Other) >= 0;
}

// -----------------------------------------------------------------------------
// Iteration
// -----------------------------------------------------------------------------
int String::CharLength() const
{
	return m_Buffer->CharSize;
}

int String::ByteLength() const
{
	return m_Buffer->Size;
}

const char* String::Data() const
{
	return m_Buffer->Buffer;
}

int String::GetBufferCharLength(const CharType* Start, const CharType* End)
{
	int Length = 0;
	try
	{
		Length = (int)utf8::distance(Start, End);
	}
	catch (utf8::exception)
	{
		// Someones passed in invalid utf8 here. Ignore error and set char length to 0.
	}
	return Length;
}

String::CharIterator String::GetCharIterator(int CharOffset) const
{
	AssertMsg(CharOffset >= 0 && CharOffset <= m_Buffer->CharSize, "Attempt to get iterator for invalid character index.");

	CharIterator StartIter = CharIterator(*this, 0, 0, 0);
	StartIter.Advance(1);
	StartIter.m_CharIndex = 0;

	if (CharOffset == 0)
	{
		return StartIter;
	}

	return StartIter + CharOffset;
}

String::CharIterator String::BeginCharIterator() const
{
	return GetCharIterator(0);
}

String::CharIterator String::EndCharIterator() const
{
	return CharIterator(*this, m_Buffer->Size, m_Buffer->Size, m_Buffer->CharSize);
}

String::CharIterator String::Begin() const
{
	return BeginCharIterator();
}

String::CharIterator String::End() const
{
	return EndCharIterator();
}

/*
String::GraphemeIterator String::GetGraphemeIterator() const
{
	GraphemeIterator Iterator;
	
	// TODO
	NotImplemented();

	return Iterator;
}

String::GraphemeIterator String::GetGraphemeIterator(CharIterator Start) const
{
	GraphemeIterator Iterator;

	// TODO
	NotImplemented();

	return Iterator;
}
*/

String::CharIterator String::begin()
{
	return BeginCharIterator();
}

String::CharIterator String::end()
{
	return EndCharIterator();
}

String::CharIterator String::begin() const
{
	return BeginCharIterator();
}

String::CharIterator String::end() const
{
	return EndCharIterator();
}

// -----------------------------------------------------------------------------
// Conversion / Formatting
// -----------------------------------------------------------------------------
int String::ToInt() const
{
	return atoi(m_Buffer->Buffer);
}

unsigned int String::ToHash() const
{
	unsigned int Hash = 0;
	const char* Value = m_Buffer->Buffer;

	for (; *Value; ++Value)
	{
		Hash += *Value;
		Hash += (Hash << 10);
		Hash ^= (Hash >> 6);
	}

	Hash += (Hash << 3);
	Hash ^= (Hash >> 11);
	Hash += (Hash << 15);

	return Hash;
}

bool String::ToBool() const
{
	return m_Buffer->Size == 0 || !(stricmp(m_Buffer->Buffer, "false") == 0 || stricmp(m_Buffer->Buffer, "0") == 0);
}

float String::ToFloat() const
{
	return (float)atof(m_Buffer->Buffer);
}

bool String::IsEmpty() const
{
	return (ByteLength() == 0);
}

bool String::IsNumeric() const
{
	for (int i = 0; i < ByteLength(); i++)
	{
		CharType Chr = m_Buffer->Buffer[i];
		bool IsNumber = (Chr >= '0' && Chr <= '9');
		if (!IsNumber)
		{
			return false;
		}
	}
	return true;
}

bool String::IsAlpha() const
{
	for (int i = 0; i < ByteLength(); i++)
	{
		CharType Chr = m_Buffer->Buffer[i];
		bool IsVal = (Chr >= 'A' && Chr <= 'Z') || (Chr >= 'a' && Chr <= 'z');
		if (!IsVal)
		{
			return false;
		}
	}
	return true;
}

bool String::IsAlphaNumeric() const
{
	for (int i = 0; i < ByteLength(); i++)
	{
		CharType Chr = m_Buffer->Buffer[i];
		bool IsVal = (Chr >= '0' && Chr <= '9') || (Chr >= 'A' && Chr <= 'Z') || (Chr >= 'a' && Chr <= 'z');
		if (!IsVal)
		{
			return false;
		}
	}
	return true;
}

bool String::IsHex() const
{
	for (int i = 0; i < ByteLength(); i++)
	{
		CharType Chr = m_Buffer->Buffer[i];
		bool IsHex = (Chr >= '0' && Chr <= '9') || (Chr >= 'A' && Chr <= 'F') || (Chr >= 'a' && Chr <= 'f');
		if (!IsHex)
		{
			return false;
		}
	}
	return true;
}

String String::HexToString(const int Value)
{
	std::stringstream Stream;
	Stream << std::hex << Value;
	return String(Stream.str().c_str());
}

int String::IntFromHex() const
{
	return strtol(Data(), nullptr, 16);
}

String String::Format(String Format, ...)
{
	va_list list;
	va_start(list, Format);
	String Result = FormatVA(Format, list);
	va_end(list);

	return Result;
}

String String::FormatArgs(const StringArgumentList& Arguments) const
{
	// No arguments, just return the format string verbatim.
	if (Arguments.Arguments.Length() == 0)
	{
		return *this;
	}

	SimpleStringArgumentFormatter DefaultFormatter;

	String Result = "";

	bool bInTag = false;
	CharIterator TagStart;
	CharIterator Iterator = BeginCharIterator();
	CharIterator EndIterator = EndCharIterator();

	while (Iterator != EndIterator)
	{
		CharIterator NextIterator = Iterator + 1;

		bool bSupressAddChar = false;

		String Char = *Iterator;
		if (Char == "{")
		{
			bool bIsEscaped = false;

			if (NextIterator != EndIterator)
			{
				String LaChar = *NextIterator;
				if (LaChar == "{")
				{
					bIsEscaped = true;
				}
			}

			if (bIsEscaped)
			{
				// Skip first brace accept next one.
				Iterator = NextIterator;
				NextIterator = Iterator + 1;
			}
			else
			{
				bInTag = true;
				TagStart = NextIterator;
			}
		}
		else if (Char == "}")
		{
			if (bInTag)
			{
				String Tag = Slice(TagStart, Iterator);
				String Replacement = "";
				bool bReplacementFound = false;

				for (const StringArgument& Argument : Arguments.Arguments)
				{
					if (Tag == Argument.Tag)
					{
						if (Argument.Formatter == nullptr)
						{
							DefaultFormatter.Format(Argument, Replacement);
						}
						else
						{
							Argument.Formatter->Format(Argument, Replacement);
						}
						bReplacementFound = true;
						break;
					}
				}

				if (bReplacementFound)
				{
					Result += Replacement;
				}

				bInTag = false;
				bSupressAddChar = true;
			}
		}

		if (!bInTag && !bSupressAddChar)
		{
			Result += Char;
		}
	
		Iterator = NextIterator;
	}

	return Result;
}

String String::FormatArgs(const String& Format, const StringArgumentList& Arguments)
{
	return Format.FormatArgs(Arguments);
}

String String::FormatVA(va_list list) const
{
	const int STACK_BUFFER_SIZE = 512;

	CharType StackBuffer[STACK_BUFFER_SIZE];
	CharType* Buffer = StackBuffer;

	int Size = vsnprintf(Buffer, STACK_BUFFER_SIZE, Data(), list);
	
	// Encoding error.
	if (Size < 0)
	{
		return "";
	}
	
	// Not enough size in buffer.
	else if (Size >= STACK_BUFFER_SIZE)
	{
		Buffer = LD_ARENA_NEW_ARRAY(StringMemoryArena, CharType, Size + 1);
		int Result = vsnprintf(Buffer, Size + 1, Data(), list);
		
		// Encoding error.
		if (Result < 0)
		{
			return "";
		}

		// Wut, this should never happen.
		else if (Result >= Size)
		{
			return "";
		}
	}

	String Result(Buffer, Size);

	if (Buffer != StackBuffer)
	{
		LD_ARENA_DELETE_ARRAY(StringMemoryArena, Buffer);
	}

	return Buffer;
}

String String::FormatVA(const String& Format, va_list List)
{
	return Format.FormatVA(List);
}

// -----------------------------------------------------------------------------
// Searching
// -----------------------------------------------------------------------------

bool String::Contains(const String& Needle) const
{
	return (IndexOf(Needle).Exists());
}

bool String::ContainsAny(const Array<String>& Needles) const
{
	return (IndexOfAny(Needles).Exists());
}

String::CharIterator String::IndexOf(const String& Needle) const
{
	return IndexOf(Needle, BeginCharIterator());
}

String::CharIterator String::IndexOf(const String& Needle, const CharIterator& StartIndex) const
{
	String Result = "";
	if (Needle.IsEmpty())
	{
		return EndCharIterator();
	}

	// TODO: Assuming Needle is a valid utf8 string we should just be able to do a byte comparison.

	CharIterator BeginIterator = StartIndex;
	CharIterator EndIterator = EndCharIterator();

	for (; BeginIterator.m_CharIndex < (CharLength() - Needle.CharLength()) + 1; BeginIterator++)
	{
		if (memcmp(m_Buffer->Buffer + BeginIterator.m_StartByte, Needle.m_Buffer->Buffer, Needle.m_Buffer->Size) == 0)
		{
			return BeginIterator;
		}
	}
	
	return EndIterator;
}

String::CharIterator String::IndexOfAny(const Array<String>& Needles) const
{
	return IndexOfAny(Needles, BeginCharIterator());
}

String::CharIterator String::IndexOfAny(const Array<String>& Needles, const CharIterator& StartIndex) const
{
	String Result = "";
	if (Needles.IsEmpty())
	{
		return EndCharIterator();
	}

	CharIterator BeginIterator = StartIndex;
	CharIterator EndIterator = EndCharIterator();

	for (; BeginIterator != EndIterator; BeginIterator++)
	{
		int BytesRemaining = m_Buffer->Size - BeginIterator.m_StartByte;
		for (const String& Needle : Needles)
		{
			if (Needle.m_Buffer->Size > BytesRemaining)
			{
				continue;
			}

			if (memcmp(m_Buffer->Buffer + BeginIterator.m_StartByte, Needle.m_Buffer->Buffer, Needle.m_Buffer->Size) == 0)
		//	String SubSlice = Slice(BeginIterator, BeginIterator + Needle.CharLength());
		//	if (SubSlice == Needle)
			{
				return BeginIterator;
			}
		}
	}

	return EndCharIterator();
}

String::CharIterator String::LastIndexOf(const String& Needle) const
{
	return LastIndexOf(Needle, EndCharIterator());
}

String::CharIterator String::LastIndexOf(const String& Needle, const CharIterator& EndIndex) const
{
	String Result = "";
	if (Needle.IsEmpty() ||
		IsEmpty() ||
		CharLength() < Needle.CharLength())
	{
		return EndCharIterator();
	}

	// TODO: Assuming Needle is a valid utf8 string we should just be able to do a byte comparison.

	CharIterator BeginIterator = BeginCharIterator();
	CharIterator EndIterator = EndIndex - Needle.CharLength();

	while (true)
	{
	//	String SubSlice = Slice(EndIterator, EndIterator + Needle.CharLength());
	//	if (SubSlice == Needle)

		if (memcmp(m_Buffer->Buffer + EndIterator.m_StartByte, Needle.m_Buffer->Buffer, Needle.m_Buffer->Size) == 0)
		{
			return EndIterator;
		}

		if (BeginIterator == EndIterator)
		{
			break;
		}

		EndIterator--;
	}

	return EndCharIterator();
}

String::CharIterator String::LastIndexOfAny(const Array<String>& Needles) const
{
	return LastIndexOfAny(Needles, EndCharIterator());
}

String::CharIterator String::LastIndexOfAny(const Array<String>& Needles, const CharIterator& EndIndex) const
{
	String Result = "";
	if (Needles.IsEmpty() || IsEmpty())
	{
		return EndCharIterator();
	}

	CharIterator BeginIterator = BeginCharIterator();
	CharIterator EndIterator = EndIndex - 1;

	while (true)
	{
		int BytesRemaining = m_Buffer->Size - EndIterator.m_StartByte;

		for (const String& Needle : Needles)
		{
			if (BytesRemaining >= Needle.m_Buffer->Size)
			{
				if (memcmp(m_Buffer->Buffer + EndIterator.m_StartByte, Needle.m_Buffer->Buffer, Needle.m_Buffer->Size) == 0)
				//String SubSlice = Slice(EndIterator, EndIterator + Needle.CharLength());
				//if (SubSlice == Needle)
				{
					return EndIterator;
				}
			}
		}

		if (BeginIterator == EndIterator)
		{
			break;
		}

		EndIterator--;
	}

	return EndCharIterator();
}

bool String::StartsWith(const String& What) const
{
	if (m_Buffer->Size < What.m_Buffer->Size || What.m_Buffer->Size <= 0)
	{
		return false;
	}
	return (memcmp(m_Buffer->Buffer, What.m_Buffer->Buffer, What.m_Buffer->Size) == 0);
}

bool String::EndsWith(const String& What) const
{
	if (m_Buffer->Size < What.m_Buffer->Size || What.m_Buffer->Size <= 0)
	{
		return false;
	}
	return (memcmp(m_Buffer->Buffer + m_Buffer->Size - What.m_Buffer->Size, What.m_Buffer->Buffer, What.m_Buffer->Size) == 0);
}

// -----------------------------------------------------------------------------
// Splitting / Joining
// -----------------------------------------------------------------------------

Array<String> String::Split(const String& Seperator, const int MaxSplits, const bool RemoveDuplicates) const
{
	Array<String> Result;
	String Split = "";

	if (Seperator.IsEmpty())
	{
		Result.Add(*this);
		return Result;
	}

	CharIterator BeginIterator = BeginCharIterator();
	CharIterator EndIterator = EndCharIterator();

	for (; BeginIterator != EndIterator; BeginIterator++)
	{
		String SubSlice = Slice(BeginIterator, BeginIterator + 1);
		if (BeginIterator.m_CharIndex <= CharLength() - Seperator.CharLength())
		{
			SubSlice = Slice(BeginIterator, BeginIterator + Seperator.CharLength());
		}

		if (SubSlice == Seperator && ((int)Result.Length() < MaxSplits || MaxSplits <= 0))
		{
			if (Split != "" || RemoveDuplicates == false)
			{
				Result.Add(Split);
				Split = "";
			}
			BeginIterator += (Seperator.CharLength() - 1);
			continue;
		}
		else
		{
			Split += Slice(BeginIterator, BeginIterator + 1);
		}
	}

	if (Split != "" || RemoveDuplicates == false)
	{
		Result.Add(Split);
		Split = "";
	}

	return Result;
}

void String::SplitOnIndex(const CharIterator& Index, String& LeftOut, String& RightOut) const
{
	// These temporaries are intentional as its common to do things like this with this function:
	// Consumable.Split(10, Consumable, Consumed)

	CharIterator BeginIterator = BeginCharIterator();

	String LeftValue = Slice(BeginIterator, Index);
	String RightValue = Slice(Index + 1);

	LeftOut = LeftValue;
	RightOut = RightValue;
}

String String::Join(const Array<String>& Fragments) const
{
	String Result = "";
	for (const String& Fragment : Fragments)
	{
		if (Result != "")
		{
			Result += (*this);
		}
		Result += Fragment;
	}
	return Result;
}

String String::Filter(const String& AllowedChars, const String& Replacement) const
{
	String Result = "";

	CharIterator BeginIterator = BeginCharIterator();
	CharIterator EndIterator = EndCharIterator();

	for (; BeginIterator != EndIterator; BeginIterator++)
	{
		String Chr = *BeginIterator;
		bool Found = false;

		CharIterator CharsBeginIterator = AllowedChars.BeginCharIterator();
		CharIterator CharsEndIterator = AllowedChars.EndCharIterator();

		for (; CharsBeginIterator != CharsEndIterator; CharsBeginIterator++)
		{
			if (*CharsBeginIterator == Chr)
			{
				Found = true;
				break;
			}
		}

		if (Found == true)
		{
			Result += Chr;
		}
		else
		{
			Result += Replacement;
		}
	}

	return Result;
}

// -----------------------------------------------------------------------------
// State changing
// -----------------------------------------------------------------------------

String String::ToLower() const
{
	String Result = "";
	const char* Buffer = m_Buffer->Buffer;
	while (Buffer[0] != '\0')
	{
		Result += (char)::tolower(Buffer[0]);
		Buffer++;
	}
	return Result;
}

String String::ToUpper() const
{
	String Result = "";
	const char* Buffer = m_Buffer->Buffer;
	while (Buffer[0] != '\0')
	{
		Result += (char)::toupper(Buffer[0]);
		Buffer++;
	}
	return Result;
}

String String::Escape() const
{
	String Result = "";
	const char* Buffer = m_Buffer->Buffer;
	while (Buffer[0] != '\0')
	{
		if (Buffer[0] == '"' ||
			Buffer[0] == '\\')
		{
			Result += "\\";
		}
		Result += Buffer[0];
		Buffer++;
	}
	return Result;
}

String String::DoubleQuoted() const
{
	return String('"') + Escape() + String('"');
}

// -----------------------------------------------------------------------------
// General manipulation.
// -----------------------------------------------------------------------------

String String::Replace(const CharIterator& Start, const int Length, const String& Replacement) const
{
	CharIterator BeginIterator = BeginCharIterator();
	return Slice(BeginIterator, Start) + Replacement + Slice(Start + Length);
}

String String::Replace(const CharIterator& Start, const CharIterator& End, const String& Replacement) const
{
	CharIterator BeginIterator = BeginCharIterator();
	return Slice(BeginIterator, Start) + Replacement + Slice(End);
}

String String::Replace(const String& What, const String& With) const
{
	if (What.IsEmpty())
	{
		return *this;
	}

	String Result = *this;

	CharIterator RemoveIterator = Result.BeginCharIterator();
	CharIterator EndIterator = Result.EndCharIterator();

	for (; RemoveIterator != EndIterator; )
	{
		String SubSlice;
		if (RemoveIterator.m_CharIndex <= Result.CharLength() - What.CharLength())
		{
			SubSlice = Result.Slice(RemoveIterator, RemoveIterator + What.CharLength());
		}
		else
		{
			SubSlice = Result.Slice(RemoveIterator, RemoveIterator + 1);
		}

		if (SubSlice == What)
		{
			Result = Result.Replace(RemoveIterator, What.CharLength(), With);
			
			EndIterator = Result.EndCharIterator();
			RemoveIterator = Result.GetCharIterator(RemoveIterator.m_CharIndex);	
		}
		else
		{
			RemoveIterator++;
		}
	}

	return Result;
}

String String::Insert(const String& What, const CharIterator& Offset) const
{
	CharIterator BeginIterator = BeginCharIterator();

	return Slice(BeginIterator, Offset) + What + Slice(Offset);
}

String String::Remove(const CharIterator& Start, const int CharLength) const
{
	CharIterator BeginIterator = BeginCharIterator();

	return Slice(BeginIterator, Start) + Slice(Start + CharLength);
}

String String::Remove(const CharIterator& Start, const CharIterator& End) const
{
	CharIterator BeginIterator = BeginCharIterator();

	return Slice(BeginIterator, Start) + Slice(End);
}

String String::Reverse() const
{
	String Result = "";

	CharIterator EndIterator = EndCharIterator();
	CharIterator BeginIterator = BeginCharIterator();
	
	while (true)
	{
		EndIterator--;

		Result += *EndIterator;

		if (BeginIterator == EndIterator)
		{
			break;
		}
	}

	return Result;
}

String String::LimitEnd(const int CharLength, const String& Postfix) const
{
	if (CharLength <= Postfix.CharLength())
	{
		CharIterator BeginIterator = Postfix.BeginCharIterator();
		return Postfix.Slice(BeginIterator, BeginIterator + CharLength);
	}
	else
	{
		CharIterator BeginIterator = BeginCharIterator();
		return Slice(BeginIterator, BeginIterator + (CharLength - (Postfix.CharLength()))) + Postfix;
	}
}

String String::LimitStart(const int CharLength, const String& Postfix) const
{
	if (CharLength <= Postfix.CharLength())
	{
		CharIterator BeginIterator = Postfix.BeginCharIterator();
		return Postfix.Slice(BeginIterator, BeginIterator + CharLength);
	}
	else
	{
		CharIterator BeginIterator = BeginCharIterator();
		int CutLength = (this->CharLength() - CharLength) + Postfix.CharLength();
		return Postfix + Slice(BeginIterator + CutLength);
	}
}

String String::PadLeft(const int CharLength, const String& padding) const
{
	String Result = *this;

	CharIterator EndIterator = padding.EndCharIterator();
	CharIterator BeginIterator = padding.BeginCharIterator();
	CharIterator PaddingIterator = BeginIterator;

	for (int Offset = 0; Result.CharLength() < CharLength; Offset++)
	{
		Result = *PaddingIterator + Result;

		PaddingIterator++;
		if (PaddingIterator == EndIterator)
		{
			PaddingIterator = BeginIterator;
		}
	}

	return Result;
}

String String::PadRight(const int CharLength, const String& padding) const
{
	String Result = *this;

	CharIterator EndIterator = padding.EndCharIterator();
	CharIterator BeginIterator = padding.BeginCharIterator();
	CharIterator PaddingIterator = BeginIterator;

	for (int Offset = 0; Result.CharLength() < CharLength; Offset++)
	{
		Result += *PaddingIterator;

		PaddingIterator++;
		if (PaddingIterator == EndIterator)
		{
			PaddingIterator = BeginIterator;
		}
	}

	return Result;
}

String String::SubString(const CharIterator& Start, const int Count) const
{
	Assert(Start.ValidFor(*this));
	return Slice(Start, Start + Count);
}

String String::SubString(const CharIterator& Start, const CharIterator& End) const
{
	Assert(Start.ValidFor(*this));
	Assert(End.ValidFor(*this));
	return Slice(Start, End);
}

String String::Trim(const String& chars) const
{
	return TrimStart(chars).TrimEnd(chars);
}

String String::TrimStart(const String& chars) const
{
	CharIterator EndIterator = EndCharIterator();
	CharIterator BeginIterator = BeginCharIterator();

	while (EndIterator != BeginIterator)
	{
		bool bShouldTrim = false;

		CharIterator PaddingBeginIterator = chars.BeginCharIterator();
		CharIterator PaddingEndIterator = chars.EndCharIterator();

		for (; PaddingBeginIterator != PaddingEndIterator; PaddingBeginIterator++)
		{
			if (*PaddingBeginIterator == *BeginIterator)
			{
				bShouldTrim = true;
				break;
			}
		}

		if (bShouldTrim == false)
		{
			break;
		}

		BeginIterator++;
	}

	return Slice(BeginIterator, EndIterator);
}

String String::TrimEnd(const String& chars) const
{
	if (IsEmpty())
	{
		return "";
	}

	CharIterator EndIterator = EndCharIterator() - 1;
	CharIterator BeginIterator = BeginCharIterator();

	while (true)
	{
		bool bShouldTrim = false;

		CharIterator PaddingBeginIterator = chars.BeginCharIterator();
		CharIterator PaddingEndIterator = chars.EndCharIterator();

		for (; PaddingBeginIterator != PaddingEndIterator; PaddingBeginIterator++)
		{
			if (*PaddingBeginIterator == *EndIterator)
			{
				bShouldTrim = true;
				break;
			}
		}

		if (bShouldTrim == false)
		{
			break;
		}

		if (EndIterator == BeginIterator)
		{
			break;
		}

		EndIterator--;
	}

	return Slice(BeginIterator, EndIterator + 1);
}

String String::Slice(const CharIterator& Start) const
{
	return Slice(Start, EndCharIterator());
}

String String::Slice(const CharIterator& Start, const CharIterator& End) const
{
	Assert(Start.ValidFor(*this));
	Assert(End.ValidFor(*this));
	Assert(Start <= End);

	int StartByte = Start.m_StartByte;
	if (StartByte < 0)
	{
		StartByte = 0;
	}

	int EndByte = End.m_StartByte;
	if (EndByte > m_Buffer->Size)
	{
		EndByte = m_Buffer->Size;
	}

	int TotalBytes = EndByte - StartByte;

	MemoryBuffer* Buffer = MemoryBuffer::Allocate(TotalBytes);
	Buffer->Iteration++;
	Buffer->CharSize = End.m_CharIndex - Start.m_CharIndex;
	Buffer->Size = TotalBytes;
	memcpy(Buffer->Buffer, m_Buffer->Buffer + StartByte, TotalBytes);

	return String(Buffer);
}

}; // namespace Ludo