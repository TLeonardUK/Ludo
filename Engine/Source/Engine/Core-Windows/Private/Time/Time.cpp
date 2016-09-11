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

#include "Core-Windows/Private/Platform/Helper/TimeConversion.h"

namespace Ludo {

Time Time::Now()
{
	FILETIME FileTime;
	SYSTEMTIME SystemTime;

	GetSystemTime(&SystemTime);            
	SystemTimeToFileTime(&SystemTime, &FileTime);

	return Time(Ludo::Windows::FileTimeToUnixTimestamp(FileTime));
}

TimeSpan Time::AppDuration()
{
	static LARGE_INTEGER freq;
	static LARGE_INTEGER start;
	static int state = 0; // 0=not initialized, -1=not supported, 1=supported

	if (state == 0)
	{
		if (QueryPerformanceFrequency(&freq) == 0)
		{
			state = -1;
		}
		else
		{
			QueryPerformanceCounter(&start);
			state = 1;
		}
	}

	// Not supported, use tick count.
	if (state == -1)
	{
		return TimeSpan(GetTickCount());
	}
	else
	{
		LARGE_INTEGER tickCount;
		QueryPerformanceCounter(&tickCount);

		LONGLONG f = freq.QuadPart / 1000;
		return TimeSpan((tickCount.QuadPart - start.QuadPart) / f);
	}
}

}; // namespace Ludo