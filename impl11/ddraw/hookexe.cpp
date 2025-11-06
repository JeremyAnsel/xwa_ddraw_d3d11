#include "targetver.h"
#include <Windows.h>
#include "hookexe.h"

HWND g_hookexe = NULL;

enum ExeDataType
{
	ExeDataType_None,
	ExeDataType_ShowMessage,
	ExeDataType_LoadTexture_Result,
	ExeDataType_LoadTexture_Handle,
	ExeDataType_LoadTexture_Release,
};

void ExeShowMessage(const std::string& message)
{
	if (!g_hookexe)
	{
		return;
	}

	COPYDATASTRUCT cds{};
	cds.dwData = ExeDataType_ShowMessage;
	cds.cbData = message.length() + 1;
	cds.lpData = (void*)message.data();

	SendMessage(g_hookexe, WM_COPYDATA, NULL, (LPARAM)&cds);
}

HRESULT ExeLoadTexture(const std::wstring& name, HANDLE* handle)
{
	*handle = 0;

	if (!g_hookexe)
	{
		return E_ABORT;
	}

	COPYDATASTRUCT cds{};
	cds.dwData = ExeDataType_LoadTexture_Result;
	cds.cbData = (name.length() + 1) * sizeof(wchar_t);
	cds.lpData = (void*)name.data();
	HRESULT hr = (HRESULT)SendMessage(g_hookexe, WM_COPYDATA, NULL, (LPARAM)&cds);

	if (SUCCEEDED(hr))
	{
		cds.dwData = ExeDataType_LoadTexture_Handle;
		cds.cbData = 0;
		cds.lpData = nullptr;
		*handle = (HANDLE)SendMessage(g_hookexe, WM_COPYDATA, NULL, (LPARAM)&cds);
	}

	return hr;
}

void ExeLoadTextureRelease()
{
	if (!g_hookexe)
	{
		return;
	}

	COPYDATASTRUCT cds{};
	cds.dwData = ExeDataType_LoadTexture_Release;
	cds.cbData = 0;
	cds.lpData = nullptr;
	SendMessage(g_hookexe, WM_COPYDATA, NULL, (LPARAM)&cds);
}
