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

#include "PCH.h"

// TODO: More unicode tests.
// TODO: Grapheme iterator tests.
// TODO: Need to look into things like ligatures/text-layout/etc, but thats going to likely be in higher level tests than this.

using namespace Ludo;

// ************************************************************************************************

TEST(StringTests, Constructors) 
{
	String PaddedConstructor('*', 5);
	EXPECT_TRUE(PaddedConstructor == "*****");

	String IntConstructor(61231);
	EXPECT_TRUE(IntConstructor == "61231");

	String FloatConstructor(0.1f);
	EXPECT_TRUE(FloatConstructor.SubString(FloatConstructor.BeginCharIterator(), 3) == "0.1");

	String ConstCharConstructor("Test");
	EXPECT_TRUE(ConstCharConstructor == "Test");

	String ConstCharWithLengthConstructor("Test", 2);
	EXPECT_TRUE(ConstCharWithLengthConstructor == "Te");

	String CopyConstructor(ConstCharWithLengthConstructor);
	EXPECT_TRUE(CopyConstructor == "Te");

	String CharConstructor('T');
	EXPECT_TRUE(CharConstructor == "T");

	Array<char> CharArray;
	CharArray.Add('T');
	CharArray.Add('e');
	CharArray.Add('s');
	CharArray.Add('t');

	String CharArrayConstructor(CharArray);
	EXPECT_TRUE(CharArrayConstructor == "Test");
}

// ************************************************************************************************

TEST(StringTests, Comparison)
{
	String Value1 = "A";
	String Value2 = "B";

	EXPECT_TRUE(Value1.Compare(Value2) < 0);
	EXPECT_TRUE(Value2.Compare(Value1) > 0);
	EXPECT_TRUE(Value1.Compare(Value1) == 0);

	EXPECT_TRUE(Value1 != Value2);
	EXPECT_TRUE(Value1 == Value1);
}

// ************************************************************************************************

TEST(StringTests, Assignment)
{
	String Value1 = "A";
	String Value2 = "B";

	Value1 = Value2;
	EXPECT_TRUE(Value1 == "B");

	Value1 = "C";
	EXPECT_TRUE(Value1 == "C");
}

// ************************************************************************************************

TEST(StringTests, Concatination)
{
	String Value1 = "A";
	String Value2 = "B";

	String Value3 = Value1 + Value2;
	EXPECT_TRUE(Value3 == "AB");

	String Value4 = Value1 + "C";
	EXPECT_TRUE(Value4 == "AC");
}

// ************************************************************************************************

TEST(StringTests, SubScripting)
{
	String Value = "Test";

	EXPECT_TRUE(Value[0] == 'T');
	EXPECT_TRUE(Value[1] == 'e');
	EXPECT_TRUE(Value[2] == 's');
	EXPECT_TRUE(Value[3] == 't');
}

// ************************************************************************************************

TEST(StringTests, Length)
{
	String Value1 = "Test";
	EXPECT_TRUE(Value1.ByteLength() == 4);
	EXPECT_TRUE(Value1.CharLength() == 4);

	unsigned char UnicodeValue[4];
	UnicodeValue[0] = 0xF0;
	UnicodeValue[1] = 0x9F;
	UnicodeValue[2] = 0x92;
	UnicodeValue[3] = 0xA9;

	EXPECT_TRUE(String((char*)UnicodeValue, 4).CharLength() == 1);

	String EmptyValue = "";
	EXPECT_TRUE(EmptyValue.IsEmpty());
}

// ************************************************************************************************

TEST(StringTests, Conversion)
{
	String Value = "12345";

	EXPECT_TRUE(Value.ToInt() == 12345);
	EXPECT_TRUE(Value.ToBool() == true);
	EXPECT_TRUE(Value.ToFloat() == 12345.0f);
	EXPECT_TRUE(Value.IsNumeric());
	EXPECT_TRUE(Value.IsHex());

	String FalseValue = "False";
	EXPECT_TRUE(FalseValue.ToBool() == false);

	String TrueValue = "True";
	EXPECT_TRUE(TrueValue.ToBool() == true);

	String HexString = String::HexToString(31);
	EXPECT_TRUE(HexString == "1f");
	EXPECT_TRUE(String("1F").IntFromHex() == 31);
}

// ************************************************************************************************

TEST(StringTests, Formatting)
{
	StringArgumentList Args;
	Args.Add("Val1", 123);
	Args.Add("Val2", 321);
	Args.Add("Val3", 987);
	Args.Add("Val4", "Test");

	String FormatResult = String::FormatArgs("{Val1}{Val2} = {Val3}{Val4}", Args);
	EXPECT_TRUE(FormatResult == "123321 = 987Test");
}

// ************************************************************************************************

TEST(StringTests, Contains)
{
	Array<String> Needles;
	Needles.Add("Val");
	Needles.Add("est");

	EXPECT_TRUE(String("Test").Contains("es"));
	EXPECT_TRUE(String("Test").ContainsAny(Needles));
}

// ************************************************************************************************

TEST(StringTests, IndexOf)
{
	Array<String> Needles;
	Needles.Add("Val");
	Needles.Add("est");

	String Value1 = "Test";
	String Value2 = "TestTest";

	EXPECT_TRUE(Value1.IndexOf("es").CharIndex() == 1);
	EXPECT_TRUE(Value1.IndexOfAny(Needles).CharIndex() == 1);
	EXPECT_TRUE(Value2.LastIndexOf("es").CharIndex() == 5);
	EXPECT_TRUE(Value2.LastIndexOfAny(Needles).CharIndex() == 5);
}

// ************************************************************************************************

TEST(StringTests, StartsWith)
{
	String Value1 = "TestTest";

	EXPECT_TRUE(Value1.StartsWith("TestTe"));
	EXPECT_TRUE(Value1.EndsWith("stTest"));
}

// ************************************************************************************************

TEST(StringTests, Split)
{
	String Value1 = "Val1,Val2,Val3,Val4";
	Array<String> SplitValues = Value1.Split(",", 0, false);

	EXPECT_TRUE(SplitValues[0] == "Val1");
	EXPECT_TRUE(SplitValues[1] == "Val2");
	EXPECT_TRUE(SplitValues[2] == "Val3");
	EXPECT_TRUE(SplitValues[3] == "Val4");

	SplitValues = (Value1.Split(",", 1, false));
	EXPECT_TRUE(SplitValues[0] == "Val1");
	EXPECT_TRUE(SplitValues[1] == "Val2,Val3,Val4");

	String Value2 = "Val1,,,Val4";
	SplitValues = (Value2.Split(",", 0, true));
	EXPECT_TRUE(SplitValues[0] == "Val1");
	EXPECT_TRUE(SplitValues[1] == "Val4");
}

// ************************************************************************************************

TEST(StringTests, Join)
{
	Array<String> Needles;
	Needles.Add("Val1");
	Needles.Add("Val4");

	String Glue = String(".");

	EXPECT_TRUE(Glue.Join(Needles) == "Val1.Val4");
}

// ************************************************************************************************

TEST(StringTests, Filter)
{
	String Value1 = "123ABC456";
	EXPECT_TRUE(Value1.Filter("ABC", "") == "ABC");
}

// ************************************************************************************************

TEST(StringTests, CaseShift)
{
	EXPECT_TRUE(String("abc").ToUpper() == "ABC");
	EXPECT_TRUE(String("ABC").ToLower() == "abc");
}

// ************************************************************************************************

TEST(StringTests, Insertion)
{
	String Value1 = "Tt";
	String Value2 = Value1.Insert("es", Value1.GetCharIterator(1));
	EXPECT_TRUE(Value2 == "Test");
}

// ************************************************************************************************

TEST(StringTests, Replace)
{
	String Value1 = "Test";

	EXPECT_TRUE(Value1.Replace(Value1.GetCharIterator(1), 2, "44") == "T44t");
	EXPECT_TRUE(Value1.Replace("es", "44") == "T44t");
}

// ************************************************************************************************

TEST(StringTests, Remove)
{
	String Value1 = "Test";

	EXPECT_TRUE(Value1.Remove(Value1.GetCharIterator(1), 2) == "Tt");
}

// ************************************************************************************************

TEST(StringTests, Reverse)
{
	String Value1 = "Test";

	EXPECT_TRUE(Value1.Reverse() == "tseT");
}

// ************************************************************************************************

TEST(StringTests, Limit)
{
	String Value1 = "TestTest";

	EXPECT_TRUE(Value1.LimitEnd(5, "...") == "Te...");
	EXPECT_TRUE(Value1.LimitStart(5, "...") == "...st");
}

// ************************************************************************************************

TEST(StringTests, Padding)
{
	String Value1 = "Test";

	EXPECT_TRUE(Value1.PadLeft(8, "*") == "****Test");
	EXPECT_TRUE(Value1.PadRight(8, "*") == "Test****");
}

// ************************************************************************************************

TEST(StringTests, SubString)
{
	String Value1 = "Test";

	EXPECT_TRUE(Value1.SubString(Value1.GetCharIterator(1), 2) == "es");
}

// ************************************************************************************************

TEST(StringTests, Trim)
{
	String Value1 = "Test";

	EXPECT_TRUE(Value1.Trim("T") == "est");
	EXPECT_TRUE(Value1.TrimStart("T") == "est");
	EXPECT_TRUE(Value1.TrimEnd("t") == "Tes");
}

// ************************************************************************************************

TEST(StringTests, Slice)
{
	String Value1 = "Test";

	EXPECT_TRUE(Value1.Slice(Value1.GetCharIterator(1)) == "est");
	EXPECT_TRUE(Value1.Slice(Value1.GetCharIterator(1), Value1.GetCharIterator(2)) == "e");
}

// ************************************************************************************************
