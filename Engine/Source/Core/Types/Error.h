// Copyright 2016 TwinDrills
#pragma once

namespace Ludo {

	/// \brief TODO
	enum class ErrorType
	{
		Success,
		Failure,
	};

	/// \brief TODO
	class Error
	{
	public:
		/// \brief TODO
		Error()
			: mErrorType(ErrorType::Success)
		{
		}

		/// \brief TODO
		Error(const ErrorType Type)
			: mErrorType(Type)
		{
		}

		/// \brief TODO
		inline bool Failed() const
		{
			return !Succeeded();
		}

		/// \brief TODO
		inline bool Succeeded() const
		{
			return (mErrorType == ErrorType::Success);
		}

	protected:
		ErrorType mErrorType;

	};

};