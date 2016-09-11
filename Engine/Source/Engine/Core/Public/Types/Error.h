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

	/// \brief TODO
	enum class ErrorType
	{
		Success,
		Failure,
		NotReentrant,
		AlreadyOpen,
		Corrupt,
		NotSupported,
		AtEnd,

		Count
	};

	/// \brief TODO
	class Error
	{
	protected:
		ErrorType m_ErrorType;

		static const char* s_ErrorStrings[(int)ErrorType::Count];

	public:
		/// \brief TODO
		Error()
			: m_ErrorType(ErrorType::Success)
		{
		}

		/// \brief TODO
		Error(const ErrorType Type)
			: m_ErrorType(Type)
		{
		}

		/// \brief TODO
		inline bool Failed() const
		{
			return !Succeeded();
		}

		/// \brief TODO
		inline ErrorType GetType()
		{
			return m_ErrorType;
		}

		/// \brief TODO
		inline bool Succeeded() const
		{
			return (m_ErrorType == ErrorType::Success);
		}

		const char* ToString() const
		{
			return s_ErrorStrings[(int)m_ErrorType];
		}
	};

};