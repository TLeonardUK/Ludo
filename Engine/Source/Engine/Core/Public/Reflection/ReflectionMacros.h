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

namespace Ludo {

	// Reflection attributes.
#ifdef LD_REFLECTION_GENERATOR

// Ok, so this ugly struct is here so we can do things like
//
// META(X)
// class Somthing;
//
// Which we cannot do by default as the annotate keyword for class/structs/enums/unions
// needs to follow the keyword not preceed it.
//
// The reflection builder determines which "attribute struct" matches
// which field/class/etc.
//
// Don't worry, its not compiled into final code, just for the reflection generator :).
//#define META(...) struct __attribute__((annotate(#__VA_ARGS__))) LD_INDIRECT_TOKEN_PASTE(_MetaAttributes_, __COUNTER__) {}; 

#else


#endif

// This is used to provide attributes to specific constructs in the code. This forms
// the basis of our reflection system.
#define META(...) 

// This macro must be included inside all Object derived classes, it writes various
// meta-data related data into the class.
#define GENERATED_BODY() \
	private:\
		friend class ReflectionAccessor; \
		/*typedef _META_ObjectXSuper Super;*/ \
		void StaticType(); \
	\
	public:


}; // namespace Ludo
