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

namespace Ludo {

/// \brief TODO
class TimeSpan
{
private:
	uint64 m_Milliseconds;

public:

	static TimeSpan Infinite;


	/// \brief TODO
	TimeSpan();

	/// \brief TODO
	TimeSpan(uint64 TotalMilliseconds);

	/// \brief TODO
	uint64 GetTotalMilliseconds() const;

	/// \brief TODO
	uint64 GetTotalSeconds() const;

	/// \brief TODO
	uint64 GetTotalMinutes() const;

	/// \brief TODO
	uint64 GetTotalHours() const;

	/// \brief TODO
	uint64 GetTotalDays() const;

	/// \brief TODO
	uint64 GetMilliseconds() const;

	/// \brief TODO
	uint64 GetSeconds() const;

	/// \brief TODO
	uint64 GetMinutes() const;

	/// \brief TODO
	uint64 GetHours() const;

	/// \brief TODO
	uint64 GetDays() const;


	/// \brief TODO
	bool operator ==(const TimeSpan& Other) const;

	/// \brief TODO
	bool operator !=(const TimeSpan& Other) const;

	/// \brief TODO
	bool operator <(const TimeSpan& Other) const;

	/// \brief TODO
	bool operator >(const TimeSpan& Other) const;

	/// \brief TODO
	bool operator <=(const TimeSpan& Other) const;

	/// \brief TODO
	bool operator >=(const TimeSpan& Other) const;

	/// \brief TODO
	TimeSpan operator -(const TimeSpan& Other) const;

	/// \brief TODO
	TimeSpan operator +(const TimeSpan& Other) const;

};

/// \brief TODO
class Time
{
private:
	uint64 m_UnixTimestamp;

public:

	/// \brief TODO
	Time();

	/// \brief TODO
	Time(uint64 UnixTimestamp);

	/// \brief TODO
	uint64 GetUnixTimestamp();

	/// \brief TODO
	bool operator ==(const Time& Other) const;

	/// \brief TODO
	bool operator !=(const Time& Other) const;

	/// \brief TODO
	bool operator <(const Time& Other) const;

	/// \brief TODO
	bool operator >(const Time& Other) const;

	/// \brief TODO
	bool operator <=(const Time& Other) const;

	/// \brief TODO
	bool operator >=(const Time& Other) const;

	/// \brief TODO
	static Time Now();

	/// \brief TODO
	static Time Epoch();

	/// \brief TODO
	//static Time PerfNow();

	/// \brief TODO
	static TimeSpan AppDuration();

};

};