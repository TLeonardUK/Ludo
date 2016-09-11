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

#include "PCH.h"

#include "Core/Public/Environment/Environment.h"

namespace Ludo {
namespace Environment {

const char* CpuFeatureName[(int)CpuFeature::Count] = {
	"MMX",
	"SSE",
	"SSE2",
	"SSE3",
	"SSSE3",
	"SSE41",
	"SSE42",
	"AES",
	"AVX",
	"FMA3",
	"RDRAND",
	"AVX2",
	"BMT1",
	"BMT2",
	"ADX",
	"SHA",
	"PREFETCHWT1",
	"AVX512F",
	"AVX512CD",
	"AVX512PF",
	"AVX512ER",
	"AVX512VL",
	"AVX512BW",
	"AVX512DQ",
	"AVX512IFMA",
	"AVX512VBMI",
	"X64",
	"ABM",
	"SSE4a",
	"FMA4",
	"XOP"
};

String GetEnvironmentVariable(const String& tag)
{
	const char* ptr = std::getenv(tag.Data());
	if (ptr == nullptr)
	{
		return "";
	}
	else
	{
		return ptr;
	}
}

}; // namespace Environment
}; // namespace Ludo