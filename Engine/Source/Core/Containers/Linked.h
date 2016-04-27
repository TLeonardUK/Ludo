// Copyright 2016 TwinDrills
#pragma once

namespace Ludo {

/// \brief TODO
template 
<
	typename ElementType
>
class Linked
{
public:

	/// \brief TODO
	Linked()
		: m_Next(nullptr)
	{
	}

	/// \brief TODO
	void Link(Linked<ElementType>* & other)
	{
		this->m_Next = other;
		other = this;
	}

	/// \brief TODO
	void Unlink(Linked<ElementType>*& other)
	{
		other = this->m_Next;
	}

	/// \brief TODO
	Linked<ElementType>* Next() const
	{
		return m_Next;
	}

	/// \brief TODO
	ElementType& operator*()
	{
		return static_cast<ElementType*>(this);
	}

	/// \brief TODO
	ElementType* operator->()
	{
		return static_cast<ElementType*>(this);
	}

protected:
	Linked<ElementType>* m_Next;

};

};