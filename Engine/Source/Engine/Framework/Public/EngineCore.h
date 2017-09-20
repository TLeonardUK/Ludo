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

#include "Core/Public/Core.h"

#define LD_EXPORT_ENGINE_CORE(ClassName)					\
	Error CreateEngineCore(EngineCore** Result)				\
	{														\
		static bool bReEntranceGuard = false;				\
		static ClassName Core;								\
		if (bReEntranceGuard)								\
		{													\
			return Error(ErrorType::NotReentrant);			\
		}													\
		*Result = &Core;									\
		bReEntranceGuard = true;							\
		return Error(ErrorType::Success);					\
	}														\
	Error DeleteEngineCore(EngineCore** Result)				\
	{														\
		static bool bReEntranceGuard = false;				\
		if (bReEntranceGuard)								\
		{													\
			return Error(ErrorType::NotReentrant);			\
		}													\
		*Result = nullptr;									\
		bReEntranceGuard = true;							\
		return Error(ErrorType::Success);					\
	}														\

namespace Ludo {

class GameEngine;

/// \brief TODO
class EngineCore
{
public:

	/// \brief TODO
	virtual Error Init(GameEngine* Engine) = 0;

	/// \brief TODO
	virtual Error Term() = 0;

	/// \brief TODO
	virtual int Run() = 0;

	/// \brief TODO
	virtual bool UseFastInit();

};

}; // namespace Ludo