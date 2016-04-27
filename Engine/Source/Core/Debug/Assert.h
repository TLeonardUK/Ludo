// Copyright 2016 TwinDrills
#pragma once

namespace Ludo {

struct StringArgumentList;

void AssertFailed(const char* Message, const char* File, int Line);
void AssertFailed(const char* Message, const char* File, int Line, const char* Format);
void AssertFailed(const char* Message, const char* File, int Line, const char* Format, StringArgumentList& Args);

}; // namespace Ludo

/// \brief TODO
#define Assert(cond) \
	if (!(cond)) \
	{ \
		Ludo::AssertFailed(#cond, __FILE__, __LINE__); \
	}

#define AssertMsg(cond, format, ...) \
	if (!(cond)) \
	{ \
		Ludo::AssertFailed(#cond, __FILE__, __LINE__, format, ##__VA_ARGS__); \
	}

#define ConstantAssert() \
	Ludo::AssertFailed("ConstantAssert", __FILE__, __LINE__);

#define ConstantAssertMsg(format, ...) \
	Ludo::AssertFailed("ConstantAssert", __FILE__, __LINE__, format, ##__VA_ARGS__); 

