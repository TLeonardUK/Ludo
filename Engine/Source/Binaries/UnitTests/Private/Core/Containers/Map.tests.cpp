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

using namespace Ludo;

TEST(MapTests, Constructors)
{
	Map<int, int> Value1;
	Value1.Set(1, 2);

	Map<int, int> Value2(Value1);
	EXPECT_TRUE(Value1.Count() == Value2.Count());

	for (Map<int, int>::PairType Pair : Value2)
	{
		EXPECT_TRUE(Pair.Key() == 1);
		EXPECT_TRUE(Pair.Value() == 2);
	}
}

TEST(MapTests, KeyOverwrite)
{
	Map<int, int> Value1;
	Value1.Set(1, 2);
	Value1.Set(1, 3);
	Value1.Set(1, 4);

	EXPECT_TRUE(Value1.Count() == 1);

	for (Map<int, int>::PairType Pair : Value1)
	{
		EXPECT_TRUE(Pair.Key() == 1);
		EXPECT_TRUE(Pair.Value() == 4);
	}
}

TEST(MapTests, GetKeys)
{
	Map<int, int> Value1;
	Value1.Set(1, 4);
	Value1.Set(2, 5);
	Value1.Set(3, 6);

	Array<int> Keys = Value1.GetKeys();

	EXPECT_TRUE(Keys.Length() == 3);
	EXPECT_TRUE(Keys.Contains(1));
	EXPECT_TRUE(Keys.Contains(2));
	EXPECT_TRUE(Keys.Contains(3));
}

TEST(MapTests, GetValues)
{
	Map<int, int> Value1;
	Value1.Set(1, 4);
	Value1.Set(2, 5);
	Value1.Set(3, 6);

	Array<int> Values = Value1.GetValues();

	EXPECT_TRUE(Values.Length() == 3);
	EXPECT_TRUE(Values.Contains(4));
	EXPECT_TRUE(Values.Contains(5));
	EXPECT_TRUE(Values.Contains(6));
}

TEST(MapTests, GetPairs)
{
	Map<int, int> Value1;
	Value1.Set(1, 4);
	Value1.Set(2, 5);
	Value1.Set(3, 6);

	Array<Map<int,int>::PairType> Pairs = Value1.GetPairs();

	EXPECT_TRUE(Pairs.Length() == 3);
	EXPECT_TRUE(Pairs.Contains(Map<int,int>::PairType(1, 4)));
	EXPECT_TRUE(Pairs.Contains(Map<int, int>::PairType(2, 5)));
	EXPECT_TRUE(Pairs.Contains(Map<int, int>::PairType(3, 6)));
}

TEST(MapTests, FindKey)
{
	Map<int, int> Value1;
	Value1.Set(1, 4);
	Value1.Set(2, 5);
	Value1.Set(3, 6);

	int Key = -1;
	EXPECT_TRUE(Value1.FindKey(4, Key));
	EXPECT_TRUE(Key == 1);
	EXPECT_TRUE(Value1.FindKey(5, Key));
	EXPECT_TRUE(Key == 2);
	EXPECT_TRUE(Value1.FindKey(6, Key));
	EXPECT_TRUE(Key == 3);
}

TEST(MapTests, FindValue)
{
	Map<int, int> Value1;
	Value1.Set(1, 4);
	Value1.Set(2, 5);
	Value1.Set(3, 6);

	int Value = -1;
	EXPECT_TRUE(Value1.FindValue(1, Value));
	EXPECT_TRUE(Value == 4);
	EXPECT_TRUE(Value1.FindValue(2, Value));
	EXPECT_TRUE(Value == 5);
	EXPECT_TRUE(Value1.FindValue(3, Value));
	EXPECT_TRUE(Value == 6);
}

TEST(MapTests, ForEach)
{
	Map<int, int> Value1;
	Value1.Set(1, 4);
	Value1.Set(2, 5);
	Value1.Set(3, 6);

	int ValueCounter = 0;
	int KeyCounter = 0;
	Value1.ForEach([&ValueCounter, &KeyCounter] (const Map<int,int>::PairType& Pair) {
		ValueCounter += Pair.Value();		
		KeyCounter += Pair.Key();
	});

	EXPECT_TRUE(KeyCounter == 6);
	EXPECT_TRUE(ValueCounter == 15);
}

TEST(MapTests, ContainsKey)
{
	Map<int, int> Value1;
	Value1.Set(1, 4);
	Value1.Set(2, 5);
	Value1.Set(3, 6);

	EXPECT_TRUE( Value1.ContainsKey(1));
	EXPECT_TRUE( Value1.ContainsKey(2));
	EXPECT_TRUE( Value1.ContainsKey(3));
	EXPECT_TRUE(!Value1.ContainsKey(4));
}

TEST(MapTests, ContainsValue)
{
	Map<int, int> Value1;
	Value1.Set(1, 4);
	Value1.Set(2, 5);
	Value1.Set(3, 6);

	EXPECT_TRUE( Value1.ContainsValue(4));
	EXPECT_TRUE( Value1.ContainsValue(5));
	EXPECT_TRUE( Value1.ContainsValue(6));
	EXPECT_TRUE(!Value1.ContainsValue(7));
}

TEST(MapTests, RemoveKey)
{
	Map<int, int> Value1;
	Value1.Set(1, 4);
	Value1.Set(2, 5);
	Value1.Set(3, 6);

	Value1.RemoveKey(1);

	EXPECT_TRUE( Value1.Count() == 2);
	EXPECT_TRUE( Value1.ContainsKey(2));
	EXPECT_TRUE( Value1.ContainsKey(3));
	EXPECT_TRUE(!Value1.ContainsKey(1));
}

TEST(MapTests, RemoveValue)
{
	Map<int, int> Value1;
	Value1.Set(1, 4);
	Value1.Set(2, 5);
	Value1.Set(3, 6);

	Value1.RemoveValue(6);

	EXPECT_TRUE( Value1.Count() == 2);
	EXPECT_TRUE( Value1.ContainsValue(4));
	EXPECT_TRUE( Value1.ContainsValue(5));
	EXPECT_TRUE(!Value1.ContainsValue(6));
}

TEST(MapTests, RemoveMatches)
{
	Map<int, int> Value1;
	Value1.Set(1, 4);
	Value1.Set(2, 5);
	Value1.Set(3, 6);

	Value1.RemoveMatches([] (const Map<int,int>::PairType& Pair) -> bool {
		return Pair.Key() == 2;
	});

	EXPECT_TRUE( Value1.Count() == 2);
	EXPECT_TRUE( Value1.ContainsKey(1));
	EXPECT_TRUE( Value1.ContainsKey(3));
	EXPECT_TRUE(!Value1.ContainsKey(2));
}

TEST(MapTests, Empty)
{
	Map<int, int> Value1;
	Value1.Set(1, 4);
	Value1.Set(2, 5);
	Value1.Set(3, 6);

	Value1.Empty();

	EXPECT_TRUE(Value1.Count() == 0);
}

TEST(MapTests, IsEmpty)
{
	Map<int, int> Value1;
	Value1.Set(1, 4);
	Value1.Set(2, 5);
	Value1.Set(3, 6);

	Value1.Empty();

	Map<int, int> Value2;

	EXPECT_TRUE(Value1.IsEmpty());
	EXPECT_TRUE(Value2.IsEmpty());
}

TEST(MapTests, Count)
{
	Map<int, int> Value1;
	Value1.Set(1, 4);
	Value1.Set(2, 5);
	Value1.Set(3, 6);

	EXPECT_TRUE(Value1.Count() == 3);
}

TEST(MapTests, SubScript)
{
	Map<int, int> Value1;
	Value1.Set(1, 4);
	Value1.Set(2, 5);
	Value1.Set(3, 6);

	EXPECT_TRUE(Value1[1] == 4);
	EXPECT_TRUE(Value1[2] == 5);
	EXPECT_TRUE(Value1[3] == 6);

	Value1[1] = 7;
	Value1[2] = 8;
	Value1[3] = 9;

	EXPECT_TRUE(Value1[1] == 7);
	EXPECT_TRUE(Value1[2] == 8);
	EXPECT_TRUE(Value1[3] == 9);
}