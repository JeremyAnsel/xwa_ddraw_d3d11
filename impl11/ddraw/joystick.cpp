// Copyright (c) 2016 Reimar D�ffinger
// Licensed under the MIT license. See LICENSE.txt

#include "config.h"
#include "common.h"

#include <mmsystem.h>

#include "joystick.h"

#pragma comment(lib, "winmm")

#undef min
#undef max
#include <algorithm>

static bool needsJoyEmul()
{
	UINT cnt = joyGetNumDevs();
	for (unsigned i = 0; i < cnt; ++i)
	{
		JOYINFOEX jie;
		memset(&jie, 0, sizeof(jie));
		jie.dwSize = sizeof(jie);
		jie.dwFlags = JOY_RETURNALL;
		UINT res = joyGetPosEx(0, &jie);
		if (res == JOYERR_NOERROR)
			return false;
	}
	return true;
}

UINT WINAPI emulJoyGetNumDevs(void)
{
	if (g_config.JoystickEmul < 0) {
		g_config.JoystickEmul = needsJoyEmul();
	}
	if (!g_config.JoystickEmul) {
		return joyGetNumDevs();
	}
	return 1;
}

UINT WINAPI emulJoyGetDevCaps(UINT_PTR joy, struct tagJOYCAPSA *pjc, UINT size)
{
	if (!g_config.JoystickEmul) {
		return joyGetDevCaps(joy, pjc, size);
	}
	if (joy != 0) return MMSYSERR_NODRIVER;
	if (size != 0x194) return MMSYSERR_INVALPARAM;
	memset(pjc, 0, size);
	pjc->wXmax = 512;
	pjc->wYmax = 512;
	pjc->wNumButtons = 5;
	pjc->wMaxButtons = 5;
	pjc->wNumAxes = 2;
	pjc->wMaxAxes = 2;
	return JOYERR_NOERROR;
}

static DWORD lastGetPos;

UINT WINAPI emulJoyGetPosEx(UINT joy, struct joyinfoex_tag *pji)
{
	if (!g_config.JoystickEmul) {
		return joyGetPosEx(joy, pji);
	}
	if (joy != 0) return MMSYSERR_NODRIVER;
    if (pji->dwSize != 0x34) return MMSYSERR_INVALPARAM;
	DWORD now = GetTickCount();
	// Assume we started a new game
	if ((now - lastGetPos) > 5000)
	{
		SetCursorPos(240, 240);
		GetAsyncKeyState(VK_LBUTTON);
		GetAsyncKeyState(VK_RBUTTON);
		GetAsyncKeyState(VK_MBUTTON);
		GetAsyncKeyState(VK_XBUTTON1);
		GetAsyncKeyState(VK_XBUTTON2);
	}
	lastGetPos = now;
	POINT pos;
	GetCursorPos(&pos);
	pji->dwXpos = static_cast<DWORD>(std::min(256.0f + (pos.x - 240.0f) * g_config.MouseSensitivity, 512.0f));
	pji->dwYpos = static_cast<DWORD>(std::min(256.0f + (pos.y - 240.0f) * g_config.MouseSensitivity, 512.0f));
	pji->dwButtons = 0;
	pji->dwButtonNumber = 0;
	if (GetAsyncKeyState(VK_LBUTTON)) {
		pji->dwButtons |= 1;
		++pji->dwButtonNumber;
	}
	if (GetAsyncKeyState(VK_RBUTTON)) {
		pji->dwButtons |= 2;
		++pji->dwButtonNumber;
	}
	if (GetAsyncKeyState(VK_MBUTTON)) {
		pji->dwButtons |= 4;
		++pji->dwButtonNumber;
	}
	if (GetAsyncKeyState(VK_XBUTTON1)) {
		pji->dwButtons |= 8;
		++pji->dwButtonNumber;
	}
	if (GetAsyncKeyState(VK_XBUTTON2)) {
		pji->dwButtons |= 16;
		++pji->dwButtonNumber;
	}
	if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
		pji->dwXpos = static_cast<DWORD>(std::max(256 - 256 * g_config.KbdSensitivity, 0.0f));;
	}
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
		pji->dwXpos = static_cast<DWORD>(std::min(256 + 256 * g_config.KbdSensitivity, 512.0f));;
	}
	if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
		pji->dwYpos = static_cast<DWORD>(std::max(256 - 256 * g_config.KbdSensitivity, 0.0f));;
	}
	if (GetAsyncKeyState(VK_UP) & 0x8000) {
		pji->dwYpos = static_cast<DWORD>(std::min(256 + 256 * g_config.KbdSensitivity, 512.0f));;
	}
	return JOYERR_NOERROR;
}