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

namespace Ludo {

/* 
@brief Interface that hides a pointer to a private implementation of a derived class.

@description This is primarily used for things like platform-specific objects where you do 
no want to pull in all the dependencies the implementation requires to everywhere
that uses it. 

This can be done cleaner in a more tranditional pimpl implementation, using virtual
methods and wrappers, but this results in uneccecssary virtual function calls, 
where as this method can be resolved at compile time without indirection.

*/
class IPimpl
{
protected:
	void* m_Impl;

protected:

	/*
	@brief Constructor.
	*/
	IPimpl()
		: m_Impl(nullptr)
	{
	}

	/* 
	@brief		Helper function, returns the implementation pointer reintepreted to the template type.
	@returns	Implementation reinterpreted to the template type.	

	@tparam	ResultType		
	Type to cast implementation pointer to.
	*/
	template <typename ResultType>
	ResultType* GetImpl()
	{
		Assert(m_Impl != nullptr);
		return reinterpret_cast<ResultType*>(m_Impl);
	}

	template <typename ResultType>
	const ResultType* GetImpl() const
	{
		Assert(m_Impl != nullptr);
		return reinterpret_cast<ResultType*>(m_Impl);
	}

	/*
	@brief TODO
	*/
	template <typename ArenaType, typename ImplType>
	ImplType* CreateImpl()
	{
		m_Impl = LD_ARENA_NEW(ArenaType, ImplType);
		return GetImpl<ImplType>();
	}

	/*
	@brief TODO
	*/
	template <typename ArenaType, typename ImplType>
	void DestroyImpl()
	{
		LD_ARENA_DELETE(ArenaType, m_Impl);
		m_Impl = nullptr;
	}

};

}; // namespace Ludo