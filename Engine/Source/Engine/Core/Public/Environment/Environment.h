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

/// \brief Represents a feature or extension available to use on the current cpu.
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

/// \brief Represents paths to common directories on the file-system.
enum class SpecialPath
{
	ExecutableFile,		// Path to the executable path.
	TempDirectory,		// Path to a temporary directory that the local user can write to. 
	SaveDataDirectory,	// Path to a directory that save data should be stored in.
};

/// \brief Holds information about the name and specs of a cpu.
struct CpuInfo
{
	String				Name;
	int					CoreCount;
	int					ThreadCount;
	Array<CpuFeature>	Features;
};

/// \brief Holds information about the name and specs of a gpu.
struct GpuInfo
{
	String				Name;
	uint64				TotalVRam;
};

/// \brief Holds information about the name and specs of a devices RAM.
struct RamInfo
{
    uint64				TotalPhysicalRam;
    uint64				TotalVirtualRam;
	uint64				TotalPageRam;
};

/// \brief Represents the nominal new-line sequence used on the current platform (\r\n for windows etc).
extern const char* NewLineSequence;

/** \brief  Retrieves the number of CPU's available on the current device.
 *
 *  \return Number of CPU's available.
 *
 *  This is individual CPU's chips, not threads or cores. To get threads or cores,
 *  please use GetCpuInfo.
 */
int GetCpuCount();

/** \brief  Retrieves the number of GPU's available on the current device.
 *
 *  \return Number of GPU's available.
 */
int GetGpuCount();

/** \brief  Retrieves information about the given cpu on the device.
 *
 *  \param  CpuIndex Index of cpu to retrieve information for. In the range of 0
 *          to the value returned by GetCpuCount.
 *
 *  \return Information about the given CPU index.
 */
CpuInfo GetCpuInfo(const int CpuIndex);

/** \brief  Retrieves information about the given gpu on the device.
 *
 *  \param  CpuIndex Index of gpu to retrieve information for. In the range of 0
 *          to the value returned by GetGpuCount.
 *
 *  \return Information about the given GPU index.
 */
GpuInfo GetGpuInfo(const int GpuIndex);

/** \brief  Gets the memory available on the current device.
 *
 *  \return Information about memory available.
 *
 *  This provides general cpu-accessible memory information. It does
 *  not break down into platform-specific memory pools, i.e. for non-unified
 *  architectures.
 */
RamInfo GetRamInfo();

/** \brief  Gets a rough idea of the optimal number of operations to run in parallel
 *          on the device.
 *
 *  \return Optimal number of threads to run in parallel.
 *
 *  The optimal number in most circumstances will return the number of logical cores
 *  available to all primary cpu's on the device. It can be used as a rough guide
 *  to how many threads/operations you should run in parallel.
 */
int GetConcurrencyFactor();

/** \brief  Retrieves the name of the current platform.
 *
 *  \return Name of the current platform. This is primarily used for display purposes,
 *          do not assume its value on any specific platform as it may change between
 *          operating systems / models.
 */
String GetPlatformName();

/** \brief  Retrieves the username of the currently logged in user.
 *
 *  \return Name of the currently logged in user. If no user is logged in, or the 
 *          concept of a logged in user is not applicable to the platform, then 
 *          this will return an empty string.
 */
String GetUsername();

/** \brief  Retrieves a common folder on the devices file system.
 *
 *  \param  Type  Common folder to retrieve path for.
 *
 *  \return Path to the common folder requested.
 */
Path GetSpecialPath(SpecialPath Type);

/** \brief  Gets command line arguments.
 *
 *  \return The command line arguments that were passed to the program by the 
 *          operating system on initialization.
 */
Array<String> GetCommandLineArguments();

/** \brief  Performs pre-initilization of the platform sub-system.
 *
 * This is called before anything else occurs. What occurs is platform-specific, but
 * typically this registers initial logging listeners.
 */
void PreInit();

/** \brief  Performs primary initialization of the platform sub-system.
 *
 * What occurs is platform-specific, this will configure whatever is required for the
 * platform to run.
 */
void Init();

/** \brief  Performs termination of the platform sub-system.
 *
 * What occurs is platform-specific, but typically this will tear down everything 
 * that was initialized in Init.
 */
void Term(); 

/// \brief  If a debugger is attached, this breaks into it.
void DebugBreak();

/// \brief  Aborts execution without any cleanup. 
void Exit();

/** \brief  Delays the current thread for the given time span.
 *
 *  \param  Time  Timespan to delay for.
 */
void Delay(TimeSpan Time);

/** \brief  Gets the given environment variable based on its tag.
 *
 *  \param  Tag  Name of environment variable to get the value for.
 *
 *  \return Value of environment variable with Name.
 */
String GetEnvironmentVariable(const String& Tag);

}; // namespace Environment
}; // namespace Ludo