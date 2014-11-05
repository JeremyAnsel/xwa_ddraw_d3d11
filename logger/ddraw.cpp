#include "common.h"
#include "DirectDraw.h"

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
	static decltype(DirectDrawCreate)* ddraw_proc = (decltype(DirectDrawCreate)*)GetProcAddress(ddraw._module, "DirectDrawCreate");

	std::ostringstream str;
	str << __FUNCTION__;
	str << " " << (lpGUID == nullptr ? "NULL" : tostr_IID(*lpGUID));

	LogText(str.str());

	HRESULT hr = ddraw_proc(lpGUID, lplpDD, pUnkOuter);

	if (SUCCEEDED(hr))
	{
		*lplpDD = GetOrCreateWrapperT(DirectDraw, *lplpDD);
	}

	return hr;
}

extern "C" HRESULT WINAPI DirectDrawCreateClipper(
	_In_   DWORD dwFlags,
	_Out_  LPDIRECTDRAWCLIPPER FAR *lplpDDClipper,
	_In_   IUnknown FAR *pUnkOuter
	)
{
	static decltype(DirectDrawCreateClipper)* ddraw_proc = (decltype(DirectDrawCreateClipper)*)GetProcAddress(ddraw._module, "DirectDrawCreateClipper");

	LogText(__FUNCTION__);

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

	std::ostringstream str;
	str << __FUNCTION__;
	str << " " << tostr_IID(iid);

	LogText(str.str());

	return ddraw_proc(lpGUID, lplpDD, iid, pUnkOuter);
}

extern "C" HRESULT WINAPI DirectDrawEnumerateA(
	_In_  LPDDENUMCALLBACKA lpCallback,
	_In_  LPVOID lpContext
	)
{
	static decltype(DirectDrawEnumerateA)* ddraw_proc = (decltype(DirectDrawEnumerateA)*)GetProcAddress(ddraw._module, "DirectDrawEnumerateA");

	LogText(__FUNCTION__);

	return ddraw_proc(lpCallback, lpContext);
}

static LPDDENUMCALLBACKEXA s_lpCallback;

static BOOL CALLBACK EnumerateExACallback(GUID* lpGUID, LPSTR lpDriverDescription, LPSTR lpDriverName, LPVOID lpContext, HMONITOR hm)
{
	std::ostringstream str;
	str << "\t" << lpDriverName << "\t" << lpDriverDescription;
	str << "\t" << lpGUID << "\t" << hm;

	LogText(str.str());

	return s_lpCallback(lpGUID, lpDriverDescription, lpDriverName, lpContext, hm);
}

extern "C" HRESULT WINAPI DirectDrawEnumerateExA(
	_In_  LPDDENUMCALLBACKEXA lpCallback,
	_In_  LPVOID lpContext,
	_In_  DWORD dwFlags
	)
{
	static decltype(DirectDrawEnumerateExA)* ddraw_proc = (decltype(DirectDrawEnumerateExA)*)GetProcAddress(ddraw._module, "DirectDrawEnumerateExA");

	LogText(__FUNCTION__);

	s_lpCallback = lpCallback;

	return ddraw_proc(EnumerateExACallback, lpContext, dwFlags);
}

extern "C" HRESULT WINAPI DirectDrawEnumerateExW(
	_In_  LPDDENUMCALLBACKEXW lpCallback,
	_In_  LPVOID lpContext,
	_In_  DWORD dwFlags
	)
{
	static decltype(DirectDrawEnumerateExW)* ddraw_proc = (decltype(DirectDrawEnumerateExW)*)GetProcAddress(ddraw._module, "DirectDrawEnumerateExW");

	LogText(__FUNCTION__);

	return ddraw_proc(lpCallback, lpContext, dwFlags);
}

extern "C" HRESULT WINAPI DirectDrawEnumerateW(
	_In_  LPDDENUMCALLBACKW lpCallback,
	_In_  LPVOID lpContext
	)
{
	static decltype(DirectDrawEnumerateW)* ddraw_proc = (decltype(DirectDrawEnumerateW)*)GetProcAddress(ddraw._module, "DirectDrawEnumerateW");

	LogText(__FUNCTION__);

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
