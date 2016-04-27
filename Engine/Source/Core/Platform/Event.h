// Copyright 2016 TwinDrills
#pragma once

namespace Ludo {

/// \brief TODO
class Event
{
private:
	class Impl;
	Impl* m_Impl;

public:

	/// \brief TODO
	void Reset();

	/// \brief TODO
	void Set();

	/// \brief TODO
	void Wait();

};

};