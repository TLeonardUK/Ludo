/*
MicroBuild
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

// Can't use pragma once in PCH's or we get erronous warnings in older versions
// of llvm/clang (aka xcode), see here - https://reviews.llvm.org/D19815
#ifndef _MICROBUILD_PCH_
#define _MICROBUILD_PCH_

#include <stdio.h>
#include <vector>
#include <string>
#include <string.h>
#include <ctime>
#include <assert.h>
#include <cassert>
#include <stdarg.h>
#include <chrono>
#include <map>
#include <algorithm>
#include <climits>
#include <memory>

#include "Core/Compiler.h"
#include "Core/Log.h"

#include "Schemas/Plugin/PluginInterface.h"

#endif