// Copyright 2016 TwinDrills
#pragma once

#include "Core/Types/String.h"

namespace Ludo {

/// \brief TODO
class Thread 
{
private:
	class Impl;
	Impl* m_Impl;

public:

	/// \brief TODO
	typedef std::function<void (Thread* Self)> EntryPoint;

	/// \brief TODO
	Thread();
	Thread(String Name, EntryPoint Point);

	/// \brief TODO
	~Thread();

	/// \brief TODO
	void Start();

	/// \brief TODO
	void Abort();

	/// \brief TODO
	void Join();

	/// \brief TODO
	bool IsRunning();

	/// \brief TODO
	bool IsAborted();

	static Thread* Self();
	static Thread* Main();

};

};