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

#include "Core/Public/Types/String.h"
#include "Core/Public/Types/Array.h"
#include "Core/Public/Types/Error.h"

#include <Windows.h>
#include <Wbemcli.h>

struct IWbemClassObject;

namespace Ludo {
namespace Windows {

// ************************************************************************************************

class WmiQuery;

class WmiQueryRow
{
private:
	friend class WmiQueryResult;

	IWbemClassObject* m_Object;

public:
	String GetString(const char* Name);
	int GetInt(const char* Name);
	uint64 GetLong(const char* Name);

};

// ************************************************************************************************

class WmiQueryResult
{
private:
	friend class WmiQuery;

	Array<IWbemClassObject*> m_ResultObjects;

public:
	~WmiQueryResult();

	int RowCount();
	WmiQueryRow GetRow(int Index);

};

// ************************************************************************************************

class WmiConnection
{
private:
	IWbemLocator* m_WbemLocator;
	IWbemServices* m_ServerConnection;

	bool m_Connected;

	friend class WmiQuery;

public:
	WmiConnection();
	~WmiConnection();

	Error Connect();
	void Disconnect();

	bool IsConnected();

};

// ************************************************************************************************

class WmiQuery
{
private:
	WmiConnection* m_Connection;
	String m_Query;

public:
	WmiQuery(WmiConnection* Connection, String WqlQuery);

	Error Execute(WmiQueryResult* Result);

};

// ************************************************************************************************

};
};