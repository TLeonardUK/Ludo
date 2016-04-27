// Copyright 2016 TwinDrills
#pragma once

#include "Core/Types/String.h"
#include "Core/Types/Error.h"

#include <Windows.h>

struct IWbemClassObject;

namespace Ludo {
namespace Windows {

class WmiQuery;

class WmiQueryRow
{
private:
	friend class WmiQueryResult;

	IWbemClassObject* m_Object;

public:
	String GetString(const char* Name);
	int GetInt(const char* Name);

};

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

class WmiQuery
{
private:
	String m_Query;

public:
	WmiQuery(String WqlQuery);

	Error Execute(WmiQueryResult* Result);

};

};
};