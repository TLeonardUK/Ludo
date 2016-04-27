// Copyright 2016 TwinDrills

#include "Core/Platform/Environment.h"

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

}; // namespace Environment
}; // namespace Ludo