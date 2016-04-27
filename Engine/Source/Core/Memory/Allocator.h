// Copyright 2016 TwinDrills
#pragma once

namespace Ludo {

/// \brief TODO
class IAllocator 
{
public:

	/// \brief TODO
	static const int DefaultAlignment = -1;

	/// \brief TODO
	virtual void* Alloc(const int Size, const int Alignment = DefaultAlignment) = 0;

	/// \brief TODO
	virtual void* Realloc(void* Ptr, const int Size) = 0;

	/// \brief TODO
	virtual void Free(void* Ptr) = 0;

	template <typename BaseType, typename... ConstructorTypes>
	BaseType* New(ConstructorTypes...)
	{
		return new BaseType(ConstructorTypes...);
	}
	
	template <typename BaseType>
	BaseType* NewArray(int Count)
	{
		return new BaseType[Count];
	}

	template <typename BaseType>
	void Delete(BaseType* Data)
	{
		delete Data;
	}
	
	template <typename BaseType>
	void DeleteArray(BaseType* Data)
	{
		delete[] Data;
	}

};

};