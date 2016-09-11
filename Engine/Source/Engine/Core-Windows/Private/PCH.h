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

#include "Core/Public/Core.h"

#include <stdint.h>
#include <cstdlib>
#include <stdarg.h>
#include <cstdio>
#include <string.h>

#include <comutil.h>
#include <Wbemcli.h>
#include <Wbemidl.h>
#include <Wbemidl.h>

#include <Windows.h>
#include <Lmcons.h>
#include <VersionHelpers.h>
#include <shlwapi.h>
#include <shlobj.h>

#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <setjmp.h>

#pragma warning(push)
#pragma warning(disable:4091) // 2015 introduced some extra warnings about typedef's, dbghelp.h/shlobj.h now spams them :(
#include <dbghelp.h>
#include <Shlobj.h>
#pragma warning(pop)

#include <Winsock2.h>