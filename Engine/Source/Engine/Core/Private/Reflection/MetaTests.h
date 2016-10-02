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

// TODO: this needs to be moved to unit tests or something.

#include "Core/Public/Reflection/Reflection.h"
#include "Core/Public/Reflection/ReflectionMacros.h"
#include "Core/Public/Object/Object.h"

namespace Ludo {

// The files in this file tests various aspects of the metadata parser 
// to ensure it can cope with expected conditions.

META()
struct MetaStruct1
{
};

META
(
	Expose,
	DisplayName("Meta Struct 2"), 
	Description("Something", 123)
)
struct MetaStruct2
{
	GENERATED_BODY()

};

META(
	DisplayName("Meta Struct 3")
)
struct MetaStruct3
{
};

META()
enum class EType
{
	META(DisplayName("Key 1"))
	Key1,

	META(DisplayName("Key 2")) Key2,	
	
	META(DisplayName("Key 3")) Key3, META(DisplayName("Key 4")) Key4
};

META()
template <typename X, int Y = 23>
class MetaTemplateClass1 : public Object
{
public:
	
	//META(Expose)
	//int(*Val2) (int,float);

	//META(Expose)
	//X Val1;

};

META()
class MetaTemplateClass2 
	: public MetaTemplateClass1<float, 32>
{
public:

	//META(Expose)
	//int(*Val2) (int,float);

};

/*
META()
template <typename X>
class MetaTemplateClass1<X, 32> : public Object
{
public:

	META(Expose)
	X Val1;

};*/

META()
/* 
\brief Something Something Something
\param A Test
*/
class MetaClass1 : public Object
{
public:
	META(Test)
	MetaTemplateClass1<float> TemplateVal1;

	/*
	META(Expose) unsigned long long UI64;
	META(Expose) long long I64;
	META(Expose) unsigned int UI32;
	META(Expose) int I32;
	META(Expose) unsigned short UI16;
	META(Expose) short I16;
	META(Expose) unsigned char U8;
	META(Expose) char I8;
	META(Expose) float F32;
	META(Expose) double D64;
	META(Expose) bool B8;

	META(Expose) unsigned char* U8P;
	META(Expose) char* I8P;

	META(Expose) char& CharLRef;
	META(Expose) char&& CharRRef;

	META(Transient)
	int Val2;

	META(Transient)
	MetaStruct3 Val3;
	
	META(Transient)
	const MetaStruct3* Val3P;
	
	META(Transient)
	MetaStruct3* Val3PD;
	META(Transient)
	MetaStruct3* Val3PD2;
	META(Transient)
	MetaStruct3* Val3PD3;
	META(Transient)
	MetaStruct3** Val3PD4;
	
	META(Transient)
	EType Val4;

	META(Transient)
	const EType* Val4C;

	META(Expose)
	int AddNumbers(		
		META(Description("Does Something Cool"))
		int X,
		
		META(Description("Does Something Else Cool"))
		int Y = 3
	)
	{
		return X + Y;
	}


	META(Expose)
	int AddNumbers2(		
		META(Description("Does Something Cool")) int X, META(Description("Does Something Else Cool")) int Y
	)
	{
		return X + Y;
	}
	*/

};




}; // namespace Ludo
