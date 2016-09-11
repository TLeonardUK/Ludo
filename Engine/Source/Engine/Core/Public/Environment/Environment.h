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

#include "Core/Public/Types/Array.h"
#include "Core/Public/Types/String.h"
#include "Core/Public/IO/Path.h"

namespace Ludo {

// \brief Defined in engine code, called by the platform entry point
//		  to begin game execution!
extern int LudoEntryPoint();

namespace Environment {

enum class CpuFeature
{
	MMX,
	SSE,
	SSE2,
	SSE3,
	SSSE3,
	SSE41,
	SSE42,
	AES,
	AVX,
	FMA3,
	RDRAND,
	AVX2,
	BMT1,
	BMT2,
	ADX,
	SHA,
	PREFETCHWT1,
	AVX512F,
	AVX512CD,
	AVX512PF,
	AVX512ER,
	AVX512VL,
	AVX512BW,
	AVX512DQ,
	AVX512IFMA,
	AVX512VBMI,
	X64,
	ABM,
	SSE4a,
	FMA4,
	XOP,

	Count
};

extern const char* CpuFeatureName[(int)CpuFeature::Count];

enum class SpecialPath
{
	ExecutableFile,		// Path to the executable path.
	TempDirectory,		// Path to a temporary directory that the local user can write to. 
	SaveDataDirectory,	// Path to a directory that save data should be stored in.
};

struct CpuInfo
{
	String				Name;
	int					CoreCount;
	int					ThreadCount;
	Array<CpuFeature>	Features;
};

struct GpuInfo
{
	String				Name;
	uint64				TotalVRam;
};

struct RamInfo
{
	uint64				TotalPhysicalRam;
	uint64				TotalVirtualRam;
	uint64				TotalPageRam;
};

/// \brief TODO
extern const char* NewLineSequence;

/// \brief TODO
int GetCpuCount();

/// \brief TODO
int GetGpuCount();

/// \brief TODO
CpuInfo GetCpuInfo(const int CpuIndex);

/// \brief TODO
GpuInfo GetGpuInfo(const int GpuIndex);

/// \brief TODO
RamInfo GetRamInfo();

/// \brief TODO
int GetConcurrencyFactor();

/// \brief TODO
String GetPlatformName();

/// \brief TODO
String GetUsername();

/// \brief TODO
Path GetSpecialPath(SpecialPath Type);

/// \brief TODO
Array<String> GetCommandLineArguments();

/// \brief TODO
void PreInit();

/// \brief TODO
void Init();

/// \brief TODO
void Term();

/// \brief TODO
void DebugBreak();

/// \brief TODO
void Exit();

/// \brief TODO
void Delay(TimeSpan time);

// Gets the given environment variable based on its tag.
String GetEnvironmentVariable(const String& tag);

}; // namespace Environment
}; // namespace Ludo