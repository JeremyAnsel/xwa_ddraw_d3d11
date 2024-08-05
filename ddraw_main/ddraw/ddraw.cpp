// Copyright (c) 2014 Jérémy Ansel
// Licensed under the MIT license. See LICENSE.txt

#include "targetver.h"
#include "config.h"

#define WIN32_LEAN_AND_MEAN
#define STRICT
#include <windows.h>

class LibraryWrapper
{
public:
	LibraryWrapper(LPCSTR lpLibFileName)
	{
		this->_module = LoadLibraryA(lpLibFileName);
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

static LibraryWrapper ddraw(g_config.DDrawTypeDllPath.c_str());

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

extern "C" __declspec(naked) void DirectDrawCreate()
{
	static void(*ddraw_proc)() = (void(*)())GetProcAddress(ddraw._module, "DirectDrawCreate");

	__asm jmp ddraw_proc;
}

extern "C" __declspec(naked) void DirectDrawCreateClipper()
{
	static void(*ddraw_proc)() = (void(*)())GetProcAddress(ddraw._module, "DirectDrawCreateClipper");

	__asm jmp ddraw_proc;
}

extern "C" __declspec(naked) void DirectDrawCreateEx()
{
	static void(*ddraw_proc)() = (void(*)())GetProcAddress(ddraw._module, "DirectDrawCreateEx");

	__asm jmp ddraw_proc;
}

extern "C" __declspec(naked) void DirectDrawEnumerateA()
{
	static void(*ddraw_proc)() = (void(*)())GetProcAddress(ddraw._module, "DirectDrawEnumerateA");

	__asm jmp ddraw_proc;
}

extern "C" __declspec(naked) void DirectDrawEnumerateExA()
{
	static void(*ddraw_proc)() = (void(*)())GetProcAddress(ddraw._module, "DirectDrawEnumerateExA");

	__asm jmp ddraw_proc;
}

extern "C" __declspec(naked) void DirectDrawEnumerateExW()
{
	static void(*ddraw_proc)() = (void(*)())GetProcAddress(ddraw._module, "DirectDrawEnumerateExW");

	__asm jmp ddraw_proc;
}

extern "C" __declspec(naked) void DirectDrawEnumerateW()
{
	static void(*ddraw_proc)() = (void(*)())GetProcAddress(ddraw._module, "DirectDrawEnumerateW");

	__asm jmp ddraw_proc;
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
