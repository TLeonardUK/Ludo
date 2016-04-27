// Copyright 2016 TwinDrills
#pragma once

namespace Ludo {

/// \brief TODO
class IMemoryArena
{
public:

	/// \brief TODO
	virtual const char* GetName() const = 0;

	/// \brief TODO
	virtual const char* GetDescription() const = 0;

	/// \brief TODO
	virtual void GetDefaultState(bool& bResizable, int& DefaultSize) const = 0;

	/// \brief TODO
	virtual void* Allocate(const int Count) = 0;

	/// \brief TODO
	virtual void* Reallocate(void* Ptr, const int Count) = 0;

	/// \brief TODO
	virtual void Free(void* Ptr) = 0;

};

};