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

TEST(ArrayTests, Constructors)
{
	Array<int> Value1;

	Value1.Add(1);
	Value1.Add(2);
	Value1.Add(3);

	Array<int> Value2(Value1);

	EXPECT_TRUE(Value2.Length() == 3);
	EXPECT_TRUE(Value2[0] == 1);
	EXPECT_TRUE(Value2[1] == 2);
	EXPECT_TRUE(Value2[2] == 3);
}

TEST(ArrayTests, Resize)
{
	Array<int> Value1;
	Value1.Resize(10);

	EXPECT_TRUE(Value1.Length() == 10);
}

TEST(ArrayTests, Add)
{
	Array<int> Value1;
	Value1.Add(1);
	Value1.Add(2);
	Value1.Add(3);

	EXPECT_TRUE(Value1.Length() == 3);
	EXPECT_TRUE(Value1[0] == 1);
	EXPECT_TRUE(Value1[1] == 2);
	EXPECT_TRUE(Value1[2] == 3);
}

TEST(ArrayTests, AddUnique)
{
	Array<int> Value1;
	Value1.AddUnique(1);
	Value1.AddUnique(2);
	Value1.AddUnique(2);

	EXPECT_TRUE(Value1.Length() == 2);
	EXPECT_TRUE(Value1[0] == 1);
	EXPECT_TRUE(Value1[1] == 2);
}

TEST(ArrayTests, InsertArray)
{
	Array<int> Value1;
	Value1.Add(1);
	Value1.Add(2);
	Value1.Add(3);

	Array<int> Value2;
	Value2.Add(4);
	Value2.Add(5);
	Value2.Add(6);

	Value1.Insert(Value2, 1);

	//1
	//4
	//5
	//6
	//3
	//2

	EXPECT_TRUE(Value1.Length() == 6);
	EXPECT_TRUE(Value1[0] == 1);
	EXPECT_TRUE(Value1[1] == 4);
	EXPECT_TRUE(Value1[2] == 5);
	EXPECT_TRUE(Value1[3] == 6);
	EXPECT_TRUE(Value1[4] == 2);
	EXPECT_TRUE(Value1[5] == 3);
}

TEST(ArrayTests, Insert)
{
	Array<int> Value1;
	Value1.Add(1);
	Value1.Add(2);
	Value1.Add(3);

	Value1.Insert(4, 1);
	Value1.Insert(5, 3);

	EXPECT_TRUE(Value1.Length() == 5);
	EXPECT_TRUE(Value1[0] == 1);
	EXPECT_TRUE(Value1[1] == 4);
	EXPECT_TRUE(Value1[2] == 2);
	EXPECT_TRUE(Value1[3] == 5);
	EXPECT_TRUE(Value1[4] == 3);
}

TEST(ArrayTests, InsertUnordered)
{
	Array<int> Value1;
	Value1.Add(1);
	Value1.Add(2);
	Value1.Add(3);

	Value1.InsertUnordered(4, 0);

	EXPECT_TRUE(Value1.Length() == 4);
	EXPECT_TRUE(Value1[0] == 4);
	EXPECT_TRUE(Value1[1] == 2);
	EXPECT_TRUE(Value1[2] == 3);
	EXPECT_TRUE(Value1[3] == 1);
}

TEST(ArrayTests, AppendArray)
{
	Array<int> Value1;
	Value1.Add(1);
	Value1.Add(2);
	Value1.Add(3);

	Array<int> Value2;
	Value2.Add(4);
	Value2.Add(5);
	Value2.Add(6);

	Value1.Append(Value2);

	EXPECT_TRUE(Value1.Length() == 6);
	EXPECT_TRUE(Value1[0] == 1);
	EXPECT_TRUE(Value1[1] == 2);
	EXPECT_TRUE(Value1[2] == 3);
	EXPECT_TRUE(Value1[3] == 4);
	EXPECT_TRUE(Value1[4] == 5);
	EXPECT_TRUE(Value1[5] == 6);
}

TEST(ArrayTests, Find)
{
	Array<int> Value1;
	Value1.Add(1);
	Value1.Add(2);
	Value1.Add(3);
	Value1.Add(4);
	Value1.Add(5);
	Value1.Add(6);
	Value1.Add(4);

	EXPECT_TRUE(Value1.Find(1) == 0);
	EXPECT_TRUE(Value1.Find(2) == 1);
	EXPECT_TRUE(Value1.Find(3) == 2);
	EXPECT_TRUE(Value1.Find(4) == 3);
	EXPECT_TRUE(Value1.Find(5) == 4);
	EXPECT_TRUE(Value1.Find(6) == 5);

	EXPECT_TRUE(Value1.Find(4, 4) == 6);
}

TEST(ArrayTests, FindLast)
{
	Array<int> Value1;
	Value1.Add(1);
	Value1.Add(2);
	Value1.Add(3);
	Value1.Add(4);
	Value1.Add(5);
	Value1.Add(6);
	Value1.Add(4);

	EXPECT_TRUE(Value1.FindLast(1) == 0);
	EXPECT_TRUE(Value1.FindLast(2) == 1);
	EXPECT_TRUE(Value1.FindLast(3) == 2);
	EXPECT_TRUE(Value1.FindLast(4) == 6);
	EXPECT_TRUE(Value1.FindLast(5) == 4);
	EXPECT_TRUE(Value1.FindLast(6) == 5);

	EXPECT_TRUE(Value1.FindLast(4, 4) == 3);
}

TEST(ArrayTests, FindMatch)
{
	Array<int> Value1;
	Value1.Add(1);
	Value1.Add(2);
	Value1.Add(3);
	Value1.Add(4);
	Value1.Add(4);
	Value1.Add(4);

	EXPECT_TRUE(Value1.FindMatch([](const int* A) -> bool { return *A == 4; }) == 3);
}

TEST(ArrayTests, FindMatchLast)
{
	Array<int> Value1;
	Value1.Add(1);
	Value1.Add(2);
	Value1.Add(3);
	Value1.Add(4);
	Value1.Add(4);
	Value1.Add(4);

	EXPECT_TRUE(Value1.FindMatchLast([](const int* A) -> bool { return *A == 4; }) == 5);
}

TEST(ArrayTests, ForEach)
{
	Array<int> Value1;
	Value1.Add(1);
	Value1.Add(2);
	Value1.Add(3);
	Value1.Add(4);

	int Counter = 0;
	Value1.ForEach([&Counter](const int* A) { return Counter += *A; });

	EXPECT_TRUE(Counter == 10);
}

TEST(ArrayTests, Contains)
{
	Array<int> Value1;
	Value1.Push(0);
	Value1.Push(1);
	Value1.Push(2);
	Value1.Push(3);
	Value1.Push(4);
	Value1.Push(5);

	EXPECT_TRUE(!Value1.Contains(-1));
	EXPECT_TRUE( Value1.Contains( 0));
	EXPECT_TRUE( Value1.Contains( 1));
	EXPECT_TRUE( Value1.Contains( 2));
	EXPECT_TRUE( Value1.Contains( 3));
	EXPECT_TRUE( Value1.Contains( 4));
	EXPECT_TRUE( Value1.Contains( 5));
	EXPECT_TRUE(!Value1.Contains( 6));
}

TEST(ArrayTests, RemoveAt)
{
	Array<int> Value1;
	Value1.Push(0);
	Value1.Push(1);
	Value1.Push(2);
	Value1.Push(3);
	Value1.Push(4);
	Value1.Push(5);

	Value1.RemoveAt(1);

	EXPECT_TRUE(Value1.Length() == 5);
	EXPECT_TRUE(Value1[0] == 0);
	EXPECT_TRUE(Value1[1] == 2);
	EXPECT_TRUE(Value1[2] == 3);
	EXPECT_TRUE(Value1[3] == 4);
	EXPECT_TRUE(Value1[4] == 5);
}

TEST(ArrayTests, RemoveAtUnordered)
{
	Array<int> Value1;
	Value1.Push(0);
	Value1.Push(1);
	Value1.Push(2);
	Value1.Push(3);
	Value1.Push(4);
	Value1.Push(5);

	Value1.RemoveAtUnordered(1);

	EXPECT_TRUE(Value1.Length() == 5);
	EXPECT_TRUE(Value1[0] == 0);
	EXPECT_TRUE(Value1[1] == 5); 
	EXPECT_TRUE(Value1[2] == 2);
	EXPECT_TRUE(Value1[3] == 3);
	EXPECT_TRUE(Value1[4] == 4);
}

TEST(ArrayTests, Remove)
{
	Array<int> Value1;
	Value1.Push(0);
	Value1.Push(1);
	Value1.Push(2);
	Value1.Push(3);
	Value1.Push(4);
	Value1.Push(5);

	Value1.Remove(1);
	Value1.Remove(4);

	EXPECT_TRUE(Value1.Length() == 4);
	EXPECT_TRUE(Value1[0] == 0);
	EXPECT_TRUE(Value1[1] == 2);
	EXPECT_TRUE(Value1[2] == 3);
	EXPECT_TRUE(Value1[3] == 5);
}

TEST(ArrayTests, RemoveUnordered)
{
	Array<int> Value1;
	Value1.Push(0);
	Value1.Push(1);
	Value1.Push(2);
	Value1.Push(3);
	Value1.Push(4);
	Value1.Push(5);

	Value1.RemoveUnordered(1);

	EXPECT_TRUE(Value1.Length() == 5);
	EXPECT_TRUE(Value1[0] == 0);
	EXPECT_TRUE(Value1[1] == 5);
	EXPECT_TRUE(Value1[2] == 2);
	EXPECT_TRUE(Value1[3] == 3);
	EXPECT_TRUE(Value1[4] == 4);
}

TEST(ArrayTests, RemoveMatches)
{
	Array<int> Value1;
	Value1.Add(4);
	Value1.Add(1);
	Value1.Add(4);
	Value1.Add(2);
	Value1.Add(4);
	Value1.Add(3);
	Value1.Add(4);

	Value1.RemoveMatches([](const int* A) -> bool { return *A == 4; });

	EXPECT_TRUE(Value1.Length() == 3);
	EXPECT_TRUE(Value1[0] == 1);
	EXPECT_TRUE(Value1[1] == 2);
	EXPECT_TRUE(Value1[2] == 3);
}

TEST(ArrayTests, RemoveMatchesUnordered)
{
	Array<int> Value1;
	Value1.Push(0);
	Value1.Push(1);
	Value1.Push(2);
	Value1.Push(3);
	Value1.Push(4);
	Value1.Push(5);

	Value1.RemoveMatchesUnordered([](const int* A) -> bool { return *A == 1; });

	EXPECT_TRUE(Value1.Length() == 5);
	EXPECT_TRUE(Value1[0] == 0);
	EXPECT_TRUE(Value1[1] == 5);
	EXPECT_TRUE(Value1[2] == 2);
	EXPECT_TRUE(Value1[3] == 3);
	EXPECT_TRUE(Value1[4] == 4);
}

TEST(ArrayTests, Slice)
{
	Array<int> Value1;
	Value1.Push(0);
	Value1.Push(1);
	Value1.Push(2);
	Value1.Push(3);
	Value1.Push(4);
	Value1.Push(5);

	Array<int> Value2 = Value1.Slice(1, 3);

	EXPECT_TRUE(Value2.Length() == 3);
	EXPECT_TRUE(Value2[0] == 1);
	EXPECT_TRUE(Value2[1] == 2);
	EXPECT_TRUE(Value2[2] == 3);
}

TEST(ArrayTests, Reverse)
{
	Array<int> Value1;
	Value1.Push(0);
	Value1.Push(1);
	Value1.Push(2);
	Value1.Push(3);
	Value1.Push(4);
	Value1.Push(5);
	Value1.Reverse();

	EXPECT_TRUE(Value1[0] == 5);
	EXPECT_TRUE(Value1[1] == 4);
	EXPECT_TRUE(Value1[2] == 3);
	EXPECT_TRUE(Value1[3] == 2);
	EXPECT_TRUE(Value1[4] == 1);
	EXPECT_TRUE(Value1[5] == 0);
}

TEST(ArrayTests, Sort)
{
	Array<int> Value1;
	Value1.Push(5);
	Value1.Push(1);
	Value1.Push(2);
	Value1.Push(3);
	Value1.Push(4);
	Value1.Push(0);
	Value1.Sort([](const int* A, const int* B) -> int { return (*A) - (*B); });

	EXPECT_TRUE(Value1[0] == 0);
	EXPECT_TRUE(Value1[1] == 1);
	EXPECT_TRUE(Value1[2] == 2);
	EXPECT_TRUE(Value1[3] == 3);
	EXPECT_TRUE(Value1[4] == 4);
	EXPECT_TRUE(Value1[5] == 5);
}

TEST(ArrayTests, Push)
{
	Array<int> Value1;
	Value1.Push(1);
	Value1.Push(2);
	Value1.Push(3);

	EXPECT_TRUE(Value1.Length() == 3);
	EXPECT_TRUE(Value1[0] == 1);
	EXPECT_TRUE(Value1[1] == 2);
	EXPECT_TRUE(Value1[2] == 3);
}

TEST(ArrayTests, Pop)
{
	Array<int> Value1;
	Value1.Push(1);
	Value1.Push(2);
	Value1.Push(3);

	Value1.Pop();

	EXPECT_TRUE(Value1.Length() == 2);
	EXPECT_TRUE(Value1[0] == 1);
	EXPECT_TRUE(Value1[1] == 2);
}

TEST(ArrayTests, IsEmpty)
{
	Array<int> Value1;

	Array<int> Value2;
	Value2.Push(1);

	EXPECT_TRUE(Value1.IsEmpty());
	EXPECT_TRUE(!Value2.IsEmpty());
}

TEST(ArrayTests, Empty)
{
	Array<int> Value1;
	Value1.Push(1);
	Value1.Push(2);
	Value1.Push(3);
	Value1.Empty();

	EXPECT_TRUE(Value1.IsEmpty());
}

TEST(ArrayTests, IsValidIndex)
{
	Array<int> Value1;
	Value1.Push(1);
	Value1.Push(2);
	Value1.Push(3);

	EXPECT_TRUE(!Value1.IsValidIndex(-1));
	EXPECT_TRUE( Value1.IsValidIndex( 0));
	EXPECT_TRUE( Value1.IsValidIndex( 1));
	EXPECT_TRUE( Value1.IsValidIndex( 2));
	EXPECT_TRUE(!Value1.IsValidIndex( 3));
}

TEST(ArrayTests, FirstAndLast)
{
	Array<int> Value1;
	Value1.Push(1);
	Value1.Push(2);
	Value1.Push(3);

	EXPECT_TRUE(*Value1.Last()  == 3);
	EXPECT_TRUE(*Value1.First() == 1);
}

TEST(ArrayTests, Assignment)
{
	Array<int> Value1;
	Value1.Push(1);
	Value1.Push(2);
	Value1.Push(3);

	Array<int> Value2;
	Value2 = Value1;

	EXPECT_TRUE(Value2.Length() == 3);
	EXPECT_TRUE(Value2[0] == 1);
	EXPECT_TRUE(Value2[1] == 2);
	EXPECT_TRUE(Value2[2] == 3);
}

TEST(ArrayTests, Iterator)
{
	Array<int> Value1;
	Value1.Add(1);
	Value1.Add(2);
	Value1.Add(3);
	Value1.Add(4);

	int Counter = 0;
	for (int i : Value1)
	{
		Counter += i;
	}

	EXPECT_TRUE(Counter == 10);
}