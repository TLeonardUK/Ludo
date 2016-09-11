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

// This file contains various common compiler/logging includes that are used
// heavily in a lot of places. Its heavily included all over the place so
// keep whats in here to a minimum.

// Sigh, this allows you to paste things together line Name##__LINE__, which requires indirection
// to work correctly (6.10.3 of C99 standard -_-).
#define LD_INDIRECT_TOKEN_PASTE_SUB(x, y) x ## y
#define LD_INDIRECT_TOKEN_PASTE(x, y) LD_INDIRECT_TOKEN_PASTE_SUB(x, y)

// Compiler/Debugging support.
#include "Core/Public/Build/BuildInfo.h"
#include "Core/Public/Build/Compiler.h"
#include "Core/Public/Reflection/ReflectionMacros.h"
#include "Core/Public/Memory/MemoryOverrides.h"
#include "Core/Public/Debug/Assert.h"
#include "Core/Public/Debug/Log.h"
#include "Core/Public/Memory/MemoryMacros.h"