// Copyright 2016 TwinDrills

#include "Platforms/Windows/Helper/Wmi.h"
#include "Platforms/Windows/Helper/Errors.h"
#include "Core/Types/String.h"

#include <comutil.h>
#include <Wbemcli.h>
#include <Wbemidl.h>
#include <Wbemidl.h>

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

WmiQuery::WmiQuery(String WqlQuery)
	: m_Query(WqlQuery)
{
}

Error WmiQuery::Execute(WmiQueryResult* Result)
{
	IWbemLocator* WbemLocator = NULL;
	IWbemServices* ServerConnection = NULL;

	// Create server locator.
	HRESULT ComResult = CoCreateInstance(
		CLSID_WbemLocator,
		0,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator, (LPVOID*)&WbemLocator
	);

	if (FAILED(ComResult))
	{
		AssertLastSystemError("CoCreateInstance");
		return ErrorType::Failure;
	}

	// Connect to the server.
	ComResult = WbemLocator->ConnectServer(
		_bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
		NULL,                    // User name. NULL = current user
		NULL,                    // User password. NULL = current
		0,                       // Locale. NULL indicates current
		NULL,                    // Security flags.
		0,                       // Authority (for example, Kerberos)
		0,                       // Context object 
		&ServerConnection        // pointer to IWbemServices proxy
	);

	if (FAILED(ComResult))
	{
		AssertLastSystemError("ConnectServer");

		WbemLocator->Release();
		return ErrorType::Failure;
	}

	// Set seucirty levels on connection.
	ComResult = CoSetProxyBlanket(
		ServerConnection,            // Indicates the proxy to set
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

		ServerConnection->Release();
		WbemLocator->Release();
		return ErrorType::Failure;
	}

	// Finally our query can be called.
	IEnumWbemClassObject* ResultEnumerator = NULL;
	ComResult = ServerConnection->ExecQuery(
		bstr_t("WQL"),
		bstr_t(m_Query.Data()),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
		NULL,
		&ResultEnumerator);

	if (FAILED(ComResult))
	{
		AssertLastSystemError("ExecQuery");

		ServerConnection->Release();
		WbemLocator->Release();
		return ErrorType::Failure;
	}

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
			ServerConnection->Release();
			WbemLocator->Release();
			return ErrorType::Failure;
		}

		Result->m_ResultObjects.Add(ResultObject);
	}

	ResultEnumerator->Release();
	ServerConnection->Release();
	WbemLocator->Release();

	return ErrorType::Success;
}

}; // namespace Windows
}; // namespace Ludo