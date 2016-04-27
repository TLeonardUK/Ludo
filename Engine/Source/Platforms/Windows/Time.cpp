// Copyright 2016 TwinDrills

#include "Core/Platform/Time.h"

#include <Windows.h>

namespace Ludo {

Time Time::Now()
{
	// TODO
	return Time();
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