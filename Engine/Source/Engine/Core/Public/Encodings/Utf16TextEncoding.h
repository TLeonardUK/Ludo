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

#include "Core/Public/Encodings/ITextEncoding.h"

namespace Ludo {

/// \brief Implementation of text encoding using the utf-16 encoding standard.
class Utf16TextEncoding : public ITextEncoding
{
private:
	bool m_BigEndian;

public:
	Utf16TextEncoding(bool bBigEndian);

	virtual bool IsNewLine(const Array<uint8>& Buffer) const override;
	virtual bool IsValid(const Array<uint8>& Buffer) const override;
	virtual Error Encode(const Array<uint8>& InputBuffer, Array<uint8>* OutputBuffer) const override;
	virtual Error Decode(const Array<uint8>& InputBuffer, Array<uint8>* OutputBuffer) const override;
	virtual Array<uint8> GetBom() const override;

};

};