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

// This file defines the default memory arena properties. Note that the 
// default-size and resizable flags can be overridden per-platform in GetPlatformArenaStateOverride.

DEFINE_MEMORY_ARENA(Reflection, false, 1024 * 1024 *  32, CPU, "Reflection Memory Arena",	"Memory arena that all reflection meta-data is stored in.")
DEFINE_MEMORY_ARENA(LibC,		false, 1024 * 1024 * 512, CPU, "LibC Memory Arena",			"Memory arena that all libc allocations are redirected to, unless otherwise overriden.")
DEFINE_MEMORY_ARENA(Container,	false, 1024 * 1024 *  32, CPU, "Container Memory Arena",	"Memory arena that is used by all core containers, unless otherwise overridden.")
DEFINE_MEMORY_ARENA(String,		false, 1024 * 1024 *  32, CPU, "String Memory Arena",		"Memory arena that is used by all strings, unless otherwise overridden.")
DEFINE_MEMORY_ARENA(Platform,	false, 1024 * 1024 *  32, CPU, "Platform Memory Arena",		"Memory arena that is used by all platform specific classes (Threads, etc), unless otherwise overridden.")
