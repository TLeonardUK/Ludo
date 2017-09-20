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
	 
/* 
This is used to provide attributes to specific constructs in the code. This forms
the basis of our reflection system. Each parameter passed to the META tag is expanded to 
an instance of NameAttribute(Arguments) that is stored in the object class.

eg.
	Meta(Name("Test"))

Will expand to an attribute list containing:
	new NameAttribute("Test");
*/
#define META(...) 

/* 
This macro must be included inside all Object derived classes, it writes various
meta-data related data into the class. The metadata generator will enforce inclusion
of this macro in all Object derived classes.

This is implemented as a macro that expands to another macro that is defined in the
generated header file.
*/
#ifdef LD_REFLECTION_GENERATOR

#define GENERATED_BODY() 

#else

#define GENERATED_BODY() LD_INDIRECT_TOKEN_PASTE_6(LD_, LD_REFLECTION_FILE_ID, _, __LINE__, _, GENERATED_BODY)

#endif
	 
// This macro is what is included by the GENERATED_BODY macro.
#define LD_GENERATED_CLASS_BODY(ClassName, SuperClassName) \
	private: \
		/* Handy typedef for accessing the class we are derived from. */ \
		typedef SuperClassName Super; \
		\
		/* Handy typedef for referencing our own class. */ \
		typedef ClassName Self; \
		\
		/* Always allow reflection accessor to use and abuse our privates. */ \
		friend class ReflectionAccessor; \
		\
	public: \

// Same as LD_GENERATED_CLASS_BODY but with no super typedef.
#define LD_GENERATED_CLASS_BODY_NO_SUPER(ClassName) \
	private: \
		/* Handy typedef for referencing our own class. */ \
		typedef ClassName Self; \
		\
		/* Always allow reflection accessor to use and abuse our privates. */ \
		friend class ReflectionAccessor; \
		\
	public: \

}; // namespace Ludo
