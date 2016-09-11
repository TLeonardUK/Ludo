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

class Allocator;

/// \brief TODO
#define LD_ARENA_NEW(Arena, Type, ...) (ArenaAllocator<Arena>().New<Type>(__VA_ARGS__))

/// \brief TODO
#define LD_NEW(Allocator, Type, ...) (Allocator.New<Type>(__VA_ARGS__))

/// \brief TODO
#define LD_ARENA_DELETE(Arena, Obj) (ArenaAllocator<Arena>().Delete(Obj))

/// \brief TODO
#define LD_DELETE(Allocator, Obj) (Allocator.Delete(Obj))

/// \brief TODO
#define LD_ARENA_NEW_ARRAY(Arena, Type, Size) (ArenaAllocator<Arena>().NewArray<Type>(Size))

/// \brief TODO
#define LD_NEW_ARRAY(Allocator, Type, Size) (Allocator.NewArray<Type>(Size))

/// \brief TODO
#define LD_ARENA_DELETE_ARRAY(Arena, Obj) (ArenaAllocator<Arena>().DeleteArray(Obj))

/// \brief TODO
#define LD_DELETE_ARRAY(Allocator, Obj) (Allocator.DeleteArray(Obj))

}; // namespace Ludo
