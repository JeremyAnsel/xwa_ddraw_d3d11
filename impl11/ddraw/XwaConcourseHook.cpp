#include "common.h"
#include "XwaConcourseHook.h"
#include "DeviceResources.h"
#include "FrontbufferSurface.h"
#include "DirectDraw.h"
#include <fstream>

int g_callDrawCursor = true;

void ConcourseTakeScreenshot()
{
	bool takeScreenshot = false;

	MSG msg;
	if (PeekMessage(&msg, nullptr, WM_SYSKEYUP, WM_SYSKEYUP, PM_NOREMOVE))
	{
		if (msg.message == WM_SYSKEYUP && msg.wParam == 'O')
		{
			GetMessage(&msg, nullptr, WM_SYSKEYUP, WM_SYSKEYUP);
			takeScreenshot = true;
		}
	}

	if (!takeScreenshot)
	{
		return;
	}

	wchar_t filename[64];

	for (int index = 0; true; index++)
	{
		swprintf_s(filename, L"frontscreen%d.jpg", index);

		if (!std::ifstream(filename))
		{
			break;
		}
	}

	FrontbufferSurface* frontSurface = *(FrontbufferSurface**)(0x09F60E0 + 0x0F56);
	DeviceResources* deviceResources = frontSurface->_deviceResources;

	UINT width = deviceResources->_backbufferWidth;
	UINT height = deviceResources->_backbufferHeight;
	char* buffer = new char[width * height * 4];

	deviceResources->RetrieveBackBuffer(buffer, width, height, 4);

	saveScreenshot(filename, buffer, width, height, 4);

	delete[] buffer;
}

void DrawCursor()
{
	if (!g_callDrawCursor)
	{
		return;
	}

	// XwaDrawCursor
	((void(*)())0x0055B630)();
}

void PlayVideoClear()
{
	DirectDraw* MainIDirectDraw = *(DirectDraw**)(0x09F60E0 + 0x0F46);

	MainIDirectDraw->_deviceResources->_d3dDeviceContext->ClearRenderTargetView(MainIDirectDraw->_deviceResources->_renderTargetView, MainIDirectDraw->_deviceResources->clearColor);
	MainIDirectDraw->_deviceResources->_d3dDeviceContext->ClearDepthStencilView(MainIDirectDraw->_deviceResources->_depthStencilView, D3D11_CLEAR_DEPTH, MainIDirectDraw->_deviceResources->clearDepth, 0);

	((void(*)())0x0055EEA0)();
}
