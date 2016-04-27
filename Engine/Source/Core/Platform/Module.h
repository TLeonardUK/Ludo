// Copyright 2016 TwinDrills
#pragma once

#include "Core/Types/String.h"
#include "Core/Platform/Path.h"

namespace Ludo {

/// \brief TODO
class Module
{
private:
	class Impl;
	Impl* m_Impl;

public:

	/// \brief TODO
	Module(const Path& path);

	/// \brief TODO
	~Module();

	/// \brief TODO
	bool Open();

	/// \brief TODO
	bool IsOpen();

	/// \brief TODO
	void Close();

	/// \brief TODO
	template <typename Signature>
	Signature GetFunction(String Name);

	/*
	typedef std::function<bool(EngineInterface& Output)> CreateGameInterfaceFunction_t; 

	Module Own = Module::Self();
	CreateGameInterfaceFunction_t CreateFunction = Own.GetFunction<CreateGameInterfaceFunction_t>("CreateGameInterface");
	GameInterface* GameInterace = CreateFunction();
	*/

	/// \brief TODO
	static Module Self();

};

};