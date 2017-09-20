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

#include "Core/Public/Time/Time.h"

namespace Ludo {

TimeSpan TimeSpan::Infinite(UINT64_MAX);

// ************************************************************************************************

TimeSpan::TimeSpan()
	: m_Milliseconds(0)
{
}

// ************************************************************************************************

TimeSpan::TimeSpan(uint64 TotalMilliseconds)
	: m_Milliseconds(TotalMilliseconds)
{
}

// ************************************************************************************************

uint64 TimeSpan::GetTotalMilliseconds() const
{
	return m_Milliseconds;
}

// ************************************************************************************************

uint64 TimeSpan::GetTotalSeconds() const
{
	return GetTotalMilliseconds() / 1000;
}

// ************************************************************************************************

uint64 TimeSpan::GetTotalMinutes() const
{
	return GetTotalSeconds() / 60;
}

// ************************************************************************************************

uint64 TimeSpan::GetTotalHours() const
{
	return GetTotalMinutes() / 60;
}

// ************************************************************************************************

uint64 TimeSpan::GetTotalDays() const
{
	return GetTotalHours() / 24;
}

// ************************************************************************************************

uint64 TimeSpan::GetMilliseconds() const
{
	return m_Milliseconds % 1000;
}

// ************************************************************************************************

uint64 TimeSpan::GetSeconds() const
{
	return (m_Milliseconds / 1000) % 60;
}

// ************************************************************************************************

uint64 TimeSpan::GetMinutes() const
{
	return ((m_Milliseconds / 1000) / 60) % 60;
}

// ************************************************************************************************

uint64 TimeSpan::GetHours() const
{
	return (((m_Milliseconds / 1000) / 60) / 60) % 24;
}

// ************************************************************************************************

uint64 TimeSpan::GetDays() const
{
	return (((m_Milliseconds / 1000) / 60) / 60) / 24;
}

// ************************************************************************************************

bool TimeSpan::operator ==(const TimeSpan& Other) const
{
	return m_Milliseconds == Other.m_Milliseconds;
}

// ************************************************************************************************

bool TimeSpan::operator !=(const TimeSpan& Other) const
{
	return m_Milliseconds != Other.m_Milliseconds;
}

// ************************************************************************************************

bool TimeSpan::operator <(const TimeSpan& Other) const
{
	return m_Milliseconds < Other.m_Milliseconds;
}

// ************************************************************************************************

bool TimeSpan::operator >(const TimeSpan& Other) const
{
	return m_Milliseconds > Other.m_Milliseconds;
}

// ************************************************************************************************

bool TimeSpan::operator <=(const TimeSpan& Other) const
{
	return m_Milliseconds <= Other.m_Milliseconds;
}

// ************************************************************************************************

bool TimeSpan::operator >=(const TimeSpan& Other) const
{
	return m_Milliseconds >= Other.m_Milliseconds;
}

// ************************************************************************************************

TimeSpan TimeSpan::operator -(const TimeSpan& Other) const
{
	return TimeSpan(m_Milliseconds - Other.m_Milliseconds);
}

// ************************************************************************************************

TimeSpan TimeSpan::operator +(const TimeSpan& Other) const
{
	return TimeSpan(m_Milliseconds + Other.m_Milliseconds);
}

// ************************************************************************************************

Time::Time()
	: m_UnixTimestamp(0)
{
}

// ************************************************************************************************

Time::Time(uint64 UnixTimestamp)
	: m_UnixTimestamp(UnixTimestamp)
{
}

// ************************************************************************************************

Time Time::Epoch()
{
	return Time(0);
}

// ************************************************************************************************

uint64 Time::GetUnixTimestamp()
{
	return m_UnixTimestamp;
}

// ************************************************************************************************

bool Time::operator ==(const Time& Other) const
{
	return m_UnixTimestamp == Other.m_UnixTimestamp;
}

// ************************************************************************************************

bool Time::operator !=(const Time& Other) const
{
	return m_UnixTimestamp != Other.m_UnixTimestamp;
}

// ************************************************************************************************

bool Time::operator <(const Time& Other) const
{
	return m_UnixTimestamp < Other.m_UnixTimestamp;
}

// ************************************************************************************************

bool Time::operator >(const Time& Other) const
{
	return m_UnixTimestamp > Other.m_UnixTimestamp;
}

// ************************************************************************************************

bool Time::operator <=(const Time& Other) const
{
	return m_UnixTimestamp <= Other.m_UnixTimestamp;
}

// ************************************************************************************************

bool Time::operator >=(const Time& Other) const
{
	return m_UnixTimestamp >= Other.m_UnixTimestamp;
}

// ************************************************************************************************

}; // namespace Ludo