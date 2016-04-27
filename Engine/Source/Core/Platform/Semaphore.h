// Copyright 2016 TwinDrills
#pragma once

namespace Ludo {

/// \brief TODO
class Semaphore
{
private:
	class Impl;
	Impl* m_Impl;

public:

	/// \brief TODO
	Semaphore();

	/// \brief TODO
	~Semaphore();

	/// \brief TODO
	void Signal();

	/// \brief TODO
	void Wait();

};

};