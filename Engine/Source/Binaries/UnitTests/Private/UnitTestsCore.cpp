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

#include "UnitTests/Private/UnitTestsCore.h"

namespace Ludo {

LD_EXPORT_ENGINE_CORE(UnitTestsCore);

Error UnitTestsCore::Init(GameEngine* Engine)
{
	UNUSED_PARAMETER(Engine);
	return Error(ErrorType::Success);
}

Error UnitTestsCore::Term()
{
	return Error(ErrorType::Success);
}

int UnitTestsCore::Run()
{
	String ExePath = Environment::GetSpecialPath(Environment::SpecialPath::ExecutableFile).ToString();

	Log(LogTests, LogInfo, "");
	Log(LogTests, LogInfo, "Starting GoogleTest ...");
	Log(LogTests, LogInfo, "");
	printf("\n");

	int argc = 1;
	char* argv[1];

	char ExeBuffer[2048];
	argv[0] = ExeBuffer;

	sprintf(ExeBuffer, "%s", ExePath.Data()); 

	testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}

bool UnitTestsCore::UseFastInit()
{
	return true;
}

}; // namespace Ludo