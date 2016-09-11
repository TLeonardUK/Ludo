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

#include "PCH.h"

#include "Core-Windows/Private/Platform/Helper/Wmi.h"
#include "Core-Windows/Private/Platform/Helper/Errors.h"

using namespace Ludo;
using namespace Ludo::Windows;

namespace Ludo {
namespace Windows {

String WmiQueryRow::GetString(const char* Name)
{
	VARIANT Variant;

	WCHAR  WideName[128];
	if (!MultiByteToWideChar(0, 0, Name, -1, WideName, 128))
	{
		AssertLastSystemError("MultiByteToWideChar");
		return "";
	}

	HRESULT ComResult = m_Object->Get(WideName, 0, &Variant, 0, 0);
	if (FAILED(ComResult))
	{
		AssertLastSystemError("IWbemClassObject->Get");
		return "";
	}
	
	char MBResult[128];
	if (!WideCharToMultiByte(0, 0, Variant.bstrVal, -1,  MBResult, 128, 0, 0))
	{
		AssertLastSystemError("MultiByteToWideChar");
		return "";
	}
	
	return MBResult;
}

int WmiQueryRow::GetInt(const char* Name)
{
	VARIANT Variant;

	WCHAR  WideName[128];
	if (!MultiByteToWideChar(0, 0, Name, -1, WideName, 128))
	{
		AssertLastSystemError("MultiByteToWideChar");
		return 0;
	}

	HRESULT ComResult = m_Object->Get(WideName, 0, &Variant, 0, 0);
	if (FAILED(ComResult))
	{
		AssertLastSystemError("IWbemClassObject->Get");
		return 0;
	}

	return Variant.intVal;
}

uint64 WmiQueryRow::GetLong(const char* Name)
{
	VARIANT Variant;

	WCHAR  WideName[128];
	if (!MultiByteToWideChar(0, 0, Name, -1, WideName, 128))
	{
		AssertLastSystemError("MultiByteToWideChar");
		return 0;
	}

	HRESULT ComResult = m_Object->Get(WideName, 0, &Variant, 0, 0);
	if (FAILED(ComResult))
	{
		AssertLastSystemError("IWbemClassObject->Get");
		return 0;
	}

	return Variant.ullVal;
}

WmiQueryResult::~WmiQueryResult()
{
	for (IWbemClassObject* Object : m_ResultObjects)
	{
		Object->Release();
	}
	m_ResultObjects.Empty();
}

int WmiQueryResult::RowCount()
{
	return m_ResultObjects.Length();
}

WmiQueryRow WmiQueryResult::GetRow(int Index)
{
	WmiQueryRow Row;
	Row.m_Object = m_ResultObjects[Index];
	return Row;
}

WmiConnection::WmiConnection()
	: m_WbemLocator(nullptr)
	, m_ServerConnection(nullptr)
	, m_Connected(false)
{
}

WmiConnection::~WmiConnection()
{
	if (m_Connected)
	{
		Disconnect();
	}
}

Error WmiConnection::Connect()
{
	// Create server locator.
	HRESULT ComResult = CoCreateInstance(
		CLSID_WbemLocator,
		0,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator, (LPVOID*)&m_WbemLocator
		);

	if (FAILED(ComResult))
	{
		AssertLastSystemError("CoCreateInstance");
		Disconnect();
		return ErrorType::Failure;
	}

	// Connect to the server.
	ComResult = m_WbemLocator->ConnectServer(
		_bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
		NULL,                    // User name. NULL = current user
		NULL,                    // User password. NULL = current
		0,                       // Locale. NULL indicates current
		NULL,                    // Security flags.
		0,                       // Authority (for example, Kerberos)
		0,                       // Context object 
		&m_ServerConnection        // pointer to IWbemServices proxy
		);

	if (FAILED(ComResult))
	{
		AssertLastSystemError("ConnectServer");
		Disconnect();
		return ErrorType::Failure;
	}

	// Set seucirty levels on connection.
	ComResult = CoSetProxyBlanket(
		m_ServerConnection,            // Indicates the proxy to set
		RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
		RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
		NULL,                        // Server principal name 
		RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
		RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
		NULL,                        // client identity
		EOAC_NONE                    // proxy capabilities 
	);

	if (FAILED(ComResult))
	{
		AssertLastSystemError("CoSetProxyBlanket");
		Disconnect();
		return ErrorType::Failure;
	}

	m_Connected = true;

	return ErrorType::Success;
}

void WmiConnection::Disconnect()
{
	if (m_ServerConnection != nullptr)
	{
		m_ServerConnection->Release();
		m_ServerConnection = nullptr;
	}
	if (m_WbemLocator != nullptr)
	{
		m_WbemLocator->Release();
		m_WbemLocator = nullptr;
	}

	m_Connected = false;
}

bool WmiConnection::IsConnected()
{
	return m_Connected;
}

WmiQuery::WmiQuery(WmiConnection* Connection, String WqlQuery)
	: m_Query(WqlQuery)
	, m_Connection(Connection)
{
}

Error WmiQuery::Execute(WmiQueryResult* Result)
{
	//Time StartTime = Time::PerfNow();

	// Finally our query can be called.
	IEnumWbemClassObject* ResultEnumerator = NULL;
	HRESULT ComResult = m_Connection->m_ServerConnection->ExecQuery(
		bstr_t("WQL"),
		bstr_t(m_Query.Data()),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
		NULL,
		&ResultEnumerator);

	if (FAILED(ComResult))
	{
		AssertLastSystemError("ExecQuery");
		return ErrorType::Failure;
	}

	//TimeSpan ElapsedTime = Time::PerfNow() - StartTime;

	// Parse our data.
	IWbemClassObject* ResultObject = NULL;
	ULONG ReturnValue = 0;

	Result->m_ResultObjects.Empty();

	while (ResultEnumerator)
	{
		ComResult = ResultEnumerator->Next(WBEM_INFINITE, 1, &ResultObject, &ReturnValue);
		if (0 == ReturnValue)
		{
			break;
		}

		if (FAILED(ComResult))
		{
			AssertLastSystemError("IEnumWbemClassObject->Next");
			ResultEnumerator->Release();
			return ErrorType::Failure;
		}

		Result->m_ResultObjects.Add(ResultObject);
	}

	ResultEnumerator->Release();
	return ErrorType::Success;
}

}; // namespace Windows
}; // namespace Ludo