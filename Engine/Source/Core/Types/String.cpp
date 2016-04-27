// Copyright 2016 TwinDrills

#include "Core/Types/String.h"
#include "Core/Types/StringFormatters/SimpleStringArgumentFormatter.h"

#include <sstream>
#include <cstring>
#include <stdio.h>
#include <cstdlib>

namespace Ludo {

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
	ArenaAllocator<StringMemoryArena> m_Allocator;

	GlobalStrings()
	{
		m_EmptyBuffer = m_Allocator.New<String::MemoryBuffer>();
		m_EmptyBuffer->Buffer = m_Allocator.NewArray<char>(1);
		m_EmptyBuffer->Buffer[0] = '\0';
		m_EmptyBuffer->Size = 0;
		m_EmptyBuffer->RefCount = 1;
		m_EmptyBuffer->Persistent = true;
	}
};

GlobalStrings gGlobalStrings;

ArenaAllocator<StringMemoryArena> String::MemoryBuffer::m_Allocator;

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
		//	delete[] Buffer;
		//	delete this;

			m_Allocator.DeleteArray(Buffer);
			m_Allocator.Delete(this);

			Buffer = nullptr;
		}
	}
}

String::MemoryBuffer* String::MemoryBuffer::Allocate(int Size)
{
	if (Size == 0)
	{
		return gGlobalStrings.m_EmptyBuffer;
	}

	MemoryBuffer* Buffer = m_Allocator.New<String::MemoryBuffer>();
	Buffer->Buffer = m_Allocator.NewArray<char>(Size + 1);
	Buffer->Buffer[Size] = '\0';
	Buffer->Size = Size;
	Buffer->RefCount = 1;

	return Buffer;
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

String::String(const char Value, const int Size)
{
	m_Buffer = MemoryBuffer::Allocate(Size);
	memset(m_Buffer->Buffer, Value, Size);
}

String::String(const int Value)
{
	char Buffer[128];
	sprintf(Buffer, "%i", Value);

	int Length = (int)strlen(Buffer);
	m_Buffer = MemoryBuffer::Allocate(Length);
	memcpy(m_Buffer->Buffer, Buffer, Length);
}

String::String(const float Value)
{
	char Buffer[128];
	sprintf(Buffer, "%f", Value);

	int Length = (int)strlen(Buffer);
	m_Buffer = MemoryBuffer::Allocate(Length);
	memcpy(m_Buffer->Buffer, Buffer, Length);
}

String::String(const char* Value)
{
	int Length = (int)strlen(Value);
	m_Buffer = MemoryBuffer::Allocate(Length);
	memcpy(m_Buffer->Buffer, Value, Length);
}

String::String(const char* Value, const int Length)
{
	m_Buffer = MemoryBuffer::Allocate(Length);
	memcpy(m_Buffer->Buffer, Value, Length);

	int realLen = (int)strlen(m_Buffer->Buffer);
	Assert(realLen == m_Buffer->Size);
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

String::String(const char Value)
{
	m_Buffer = MemoryBuffer::Allocate(1);
	m_Buffer->Buffer[0] = Value;
	m_Buffer->Buffer[1] = '\0';
}

String::String(const Array<char>& Value)
{
	m_Buffer = MemoryBuffer::Allocate(Value.Length());
	for (int i = 0; i < Value.Length(); i++)
	{
		m_Buffer->Buffer[i] = Value[i];
	}
	m_Buffer->Buffer[Value.Length()] = '\0';
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
		int Diff = (Other.m_Buffer->Buffer[i] - m_Buffer->Buffer[i]);
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

char String::operator [](const int Index) const
{
	Assert(Index >= 0 && Index < m_Buffer->Size);
	return m_Buffer->Buffer[Index];
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
// Properties
// -----------------------------------------------------------------------------
int String::Length() const
{
	return m_Buffer->Size;
}

const char* String::Data() const
{
	return m_Buffer->Buffer;
}

// -----------------------------------------------------------------------------
// Conversion / Formatting
// -----------------------------------------------------------------------------
int String::ToInt() const
{
	return atoi(m_Buffer->Buffer);
}

bool String::ToBool() const
{
	return m_Buffer->Size == 0 || !(strcmp(m_Buffer->Buffer, "false") == 0 || strcmp(m_Buffer->Buffer, "0") == 0);
}

float String::ToFloat() const
{
	return (float)atof(m_Buffer->Buffer);
}

bool String::IsEmpty() const
{
	return (Length() == 0);
}

bool String::IsNumeric() const
{
	for (int i = 0; i < Length(); i++)
	{
		char Chr = m_Buffer->Buffer[i];
		bool IsNumber = (Chr >= '0' && Chr <= '9');
		if (!IsNumber)
		{
			return false;
		}
	}
	return true;
}

bool String::IsHex() const
{
	for (int i = 0; i < Length(); i++)
	{
		char Chr = m_Buffer->Buffer[i];
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

String String::Format(const StringArgumentList& Arguments) const
{
	// No arguments, just return the format string verbatim.
	if (Arguments.Arguments.Length() == 0)
	{
		return *this;
	}

	SimpleStringArgumentFormatter DefaultFormatter;

	String Result = *this;

	bool bInTag = false;
	int TagStart = 0;

	for (int i = 0; i < Result.Length(); i++)
	{
		char chr = Result[i];
		if (chr == '{')
		{
			bool bIsEscaped = false;

			if (i + 1 < Result.Length())
			{
				char la_char = Result[i + 1];
				if (la_char == '{')
				{
					bIsEscaped = true;
				}
			}

			if (bIsEscaped)
			{
				// Remove one of the braces.
				Result.Remove(i, 1);
				i--;
			}
			else
			{
				bInTag = true;
				TagStart = i;
			}
		}
		else if (chr == '}')
		{
			if (bInTag)
			{
				String Tag = Result.Slice(TagStart + 1, i);
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
					String Left = TagStart == 0 ? "" : Result.Slice(0, TagStart);
					String Right = Result.Slice(i + 1);
					Result = Left + Replacement + Right;

					i = Left.Length() + Replacement.Length() - 1;
				}

				bInTag = false;
			}
		}
	}

	return Result;
}

String String::Format(const String& Format, const StringArgumentList& Arguments)
{
	return Format.Format(Arguments);
}

// -----------------------------------------------------------------------------
// Searching
// -----------------------------------------------------------------------------

bool String::Contains(const String& Needle) const
{
	return (IndexOf(Needle, NoIndex) >= 0);
}

bool String::ContainsAny(const Array<String>& Needles) const
{
	return (IndexOfAny(Needles, NoIndex) >= 0);
}

int String::IndexOf(const String& Needle, const int StartIndex) const
{
	String Result = "";
	if (Needle == "")
	{
		return NotFound;
	}

	int RealStartIndex = StartIndex;

	if (RealStartIndex == NoIndex)
	{
		RealStartIndex = 0;
	}

	for (int i = RealStartIndex; i < (Length() - Needle.Length()) + 1; i++)
	{
		String SubSlice = Slice(i, i + Needle.Length());
		if (SubSlice == Needle)
		{
			return i;
		}
	}

	return NotFound;
}

int String::IndexOfAny(const Array<String>& Needles, const int StartIndex) const
{
	String Result = "";
	if (Needles.Length() == 0)
	{
		return NotFound;
	}

	int RealStartIndex = StartIndex;

	if (RealStartIndex == NoIndex)
	{
		RealStartIndex = 0;
	}

	for (int i = RealStartIndex; i < Length(); i++)
	{
		for (const String& Needle : Needles)
		{
			String SubSlice = Slice(i, i + Needle.Length());
			if (SubSlice == Needle)
			{
				return i;
			}
		}
	}

	return NotFound;
}

int String::LastIndexOf(const String& Needle, const int EndIndex) const
{
	String Result = "";
	if (Needle == "")
	{
		return NotFound;
	}

	int RealEndIndex = EndIndex;

	if (RealEndIndex == NoIndex)
	{
		RealEndIndex = Length() - 1;
	}

	for (int i = RealEndIndex; i >= 0; i--)
	{
		String SubSlice = Slice(i, i + Needle.Length());
		if (SubSlice == Needle)
		{
			return i;
		}
	}

	return NotFound;
}

int String::LastIndexOfAny(const Array<String>& Needles, const int EndIndex) const
{
	String Result = "";
	if (Needles.Length() == 0)
	{
		return NotFound;
	}

	int RealEndIndex = EndIndex;

	if (RealEndIndex == NoIndex)
	{
		RealEndIndex = Length() - 1;
	}

	for (int i = RealEndIndex; i >= 0; i--)
	{
		for (const String& Needle : Needles)
		{
			String SubSlice = Slice(i, i + Needle.Length());

			if (SubSlice == Needle)
			{
				return i;
			}
		}
	}

	return NotFound;
}

bool String::StartsWith(const String& What) const
{
	return (Slice(0, What.Length()) == What);
}

bool String::EndsWith(const String& What) const
{
	return (Slice(-What.Length()) == What);
}

// -----------------------------------------------------------------------------
// Splitting / Joining
// -----------------------------------------------------------------------------

Array<String> String::Split(const String& Seperator, const int MaxSplits, const bool RemoveDuplicates) const
{
	Array<String> Result;
	String Split = "";

	if (Seperator == "")
	{
		Result.Add(*this);
		return Result;
	}

	for (int i = 0; i < Length(); i++)
	{
		String SubSlice = Slice(i, i + 1);
		if (i <= Length() - Seperator.Length())
		{
			SubSlice = Slice(i, i + Seperator.Length());
		}

		if (SubSlice == Seperator && ((int)Result.Length() < MaxSplits || MaxSplits <= 0))
		{
			if (Split != "" || RemoveDuplicates == false)
			{
				Result.Add(Split);
				Split = "";
			}
			i += (Seperator.Length() - 1);
			continue;
		}
		else
		{
			Split += Slice(i, i + 1);
		}
	}

	if (Split != "" || RemoveDuplicates == false)
	{
		Result.Add(Split);
		Split = "";
	}

	return Result;
}

void String::SplitOnIndex(int Index, String& LeftOut, String& RightOut) const
{
	// These temporaries are intentional as its common to do things like this with this function:
	// Consumable.Split(10, Consumable, Consumed)

	String LeftValue = SubString(0, Index);
	String RightValue = SubString(Index + 1);

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

	for (int i = 0; i < Length(); i++)
	{
		char Chr = (*this)[i];
		bool Found = false;

		for (int j = 0; j < AllowedChars.Length(); j++)
		{
			char AllowedChr = AllowedChars[j];
			if (Chr == AllowedChr)
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

// -----------------------------------------------------------------------------
// General manipulation.
// -----------------------------------------------------------------------------

String String::Replace(const int StartIndex, const int Length, const String& Replacement) const
{
	return Slice(0, StartIndex) + Replacement + Slice(StartIndex + Length);
}

String String::Replace(const String& What, const String& With) const
{
	if (What == "")
	{
		return *this;
	}

	String Result = *this;

	for (int i = 0; i < Result.Length(); i++)
	{
		String SubSlice = Result.Slice(i, i + 1);
		if (i <= Result.Length() - What.Length())
		{
			SubSlice = Result.Slice(i, i + What.Length());
		}

		if (SubSlice == What)
		{
			Result = Result.Replace(i, What.Length(), With);
			i--;
		}
	}

	return Result;
}

String String::Insert(const String& What, const int Offset) const
{
	return Slice(0, Offset) + What + Slice(Offset);
}

String String::Remove(const int Start, const int Length) const
{
	return Slice(0, Start) + Slice(Start + Length);
}

String String::Reverse() const
{
	String Result = "";
	const char* Buffer = m_Buffer->Buffer + m_Buffer->Size;
	while (Buffer >= m_Buffer->Buffer + 1)
	{
		Result += Buffer[-1];
		Buffer--;
	}
	return Result;
}

String String::LimitEnd(const int Length, const String& Postfix) const
{
	if (Length <= Postfix.Length())
	{
		return Postfix.Slice(0, Length);
	}
	else
	{
		return Slice(0, Length - Postfix.Length()) + Postfix;
	}
}

String String::LimitStart(const int Length, const String& Postfix) const
{
	if (Length <= Postfix.Length())
	{
		return Postfix.Slice(0, Length);
	}
	else
	{
		int CutLength = (this->Length() - Length) + Postfix.Length();
		return Postfix + Slice(CutLength);
	}
}

String String::PadLeft(const int Length, const String& padding) const
{
	String Result = *this;

	for (int Offset = 0; Result.Length() < Length; Offset++)
	{
		Result = String(padding[(Offset++) % padding.Length()]) + Result;
	}

	return Result;
}

String String::PadRight(const int Length, const String& padding) const
{
	String Result = *this;

	for (int Offset = 0; Result.Length() < Length; Offset++)
	{
		Result += padding[(Offset++) % padding.Length()];
	}

	return Result;
}

String String::SubString(const int Offset, const int Count) const
{
	Assert(Offset >= 0);

	int RealCount = Count;

	if (RealCount == Infinite)
	{
		RealCount = Length();
	}

	int Excess = Math::Min(0, Length() - (Offset + Count));
	return String(m_Buffer->Buffer + Offset, Count + Excess);
}

String String::Trim(const String& chars) const
{
	return TrimStart(chars).TrimEnd(chars);
}

String String::TrimStart(const String& chars) const
{
	char* ResultBuffer = m_Buffer->Buffer;
	int ResultLength = Length();

	while (ResultLength > 0)
	{
		bool bShouldTrim = false;

		for (int i = 0; i < chars.Length(); i++)
		{
			if (chars[i] == ResultBuffer[0])
			{
				bShouldTrim = true;
				break;
			}
		}

		if (bShouldTrim == false)
		{
			break;
		}

		ResultBuffer++;
		ResultLength--;
	}

	return String(ResultBuffer, ResultLength);
}

String String::TrimEnd(const String& chars) const
{
	int ResultLength = Length();
	char* ResultBuffer = m_Buffer->Buffer + ResultLength;

	while (ResultLength > 0)
	{
		bool bShouldTrim = false;

		for (int i = 0; i < chars.Length(); i++)
		{
			if (chars[i] == ResultBuffer[-1])
			{
				bShouldTrim = true;
				break;
			}
		}

		if (bShouldTrim == false)
		{
			break;
		}

		ResultBuffer--;
		ResultLength--;
	}

	return String(m_Buffer->Buffer, ResultLength);
}

String String::Slice(const int StartIndex) const
{
	return Slice(StartIndex, Length());
}

String String::Slice(const int StartIndex, const int EndIndex) const
{
	int Length = this->Length();

	int FinalStartIndex = StartIndex;
	int FinalEndIndex = EndIndex;

	if (FinalStartIndex < 0)
	{
		FinalStartIndex += Length;
		if (FinalStartIndex < 0)
		{
			FinalStartIndex = 0;
		}
	}
	else if (FinalStartIndex > Length)
	{
		FinalStartIndex = Length;
	}

	if (FinalEndIndex < 0)
	{
		FinalEndIndex += Length;
	}
	else if (FinalEndIndex > Length)
	{
		FinalEndIndex = Length;
	}

	if (FinalStartIndex > FinalEndIndex)
	{
		return "";
	}
	else if (FinalStartIndex == 0 && FinalEndIndex == Length)
	{
		return *this;
	}
	else
	{
		return String(m_Buffer->Buffer + FinalStartIndex, (FinalEndIndex - FinalStartIndex));
	}
}

}; // namespace Ludo