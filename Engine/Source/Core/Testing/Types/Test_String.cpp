// Copyright 2016 TwinDrills
#pragma once

#include "Core/Testing/Types/Test_String.h"
#include "Core/Containers/Array.h"
#include "Core/Types/String.h"

namespace Ludo {

void TestString::Run() const
{
	String PaddedConstructor('*', 5);
	Assert(PaddedConstructor == "*****");

	String IntConstructor(61231);
	Assert(IntConstructor == "61231");

	String FloatConstructor(0.1f);
	Assert(FloatConstructor.SubString(0, 3) == "0.1");

	String ConstCharConstructor("Test");
	Assert(ConstCharConstructor == "Test");

	String ConstCharWithLengthConstructor("Test", 2);
	Assert(ConstCharWithLengthConstructor == "Te");

	String CopyConstructor(ConstCharWithLengthConstructor);
	Assert(CopyConstructor == "Te");

	String CharConstructor('T');
	Assert(CharConstructor == "T");

	Array<char> CharArray;
	CharArray.Add('T');
	CharArray.Add('e');
	CharArray.Add('s');
	CharArray.Add('t');
	String CharArrayConstructor(CharArray);
	Assert(CharArrayConstructor == "Test");

	Assert(CharConstructor.Compare(ConstCharConstructor) != 0);
	Assert(CharConstructor.Compare(CharConstructor) == 0);

	String AssignmentOperator = ConstCharConstructor;
	Assert(AssignmentOperator == "Test");

	String AdditionOperator = ConstCharConstructor + PaddedConstructor;
	Assert(AdditionOperator == "Test*****");

	String AdditionConstCharOperator = ConstCharConstructor + "12345";
	Assert(AdditionConstCharOperator == "Test12345");

	Assert(ConstCharConstructor[0] == 'T');
	Assert(ConstCharConstructor[1] == 'e');
	Assert(ConstCharConstructor[2] == 's');
	Assert(ConstCharConstructor[3] == 't');

	Assert(ConstCharConstructor.Length() == 4);
	Assert(IntConstructor.ToInt() == 61231);
	Assert(IntConstructor.ToBool() == true);
	Assert(IntConstructor.ToFloat() == 61231.0f);
	Assert(IntConstructor.IsNumeric());
	Assert(IntConstructor.IsHex());

	String EmptyValue = "";
	Assert(EmptyValue.IsEmpty());

	String HexString = String::HexToString(31);
	Assert(HexString == "1f");
	Assert(String("1F").IntFromHex() == 31);

	StringArgumentList FormatArgs;
	FormatArgs.Add("Val1", 123);
	FormatArgs.Add("Val2", 321);
	FormatArgs.Add("Val3", 987);
	FormatArgs.Add("Val4", "Test");
	String FormatResult = String::Format("{Val1}{Val2} = {Val3}{Val4}", FormatArgs);
	Assert(FormatResult == "123321 = 987Test");

	Array<String> Needles;
	Needles.Add("Val");
	Needles.Add("est");

	Assert(String("Test").Contains("es"));
	Assert(String("Test").ContainsAny(Needles));

	Assert(String("Test").IndexOf("es") == 1);
	Assert(String("Test").IndexOfAny(Needles) == 1);
	Assert(String("TestTest").LastIndexOf("es") == 5);
	Assert(String("TestTest").LastIndexOfAny(Needles) == 5);

	Assert(String("TestTest").StartsWith("TestTe"));
	Assert(String("TestTest").EndsWith("stTest"));

	Array<String> SplitValues = String("Val1,Val2,Val3,Val4").Split(",", 0, false);
	Assert(SplitValues[0] == "Val1");
	Assert(SplitValues[1] == "Val2");
	Assert(SplitValues[2] == "Val3");
	Assert(SplitValues[3] == "Val4");

	SplitValues = (String("Val1,Val2,Val3,Val4").Split(",", 1, false));
	Assert(SplitValues[0] == "Val1");
	Assert(SplitValues[1] == "Val2,Val3,Val4");

	SplitValues = (String("Val1,,,Val4").Split(",", 0, true));
	Assert(SplitValues[0] == "Val1");
	Assert(SplitValues[1] == "Val4");

	Assert(String(".").Join(SplitValues) == "Val1.Val4");

	Assert(String("123ABC456").Filter("ABC", "") == "ABC");

	Assert(String("abc").ToUpper() == "ABC");
	Assert(String("ABC").ToLower() == "abc");

	Assert(String("Test").Replace(1, 2, "44") == "T44t");
	Assert(String("Test").Replace("es", "44") == "T44t");
	Assert(String("Tt").Insert("es", 1) == "Test");
	Assert(String("Test").Remove(1, 2) == "Tt");
	Assert(String("Test").Reverse() == "tseT");
	Assert(String("TestTest").LimitEnd(5, "...") == "Te...");
	Assert(String("TestTest").LimitStart(5, "...") == "...st");
	Assert(String("Test").PadLeft(8, "*") == "****Test");
	Assert(String("Test").PadRight(8, "*") == "Test****");
	Assert(String("Test").SubString(1, 2) == "es");
	Assert(String("Test").Trim("T") == "est");
	Assert(String("Test").TrimStart("T") == "est");
	Assert(String("Test").TrimEnd("t") == "Tes");

	Assert(String("Test").Slice(1) == "est");
	Assert(String("Test").Slice(1, 2) == "e");
}

}; // namespace Ludo
