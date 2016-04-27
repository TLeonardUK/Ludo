// Copyright 2016 TwinDrills
#pragma once

// This file is a global header thats included automatically by every compiled file. Be careful
// what you put in this as it can drastically increase compile times! Keep things slim.

// Compiler/Debugging support.
#include "Core/Build/BuildInfo.h"
#include "Core/Build/Compiler.h"
#include "Core/Debug/Assert.h"
#include "Core/Debug/Log.h"
 
#include <new>

namespace Ludo
{
	class Allocator;
}

/*
// New operator overloads.
void* operator new  (std::size_t count);
void* operator new[](std::size_t count);
void* operator new  (std::size_t count, const std::nothrow_t& tag);
void* operator new[](std::size_t count, const std::nothrow_t& tag);

// Delete operator overloads.
void operator delete  (void* ptr);
void operator delete[](void* ptr);
void operator delete  (void* ptr, const std::nothrow_t& tag);
void operator delete[](void* ptr, const std::nothrow_t& tag);
*/