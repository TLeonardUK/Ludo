// Copyright 2016 TwinDrills
#pragma once

#include <cstdint>

namespace Ludo {

/// \brief TODO
class TimeSpan
{
private:
	uint64_t m_Milliseconds;

public:

	TimeSpan(uint64_t TotalMilliseconds)
		: m_Milliseconds(TotalMilliseconds)
	{
	}

	uint64_t GetTotalMilliseconds() const
	{
		return m_Milliseconds;
	}

	uint64_t GetTotalSeconds() const
	{
		return GetTotalMilliseconds() / 1000;
	}

	uint64_t GetTotalMinutes() const
	{
		return GetTotalSeconds() / 60;
	}

	uint64_t GetTotalHours() const
	{
		return GetTotalMinutes() / 60;
	}

	uint64_t GetTotalDays() const
	{
		return GetTotalHours() / 24;
	}


	uint64_t GetMilliseconds() const
	{
		return m_Milliseconds % 1000;
	}

	uint64_t GetSeconds() const
	{
		return (m_Milliseconds / 1000) % 60;
	}

	uint64_t GetMinutes() const
	{
		return ((m_Milliseconds / 1000) / 60) % 60;
	}

	uint64_t GetHours() const
	{
		return (((m_Milliseconds / 1000) / 60) / 60) % 24;
	}

	uint64_t GetDays() const
	{
		return (((m_Milliseconds / 1000) / 60) / 60) / 24;
	}

};

/// \brief TODO
class Time
{
public:

	/// \brief TODO
	static Time Now();

	/// \brief TODO
	static TimeSpan AppDuration();

};

};