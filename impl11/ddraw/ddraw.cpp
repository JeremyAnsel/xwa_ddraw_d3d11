// Copyright (c) 2014 Jérémy Ansel
// Licensed under the MIT license. See LICENSE.txt

#include "common.h"
#include "DeviceResources.h"
#include "DirectDraw.h"

#pragma comment(lib, "dxguid")
#pragma comment(lib, "dxgi")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d2d1")
#pragma comment(lib, "dwrite")
#pragma comment(lib, "Shlwapi")

class LibraryWrapper
{
public:
	LibraryWrapper(LPCSTR lpLibFileName)
	{
		char path[MAX_PATH];
		GetSystemDirectoryA(path, MAX_PATH);
		strcat_s(path, "\\");
		strcat_s(path, lpLibFileName);

		this->_module = LoadLibraryA(path);
	}

	~LibraryWrapper()
	{
		if (this->_module)
		{
			FreeLibrary(this->_module);
		}
	}

	HMODULE _module;
};

static LibraryWrapper ddraw("ddraw.dll");

extern "C" __declspec(naked) void AcquireDDThreadLock()
{
	static void(*ddraw_proc)() = (void(*)())GetProcAddress(ddraw._module, "AcquireDDThreadLock");

	__asm jmp ddraw_proc;
}

extern "C" __declspec(naked) void CompleteCreateSysmemSurface()
{
	static void(*ddraw_proc)() = (void(*)())GetProcAddress(ddraw._module, "CompleteCreateSysmemSurface");

	__asm jmp ddraw_proc;
}

extern "C" __declspec(naked) void D3DParseUnknownCommand()
{
	static void(*ddraw_proc)() = (void(*)())GetProcAddress(ddraw._module, "D3DParseUnknownCommand");

	__asm jmp ddraw_proc;
}

extern "C" __declspec(naked) void DDGetAttachedSurfaceLcl()
{
	static void(*ddraw_proc)() = (void(*)())GetProcAddress(ddraw._module, "DDGetAttachedSurfaceLcl");

	__asm jmp ddraw_proc;
}

extern "C" __declspec(naked) void DDInternalLock()
{
	static void(*ddraw_proc)() = (void(*)())GetProcAddress(ddraw._module, "DDInternalLock");

	__asm jmp ddraw_proc;
}

extern "C" __declspec(naked) void DDInternalUnlock()
{
	static void(*ddraw_proc)() = (void(*)())GetProcAddress(ddraw._module, "DDInternalUnlock");

	__asm jmp ddraw_proc;
}

extern "C" __declspec(naked) void DSoundHelp()
{
	static void(*ddraw_proc)() = (void(*)())GetProcAddress(ddraw._module, "DSoundHelp");

	__asm jmp ddraw_proc;
}

extern "C" HRESULT WINAPI DirectDrawCreate(
	_In_   GUID FAR *lpGUID,
	_Out_  LPDIRECTDRAW FAR *lplpDD,
	_In_   IUnknown FAR *pUnkOuter
	)
{
	//static decltype(DirectDrawCreate)* ddraw_proc = (decltype(DirectDrawCreate)*)GetProcAddress(ddraw._module, "DirectDrawCreate");
	//return ddraw_proc(lpGUID, lplpDD, pUnkOuter);

#if LOGGER
	std::ostringstream str;
	str << __FUNCTION__;
	str << " " << (lpGUID == nullptr ? "NULL" : tostr_IID(*lpGUID));
	LogText(str.str());
#endif

	if (lplpDD == nullptr)
	{
#if LOGGER
		str.str("\tDDERR_INVALIDPARAMS");
		LogText(str.str());
#endif

		return DDERR_INVALIDPARAMS;
	}

	DeviceResources* deviceResources = new DeviceResources();

	if (FAILED(deviceResources->Initialize()))
	{
		delete deviceResources;

#if LOGGER
		str.str("\tDDERR_NODIRECTDRAWHW");
		LogText(str.str());
#endif

		return DDERR_NODIRECTDRAWHW;
	}

	*lplpDD = new DirectDraw(deviceResources);

#if LOGGER
	str.str("");
	str << "\tDD_OK\t" << *lplpDD;
	LogText(str.str());
#endif

	return DD_OK;
}

extern "C" HRESULT WINAPI DirectDrawCreateClipper(
	_In_   DWORD dwFlags,
	_Out_  LPDIRECTDRAWCLIPPER FAR *lplpDDClipper,
	_In_   IUnknown FAR *pUnkOuter
	)
{
	static decltype(DirectDrawCreateClipper)* ddraw_proc = (decltype(DirectDrawCreateClipper)*)GetProcAddress(ddraw._module, "DirectDrawCreateClipper");

#if LOGGER
	std::ostringstream str;
	str << __FUNCTION__;
	LogText(str.str());
#endif

	return ddraw_proc(dwFlags, lplpDDClipper, pUnkOuter);
}

extern "C" HRESULT WINAPI DirectDrawCreateEx(
	_In_   GUID FAR *lpGUID,
	_Out_  LPVOID *lplpDD,
	_In_   REFIID iid,
	_In_   IUnknown FAR *pUnkOuter
	)
{
	static decltype(DirectDrawCreateEx)* ddraw_proc = (decltype(DirectDrawCreateEx)*)GetProcAddress(ddraw._module, "DirectDrawCreateEx");

#if LOGGER
	std::ostringstream str;
	str << __FUNCTION__;
	str << " " << (lpGUID == nullptr ? "NULL" : tostr_IID(*lpGUID));
	LogText(str.str());
#endif

	return ddraw_proc(lpGUID, lplpDD, iid, pUnkOuter);
}

extern "C" HRESULT WINAPI DirectDrawEnumerateA(
	_In_  LPDDENUMCALLBACKA lpCallback,
	_In_  LPVOID lpContext
	)
{
	static decltype(DirectDrawEnumerateA)* ddraw_proc = (decltype(DirectDrawEnumerateA)*)GetProcAddress(ddraw._module, "DirectDrawEnumerateA");

#if LOGGER
	std::ostringstream str;
	str << __FUNCTION__;
	LogText(str.str());
#endif

	return ddraw_proc(lpCallback, lpContext);
}

extern "C" HRESULT WINAPI DirectDrawEnumerateExA(
	_In_  LPDDENUMCALLBACKEXA lpCallback,
	_In_  LPVOID lpContext,
	_In_  DWORD dwFlags
	)
{
	//static decltype(DirectDrawEnumerateExA)* ddraw_proc = (decltype(DirectDrawEnumerateExA)*)GetProcAddress(ddraw._module, "DirectDrawEnumerateExA");
	//return ddraw_proc(lpCallback, lpContext, dwFlags);

#if LOGGER
	std::ostringstream str;
	str << __FUNCTION__;
	LogText(str.str());
#endif

	if (lpCallback == nullptr)
	{
		return DDERR_INVALIDPARAMS;
	}

	ComPtr<IDXGIFactory> factory;

	if (FAILED(CreateDXGIFactory(IID_PPV_ARGS(&factory))))
	{
		return S_OK;
	}

	ComPtr<IDXGIAdapter> adapter;

	for (int i = 0; factory->EnumAdapters(i, &adapter) == S_OK; i++)
	{
		DXGI_ADAPTER_DESC adapterDesc;

		if (FAILED(adapter->GetDesc(&adapterDesc)))
		{
			continue;
		}

		std::string adapterStr = wchar_tostring(adapterDesc.Description);

		ComPtr<IDXGIOutput> output;

		for (int j = 0; adapter->EnumOutputs(j, &output) == S_OK; j++)
		{
			DXGI_OUTPUT_DESC outputDesc;

			if (FAILED(output->GetDesc(&outputDesc)))
			{
				continue;
			}

			std::string outputStr = wchar_tostring(outputDesc.DeviceName);

#if LOGGER
			str.str("");
			str << "\t" << outputStr << "\t" << adapterStr;
			LogText(str.str());
#endif

			if (lpCallback(nullptr, (LPSTR)adapterStr.c_str(), (LPSTR)outputStr.c_str(), lpContext, nullptr) != DDENUMRET_OK)
			{
				return DD_OK;
			}
		}
	}

	return DD_OK;
}

extern "C" HRESULT WINAPI DirectDrawEnumerateExW(
	_In_  LPDDENUMCALLBACKEXW lpCallback,
	_In_  LPVOID lpContext,
	_In_  DWORD dwFlags
	)
{
	static decltype(DirectDrawEnumerateExW)* ddraw_proc = (decltype(DirectDrawEnumerateExW)*)GetProcAddress(ddraw._module, "DirectDrawEnumerateExW");

#if LOGGER
	std::ostringstream str;
	str << __FUNCTION__;
	LogText(str.str());
#endif

	return ddraw_proc(lpCallback, lpContext, dwFlags);
}

extern "C" HRESULT WINAPI DirectDrawEnumerateW(
	_In_  LPDDENUMCALLBACKW lpCallback,
	_In_  LPVOID lpContext
	)
{
	static decltype(DirectDrawEnumerateW)* ddraw_proc = (decltype(DirectDrawEnumerateW)*)GetProcAddress(ddraw._module, "DirectDrawEnumerateW");

#if LOGGER
	std::ostringstream str;
	str << __FUNCTION__;
	LogText(str.str());
#endif

	return ddraw_proc(lpCallback, lpContext);
}

extern "C" __declspec(naked) void GetDDSurfaceLocal()
{
	static void(*ddraw_proc)() = (void(*)())GetProcAddress(ddraw._module, "GetDDSurfaceLocal");

	__asm jmp ddraw_proc;
}

extern "C" __declspec(naked) void GetOLEThunkData()
{
	static void(*ddraw_proc)() = (void(*)())GetProcAddress(ddraw._module, "GetOLEThunkData");

	__asm jmp ddraw_proc;
}

extern "C" __declspec(naked) void GetSurfaceFromDC()
{
	static void(*ddraw_proc)() = (void(*)())GetProcAddress(ddraw._module, "GetSurfaceFromDC");

	__asm jmp ddraw_proc;
}

extern "C" __declspec(naked) void RegisterSpecialCase()
{
	static void(*ddraw_proc)() = (void(*)())GetProcAddress(ddraw._module, "RegisterSpecialCase");

	__asm jmp ddraw_proc;
}

extern "C" __declspec(naked) void ReleaseDDThreadLock()
{
	static void(*ddraw_proc)() = (void(*)())GetProcAddress(ddraw._module, "ReleaseDDThreadLock");

	__asm jmp ddraw_proc;
}

extern "C" __declspec(naked) void SetAppCompatData()
{
	static void(*ddraw_proc)() = (void(*)())GetProcAddress(ddraw._module, "SetAppCompatData");

	__asm jmp ddraw_proc;
}
