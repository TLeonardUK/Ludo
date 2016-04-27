// Copyright 2016 TwinDrills
#pragma once

namespace Ludo {

/// \brief TODO
template <typename Type>
class Tls
{
private:
	class Impl;
	Impl* m_Impl;

public:

	/// \brief TODO
	Tls();

	/// \brief TODO
	~Tls();

	/// \brief TODO
	Type Get();

	/// \brief TODO
	void Set(Type T);

};

};