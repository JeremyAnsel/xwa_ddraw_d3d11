// Copyright (c) 2014 Jérémy Ansel
// Licensed under the MIT license. See LICENSE.txt
// Extended for VR by Leo Reyes (c) 2019

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN
#define STRICT
#include <Windows.h>
#include <objbase.h>

#include <stdio.h>
#include <vector>

extern bool g_bFixSkyBox, g_bSkipGUI, g_bSkipText, g_bSkipSkyBox;
bool g_bCapture2DOffscreenBuffer = false;
#ifdef DBG_VR
extern bool g_bDo3DCapture, g_bStart3DCapture;
bool g_bDumpDebug = false;

//extern bool g_bDumpSpecificTex;
//extern int g_iDumpSpecificTexIdx;
void IncreaseCockpitThreshold(float Delta);
void IncreaseNoDrawBeforeIndex(int Delta);
void IncreaseNoDrawAfterIndex(int Delta);
//void IncreaseNoExecIndices(int DeltaBefore, int DeltaAfter);
//void IncreaseZOverride(float Delta);
void IncreaseSkipNonZBufferDrawIdx(int Delta);
void IncreaseSkyBoxIndex(int Delta);
void IncreaseFocalDist(float Delta);
#endif

extern bool g_bDisableBarrelEffect, g_bEnableVR;
extern bool g_bLeftKeyDown, g_bRightKeyDown, g_bUpKeyDown, g_bDownKeyDown, g_bUpKeyDownShift, g_bDownKeyDownShift;
HWND ThisWindow = 0;
WNDPROC OldWindowProc = 0;

// User-facing functions
void ResetVRParams(); // Restores default values for the view params
void SaveVRParams();
void LoadVRParams();

void IncreaseIPD(float Delta);
void IncreaseScreenScale(float Delta); // Changes overall zoom
void ToggleZoomOutMode();
void IncreaseZoomOutScale(float Delta);
void IncreaseHUDParallax(float Delta);
void IncreaseTextParallax(float Delta);
void IncreaseFloatingGUIParallax(float Delta);
void ToggleCockpitPZHack();
void IncreaseSkipNonZBufferDrawIdx(int Delta);

// Lens distortion
void IncreaseLensK1(float Delta);
void IncreaseLensK2(float Delta);

// SteamVR
#include <headers/openvr.h>
extern bool g_bSteamVREnabled, g_bSteamVRInitialized, g_bUseSteamVR;
extern vr::IVRSystem *g_pHMD;
extern vr::IVRScreenshots *g_pVRScreenshots;
bool InitSteamVR();
void ShutDownSteamVR();

// Debug functions
bool ReloadCRCs();
void log_debug(const char *format, ...);

LRESULT CALLBACK MyWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	bool AltKey = (GetAsyncKeyState(VK_MENU) & 0x8000) == 0x8000;
	bool CtrlKey = (GetAsyncKeyState(VK_CONTROL) & 0x8000) == 0x8000;
	bool ShiftKey = (GetAsyncKeyState(VK_SHIFT) & 0x8000) == 0x8000;

	switch (uMsg)
	{
	case WM_KEYUP: {
		// Ctrl + Alt + Shift
		if (AltKey && CtrlKey && ShiftKey) {
			switch (wParam) {
			case 0xbb:
				IncreaseZoomOutScale(0.1f);
				return 0;
			case 0xbd:
				IncreaseZoomOutScale(-0.1f);
				return 0;

			case VK_RIGHT:
				IncreaseFloatingGUIParallax( 0.005f);
				return 0;
			case VK_LEFT:
				IncreaseFloatingGUIParallax(-0.005f);
				return 0;

			case VK_UP: // Delta is in cms
				IncreaseHUDParallax( 0.005f);
				return 0;
			case VK_DOWN:
				IncreaseHUDParallax(-0.005f);
				return 0;
			}
		}

		// Ctrl + Alt
		if (AltKey && CtrlKey && !ShiftKey) {
			switch (wParam) {
#if DBG_VR
			case 'C':
				log_debug("[DBG] Capture 3D");
				//g_bDo3DCapture = true;
				g_bStart3DCapture = true;
				return 0;
			case 'D':
				// Force a refresh of the display width
				g_bDisplayWidth = true;
				return 0;
			case 'D':
				log_debug("[DBG] Dumping specific texture");
				g_bDumpSpecificTex = true;
				return 0;
				/*case 'T':
					g_bSkipText = !g_bSkipText;
					if (g_bSkipText)
						log_debug("[DBG] Skipping Text GUI");
					else
						log_debug("[DBG] Drawing Text GUI");*/
						/*
						g_bSkipAfterTargetComp = !g_bSkipAfterTargetComp;
						if (g_bSkipAfterTargetComp)
							log_debug("[DBG] Skipping draw calls after TargetComp");
						else
							log_debug("[DBG] NOT Skipping draw calls after TargetComp");
						return 0;
						*/
			case 'F':
				g_bSkipSkyBox = !g_bSkipSkyBox;
				if (g_bFixSkyBox)
					log_debug("[DBG] Skip Skybox ON");
				else
					log_debug("[DBG] Skip Skybox OFF");
				return 0;

			case 'G':
				g_bSkipGUI = !g_bSkipGUI;
				log_debug("[DBG] g_bSkipGUI: %d", g_bSkipGUI);
				return 0;
#endif

			case 'B':
				g_bDisableBarrelEffect = !g_bDisableBarrelEffect;
				return 0;
			case 'R':
				ResetVRParams();
				return 0;

			case 'V':
				g_bEnableVR = !g_bEnableVR;
				return 0;

			case 'S':
				SaveVRParams();
				return 0;
			case 'L':
				LoadVRParams();
				return 0;
			case 'H':
				ToggleCockpitPZHack();
				return 0;

			case 'P': 
				if (g_bUseSteamVR && g_pVRScreenshots != NULL) {
					static int scrCounter = 0;
					char prevFileName[80], scrFileName[80];
					sprintf_s(prevFileName, 80, "./preview%d", scrCounter);
					sprintf_s(scrFileName, 80, "./screenshot%d", scrCounter);

					vr::ScreenshotHandle_t scr;
					vr::EVRScreenshotError error = g_pVRScreenshots->TakeStereoScreenshot(&scr, prevFileName, scrFileName);
					if (error)
						log_debug("[DBG] error %d when taking SteamVR screenshot", error);
					else
						log_debug("[DBG] Screeshot %d taken", scrCounter);
					scrCounter++;
				}
				else {
					log_debug("[DBG] !g_bUseSteamVR || g_pVRScreenshots is NULL");
				}
				break;

			case 0xbb:
				IncreaseScreenScale(0.1f);
				return 0;
			case 0xbd:
				IncreaseScreenScale(-0.1f);
				return 0;

			case VK_UP:
				IncreaseLensK1(0.1f);
				return 0;
			case VK_DOWN:
				IncreaseLensK1(-0.1f);
				return 0;
			case VK_LEFT:
				IncreaseLensK2(-0.1f);
				return 0;
			case VK_RIGHT:
				IncreaseLensK2(0.1f);
				return 0;
			}
		}

		// Ctrl
		if (CtrlKey && !AltKey && !ShiftKey) {
			switch (wParam) {
			case 'Z':
				ToggleZoomOutMode();
				return 0;
			
#if DBR_VR
			case 'X':
				g_bCapture2DOffscreenBuffer = true;
				return 0;
			case 'D':
				g_bDumpDebug = !g_bDumpDebug;
				return 0;
#endif

			case VK_RIGHT:
				//IncreaseSkipNonZBufferDrawIdx(1);
				return 0;
			case VK_LEFT:
				//IncreaseSkipNonZBufferDrawIdx(-1);
				return 0;

			}
		}

		// Ctrl + Shift
		if (CtrlKey && !AltKey && ShiftKey) {
			switch (wParam) {
#if DBG_VR
				/*
				case 0xbb:
					//IncreaseNoExecIndices(0, 1);
					return 0;
				case 0xbd:
					//IncreaseNoExecIndices(0, -1);
					return 0;
				*/
#endif

			case VK_UP:
				//IncreaseFloatingGUIParallax(3.0f);
				IncreaseFloatingGUIParallax(0.1f);
				return 0;
			case VK_DOWN:
				IncreaseFloatingGUIParallax(-0.1f);
				return 0;
			case VK_LEFT:
				IncreaseTextParallax(-0.1f);
				return 0;
			case VK_RIGHT:
				IncreaseTextParallax(0.1f);
				return 0;
			}
		}

		// Shift
		if (ShiftKey && !AltKey && !CtrlKey) {
			switch (wParam) {
			case VK_LEFT:
				IncreaseHUDParallax(-0.1f);
				return 0;
			case VK_RIGHT:
				IncreaseHUDParallax(0.1f);
				return 0;

			case VK_UP:
				g_bUpKeyDownShift = false;
				g_bUpKeyDown = false;
				return 0;
			case VK_DOWN:
				g_bDownKeyDown = false;
				g_bDownKeyDownShift = false;
				return 0;
			}
		}

		// Plain key: no Shift, Ctrl, Alt
		if (!ShiftKey && !AltKey && !CtrlKey) {
			switch (wParam) {
			case VK_LEFT:
				g_bLeftKeyDown = false;
				return 0;
			case VK_RIGHT:
				g_bRightKeyDown = false;
				return 0;

			case VK_UP:
				g_bUpKeyDown = false;
				g_bUpKeyDownShift = false;
				return 0;
			case VK_DOWN:
				g_bDownKeyDown = false;
				g_bDownKeyDownShift = false;
				return 0;
			case VK_OEM_PERIOD:
				if (g_bUseSteamVR)
					g_pHMD->ResetSeatedZeroPose();
				break;
			}
		}

		// Alt doesn't seem to work by itself: wParam is messed up
		/* if (!ShiftKey && AltKey && !CtrlKey) {
		}
		*/
		break;
	}

	case WM_KEYDOWN: {
		// Plain key: no Shift, Ctrl, Alt
		if (!ShiftKey && !AltKey && !CtrlKey) {
			switch (wParam) {
			case VK_LEFT:
				g_bLeftKeyDown = true;
				return 0;
			case VK_RIGHT:
				g_bRightKeyDown = true;
				return 0;

			case VK_UP:
				g_bUpKeyDown = true;
				return 0;
			case VK_DOWN:
				g_bDownKeyDown = true;
				return 0;
			}
		}

		// Shift
		if (ShiftKey && !AltKey && !CtrlKey) {
			switch (wParam) {
			case VK_UP:
				g_bUpKeyDownShift = true;
				return 0;
			case VK_DOWN:
				g_bDownKeyDownShift = true;
				return 0;
			}
		}

	}
	}
	// Call the previous WindowProc handler
	return CallWindowProc(OldWindowProc, hwnd, uMsg, wParam, lParam);
}

bool ReplaceWindowProc(HWND hwnd)
{
	ThisWindow = hwnd;
	OldWindowProc = (WNDPROC )SetWindowLong(ThisWindow, GWL_WNDPROC, (LONG )MyWindowProc);
	if (OldWindowProc != NULL)
		return true;
	return false;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		log_debug("[DBG] **********************");
		log_debug("[DBG] Initializing VR ddraw.dll");
		// Initialize the libraries needed to dump DirectX Textures
		CoInitialize(NULL);
		// Load vrparams.cfg if present
		LoadVRParams();
		// Initialize SteamVR
		if (g_bSteamVREnabled && !g_bSteamVRInitialized) {
			g_bUseSteamVR = InitSteamVR();
			log_debug("[DBG] g_bUseSteamVR: %d", g_bUseSteamVR);
			// Do not attempt initalization again, no matter what happened
			g_bSteamVRInitialized = true;
		}
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		if (g_bUseSteamVR)
			ShutDownSteamVR();
		break;
	}

	return TRUE;
}
