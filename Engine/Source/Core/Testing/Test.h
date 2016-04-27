// Copyright 2016 TwinDrills
#pragma once

#include "Core/Containers/Linked.h"

namespace Ludo {

/// \brief TODO
class Test : 
	public Linked<Test>
{
private:
	static Linked<Test>* sGlobalTestsList;

public:
	Test();
	virtual ~Test();

	/// \brief TODO
	virtual const char* Name() const = 0;

	/// \brief TODO
	virtual void Run() const = 0;

	/// \brief TODO
	static void RunAll();

};

};