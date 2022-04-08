// Copyright (c) 2014 Jérémy Ansel
// Licensed under the MIT license. See LICENSE.txt

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN
#define STRICT
#include <Windows.h>

#include "config.h"

#include "XwaDrawTextHook.h"
#include "XwaDrawRadarHook.h"
#include "XwaDrawBracketHook.h"
#include "XwaD3dRendererHook.h"

bool IsXwaExe()
{
	char filename[4096];

	if (GetModuleFileName(nullptr, filename, sizeof(filename)) == 0)
	{
		return false;
	}

	int length = strlen(filename);

	if (length < 17)
	{
		return false;
	}

	return _stricmp(filename + length - 17, "xwingalliance.exe") == 0;
}

bool _IsXwaExe = IsXwaExe();

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		if (IsXwaExe())
		{
			if (g_config.Text2DRendererEnabled)
			{
				// RenderCharHook
				*(unsigned char*)(0x00450A47 + 0x00) = 0xE8;
				*(int*)(0x00450A47 + 0x01) = (int)RenderCharHook - (0x00450A47 + 0x05);

				// ComputeMetricsHook
				*(unsigned char*)(0x00510385 + 0x00) = 0xE8;
				*(int*)(0x00510385 + 0x01) = (int)ComputeMetricsHook - (0x00510385 + 0x05);
			}

			if (g_config.Radar2DRendererEnabled)
			{
				// DrawRadarHook
				*(int*)(0x00434977 + 0x06) = (int)DrawRadarHook;
				*(int*)(0x00434995 + 0x06) = (int)DrawRadarSelectedHook;

				// DrawBracketInFlightHook
				*(unsigned char*)(0x00503D46 + 0x00) = 0xE8;
				*(int*)(0x00503D46 + 0x01) = (int)DrawBracketInFlightHook - (0x00503D46 + 0x05);

				// DrawBracketInFlightHook CMD
				*(unsigned char*)(0x00478E44 + 0x00) = 0xE8;
				*(int*)(0x00478E44 + 0x01) = (int)DrawBracketInFlightHook - (0x00478E44 + 0x05);

				// DrawBracketMapHook
				*(unsigned char*)(0x00503CFE + 0x00) = 0xE8;
				*(int*)(0x00503CFE + 0x01) = (int)DrawBracketMapHook - (0x00503CFE + 0x05);
			}

			if (g_config.D3dRendererHookEnabled)
			{
				// D3dRenderLasersHook - call 0042BBA0
				*(unsigned char*)(0x004F0B7E + 0x00) = 0xE8;
				*(int*)(0x004F0B7E + 0x01) = (int)D3dRenderLasersHook - (0x004F0B7E + 0x05);

				// D3dRenderMiniatureHook - call 00478490
				*(unsigned char*)(0x00478412 + 0x00) = 0xE8;
				*(int*)(0x00478412 + 0x01) = (int)D3dRenderMiniatureHook - (0x00478412 + 0x05);
				*(unsigned char*)(0x00478483 + 0x00) = 0xE8;
				*(int*)(0x00478483 + 0x01) = (int)D3dRenderMiniatureHook - (0x00478483 + 0x05);

				// D3dRenderHyperspaceLinesHook - call 00480A80
				*(unsigned char*)(0x0047DCB6 + 0x00) = 0xE8;
				*(int*)(0x0047DCB6 + 0x01) = (int)D3dRenderHyperspaceLinesHook - (0x0047DCB6 + 0x05);

				// D3dRendererHook - call 00480370
				*(unsigned char*)(0x004829C5 + 0x00) = 0xE8;
				*(int*)(0x004829C5 + 0x01) = (int)D3dRendererMainHook - (0x004829C5 + 0x05);
				*(unsigned char*)(0x004829DF + 0x00) = 0xE8;
				*(int*)(0x004829DF + 0x01) = (int)D3dRendererMainHook - (0x004829DF + 0x05);

				// D3dRendererShadowHook - call 0044FD10
				*(unsigned char*)(0x004847DE + 0x00) = 0xE8;
				*(int*)(0x004847DE + 0x01) = (int)D3dRendererShadowHook - (0x004847DE + 0x05);
				*(unsigned char*)(0x004847F3 + 0x00) = 0xE8;
				*(int*)(0x004847F3 + 0x01) = (int)D3dRendererShadowHook - (0x004847F3 + 0x05);

				// D3dRendererOptLoadHook - call 0050E3B0
				*(int*)(0x004CC965 + 0x01) = (int)D3dRendererOptLoadHook - (0x004CC965 + 0x05);

				// D3dRendererOptNodeHook - call 00482000
				*(unsigned char*)(0x004815BF + 0x03) = 0x10; // esp+10
				*(int*)(0x004815CA + 0x01) = (int)D3dRendererOptNodeHook - (0x004815CA + 0x05);
				*(unsigned char*)(0x00481F9E + 0x00) = 0x57; // push edi
				*(int*)(0x00481FA5 + 0x01) = (int)D3dRendererOptNodeHook - (0x00481FA5 + 0x05);
				*(unsigned char*)(0x00481FC7 + 0x00) = 0x57; // push edi
				*(int*)(0x00481FC9 + 0x01) = (int)D3dRendererOptNodeHook - (0x00481FC9 + 0x05);
			}
		}

		break;

	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}

	return TRUE;
}
