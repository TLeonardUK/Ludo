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

#include "Core/Public/Build/Common.h"

#include "Core/Public/Debug/StackTrace.h"

#include "Core/Public/Encodings/ITextEncoding.h"
#include "Core/Public/Encodings/Utf8TextEncoding.h"
#include "Core/Public/Encodings/Utf16TextEncoding.h"
#include "Core/Public/Encodings/Utf32TextEncoding.h"

#include "Core/Public/Environment/Environment.h"

#include "Core/Public/IO/Streams/BinaryStream.h"
#include "Core/Public/IO/Streams/BitStream.h"
#include "Core/Public/IO/Streams/FileStream.h"
#include "Core/Public/IO/Streams/IStream.h"
#include "Core/Public/IO/Streams/StreamWrapper.h"
#include "Core/Public/IO/Streams/TextStream.h"
#include "Core/Public/IO/File.h"
#include "Core/Public/IO/Module.h"
#include "Core/Public/IO/Path.h"
#include "Core/Public/IO/Process.h"

#include "Core/Public/Math/Geometry/Frustum.h"
#include "Core/Public/Math/Geometry/Matrix.h"
#include "Core/Public/Math/Geometry/Plane.h"
#include "Core/Public/Math/Geometry/Vector.h"
#include "Core/Public/Math/Standard/Standard.h"

#include "Core/Public/Memory/Allocator.h"
#include "Core/Public/Memory/ArenaAllocator.h"
#include "Core/Public/Memory/Endianness.h"
#include "Core/Public/Memory/HeapAllocator.h"
#include "Core/Public/Memory/Memory.h"
#include "Core/Public/Memory/MemoryArena.h"
#include "Core/Public/Memory/StackAllocator.h"

#include "Core/Public/Meta/Traits/INotCopyable.h"
#include "Core/Public/Meta/IPimpl.h"
#include "Core/Public/Meta/ISingleton.h"
#include "Core/Public/Meta/TypeConstructor.h"

#include "Core/Public/Parallel/Jobs/Async.h"
#include "Core/Public/Parallel/Jobs/JobScheduler.h"
#include "Core/Public/Parallel/Jobs/ParallelFor.h"
#include "Core/Public/Parallel/Types/ConcurrentQueue.h"
#include "Core/Public/Parallel/Atomic.h"
#include "Core/Public/Parallel/AutoResetEvent.h"
#include "Core/Public/Parallel/Barrier.h"
#include "Core/Public/Parallel/ManualResetEvent.h"
#include "Core/Public/Parallel/Mutex.h"
#include "Core/Public/Parallel/Semaphore.h"
#include "Core/Public/Parallel/Thread.h"
#include "Core/Public/Parallel/ThreadLocal.h"

#include "Core/Public/Reflection/PrimitiveTypes/PrimitiveTypes.h"
#include "Core/Public/Reflection/Attribute.h"
#include "Core/Public/Reflection/Class.h"
#include "Core/Public/Reflection/Enum.h"
#include "Core/Public/Reflection/Field.h"
#include "Core/Public/Reflection/Method.h"
#include "Core/Public/Reflection/Reflection.h"
#include "Core/Public/Reflection/Type.h"
#include "Core/Public/Reflection/TypeModule.h"

#include "Core/Public/Time/Time.h"
#include "Core/Public/Time/Timeout.h"

#include "Core/Public/Types/Formatters/SimpleStringFormatter.h"
#include "Core/Public/Types/Formatters/SizeStringFormatter.h"
#include "Core/Public/Types/Formatters/TimeStringFormatter.h"
#include "Core/Public/Types/Array.h"
#include "Core/Public/Types/Cache.h"
#include "Core/Public/Types/CacheArray.h"
#include "Core/Public/Types/Color.h"
#include "Core/Public/Types/Error.h"
#include "Core/Public/Types/Linked.h"
#include "Core/Public/Types/Map.h"
#include "Core/Public/Types/Pair.h"
#include "Core/Public/Types/String.h"
#include "Core/Public/Types/StringId.h"
#include "Core/Public/Types/Variant.h"