// Copyright 2016 TwinDrills

#include "Core/Testing/Test.h"
#include "Core/Types/String.h"

#include "Core/Testing/Types/Test_String.h"
#include "Core/Testing/Containers/Test_Array.h"
#include "Core/Testing/Math/Test_Math.h"
#include "Core/Testing/Memory/Test_Memory.h"
#include "Core/Testing/Types/Test_String.h"
#include "Core/Testing/Platform/Test_Path.h"

namespace Ludo
{

// Initialzie all the core tests.
TestString		gTestString;
TestArray		gTestArray;
TestMath		gTestMath;
TestMemory		gTestMemory;
TestPath		gTestPath;

// Global test list.
Linked<Test>* Test::sGlobalTestsList = nullptr;

Test::Test()
{
	Link(sGlobalTestsList);
}

Test::~Test()
{
	Unlink(sGlobalTestsList);
}

void Test::RunAll()
{
	Log(LogTests, LogInfo, "Running unit tests:");
	for (auto test = sGlobalTestsList; test != nullptr; test = test->Next())
	{
		StringArgumentList Args;
		Args.Add("Name", (*test)->Name());

		Log(LogTests, LogInfo, "\tRunning: {Name}", Args);

		(*test)->Run();
	}
}

}; // namespace Ludo