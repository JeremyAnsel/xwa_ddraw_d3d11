// Copyright (c) 2014 Jérémy Ansel
// Licensed under the MIT license. See LICENSE.txt

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN
#define STRICT
#include <ddraw.h>
#include <d3d.h>

#include "ComPtr.h"

#include <comdef.h>
#include <iostream>

#pragma comment(lib, "dxguid")
#pragma comment(lib, "ddraw")

using namespace std;

static BOOL CALLBACK enum_direct_draw_objects_callback(GUID* lpGUID, LPSTR lpDriverDescription, LPSTR lpDriverName, LPVOID lpContext, HMONITOR hm)
{
	cout << "\t" << lpDriverName << "\t" << lpDriverDescription << endl;

	return DDENUMRET_OK;
}

HRESULT enum_direct_draw_objects()
{
	HRESULT hr;

	if (FAILED(hr = DirectDrawEnumerateExA(enum_direct_draw_objects_callback, nullptr, 0)))
		return hr;

	return S_OK;
}

HRESULT direct_draw_create()
{
	HRESULT hr;
	ComPtr<IDirectDraw> ddraw;

	if (FAILED(hr = DirectDrawCreate(nullptr, &ddraw, nullptr)))
		return hr;

	return S_OK;
}

HRESULT get_available_video_memory()
{
	HRESULT hr;
	ComPtr<IDirectDraw> ddraw;
	ComPtr<IDirectDraw2> ddraw2;

	if (FAILED(hr = DirectDrawCreate(nullptr, &ddraw, nullptr)))
		return hr;

	if (FAILED(hr = ddraw->QueryInterface(IID_IDirectDraw2, (LPVOID*)&ddraw2)))
		return hr;

	DDSCAPS caps = {};
	caps.dwCaps = DDSCAPS_TEXTURE;

	DWORD vidMemTotal;
	DWORD vidMemFree;

	if (FAILED(hr = ddraw2->GetAvailableVidMem(&caps, &vidMemTotal, &vidMemFree)))
		return hr;

	cout << "\t" << vidMemTotal << endl;

	return S_OK;
}

static HRESULT CALLBACK enum_display_modes_callback(LPDDSURFACEDESC lpDDSurfaceDesc, LPVOID lpContext)
{
	cout << "\t" << lpDDSurfaceDesc->dwWidth << "x" << lpDDSurfaceDesc->dwHeight << endl;

	return DDENUMRET_OK;
}

HRESULT enum_display_modes()
{
	HRESULT hr;
	ComPtr<IDirectDraw> ddraw;
	ComPtr<IDirectDraw2> ddraw2;

	if (FAILED(hr = DirectDrawCreate(nullptr, &ddraw, nullptr)))
		return hr;

	if (FAILED(hr = ddraw->QueryInterface(IID_IDirectDraw2, (LPVOID*)&ddraw2)))
		return hr;

	if (FAILED(ddraw2->EnumDisplayModes(0, nullptr, nullptr, enum_display_modes_callback)))
		return hr;

	return S_OK;
}

HRESULT set_display_mode()
{
	HRESULT hr;
	ComPtr<IDirectDraw> ddraw;

	if (FAILED(hr = DirectDrawCreate(nullptr, &ddraw, nullptr)))
		return hr;

	if (FAILED(hr = ddraw->SetCooperativeLevel(nullptr, 0)))
		return hr;

	if (FAILED(hr = ddraw->SetDisplayMode(640, 480, 32)))
		return hr;

	return S_OK;
}

static HRESULT CALLBACK enum_d3d_devices_callback(LPGUID lpGuid, LPSTR lpDeviceDescription, LPSTR lpDeviceName, LPD3DDEVICEDESC lpD3DHWDeviceDesc, LPD3DDEVICEDESC lpD3DHELDeviceDesc, LPVOID lpContext)
{
	cout << "\t" << lpDeviceName << "\t" << lpDeviceDescription << endl;

	return D3DENUMRET_OK;
}

HRESULT enum_d3d_devices()
{
	HRESULT hr;
	ComPtr<IDirectDraw> ddraw;
	ComPtr<IDirect3D> d3dObject;

	if (FAILED(hr = DirectDrawCreate(nullptr, &ddraw, nullptr)))
		return hr;

	if (FAILED(hr = ddraw->QueryInterface(IID_IDirect3D, (LPVOID*)&d3dObject)))
		return hr;

	if (FAILED(hr = d3dObject->EnumDevices(enum_d3d_devices_callback, nullptr)))
		return hr;

	return S_OK;
}

HRESULT CALLBACK enum_texture_formats_callback(LPDDSURFACEDESC lpDdsd, LPVOID lpContext)
{
	cout << "\t" << lpDdsd->ddpfPixelFormat.dwRGBBitCount;
	cout << " R:" << (void*)lpDdsd->ddpfPixelFormat.dwRBitMask;
	cout << " G:" << (void*)lpDdsd->ddpfPixelFormat.dwGBitMask;
	cout << " B:" << (void*)lpDdsd->ddpfPixelFormat.dwBBitMask;

	if (lpDdsd->ddpfPixelFormat.dwRGBAlphaBitMask != 0)
	{
		cout << " A:" << (void*)lpDdsd->ddpfPixelFormat.dwRGBAlphaBitMask;
	}

	cout << endl;

	return D3DENUMRET_OK;
}

HRESULT enum_texture_formats()
{
	HRESULT hr;
	DDSURFACEDESC sd;
	DDSCAPS caps;
	ComPtr<IDirectDraw> ddraw;
	ComPtr<IDirectDrawSurface> primarySurface;
	ComPtr<IDirectDrawSurface> backbufferSurface;
	ComPtr<IDirect3DDevice> d3dDevice;

	if (FAILED(hr = DirectDrawCreate(nullptr, &ddraw, nullptr)))
		return hr;

	sd = {};
	sd.dwSize = sizeof(DDSURFACEDESC);
	sd.dwFlags = DDSD_CAPS;
	sd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP;

	if (FAILED(hr = ddraw->CreateSurface(&sd, &primarySurface, nullptr)))
		return hr;

	caps = {};
	caps.dwCaps = DDSCAPS_BACKBUFFER;

	if (FAILED(hr = primarySurface->GetAttachedSurface(&caps, &backbufferSurface)))
		return hr;

	if (FAILED(hr = backbufferSurface->QueryInterface(IID_IDirect3DHALDevice, (LPVOID*)&d3dDevice)))
		return hr;

	if (FAILED(hr = d3dDevice->EnumTextureFormats(enum_texture_formats_callback, nullptr)))
		return hr;

	return S_OK;
}

HRESULT create_main_surfaces()
{
	HRESULT hr;
	DDSURFACEDESC sd;
	DDSCAPS caps;
	DDCOLORKEY colorkey;
	ComPtr<IDirectDraw> ddraw;
	ComPtr<IDirectDrawSurface> primarySurface;
	ComPtr<IDirectDrawSurface> backbufferSurface;
	ComPtr<IDirectDrawSurface> frontbufferSurface;
	ComPtr<IDirectDrawSurface> depthSurface;
	ComPtr<IDirectDrawSurface> offscreenSurface;

	if (FAILED(hr = DirectDrawCreate(nullptr, &ddraw, nullptr)))
		return hr;

	sd = {};
	sd.dwSize = sizeof(DDSURFACEDESC);
	sd.dwFlags = DDSD_CAPS;
	sd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP;

	if (FAILED(hr = ddraw->CreateSurface(&sd, &primarySurface, nullptr)))
		return hr;

	if (FAILED(hr = primarySurface->GetSurfaceDesc(&sd)))
		return hr;

	caps = {};
	caps.dwCaps = DDSCAPS_BACKBUFFER;

	if (FAILED(hr = primarySurface->GetAttachedSurface(&caps, &backbufferSurface)))
		return hr;

	if (FAILED(hr = backbufferSurface->GetSurfaceDesc(&sd)))
		return hr;

	sd = {};
	sd.dwSize = sizeof(DDSURFACEDESC);
	sd.dwFlags = DDSD_CAPS;
	sd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;

	if (FAILED(hr = ddraw->CreateSurface(&sd, &frontbufferSurface, nullptr)))
		return hr;

	if (FAILED(hr = frontbufferSurface->GetSurfaceDesc(&sd)))
		return hr;

	colorkey = {};
	colorkey.dwColorSpaceLowValue = 0x2000;
	colorkey.dwColorSpaceHighValue = 0x2000;

	if (FAILED(hr = frontbufferSurface->SetColorKey(DDCKEY_SRCBLT, &colorkey)))
		return hr;

	sd = {};
	sd.dwSize = sizeof(DDSURFACEDESC);
	sd.dwFlags = DDSD_CAPS;
	sd.ddsCaps.dwCaps = DDSCAPS_ZBUFFER;

	if (FAILED(hr = ddraw->CreateSurface(&sd, &depthSurface, nullptr)))
		return hr;

	if (FAILED(hr = backbufferSurface->AddAttachedSurface(depthSurface)))
		return hr;

	if (FAILED(hr = depthSurface->GetSurfaceDesc(&sd)))
		return hr;

	sd = {};
	sd.dwSize = sizeof(DDSURFACEDESC);
	sd.dwFlags = DDSD_CAPS;
	sd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;

	if (FAILED(hr = ddraw->CreateSurface(&sd, &offscreenSurface, nullptr)))
		return hr;

	if (FAILED(hr = offscreenSurface->GetSurfaceDesc(&sd)))
		return hr;

	return S_OK;
}

HRESULT create_viewport()
{
	HRESULT hr;
	DDSURFACEDESC sd;
	DDSCAPS caps;
	D3DVIEWPORT vp;
	ComPtr<IDirectDraw> ddraw;
	ComPtr<IDirectDrawSurface> primarySurface;
	ComPtr<IDirectDrawSurface> backbufferSurface;
	ComPtr<IDirect3D> d3dObject;
	ComPtr<IDirect3DDevice> d3dDevice;
	ComPtr<IDirect3DViewport> viewport;

	if (FAILED(hr = DirectDrawCreate(nullptr, &ddraw, nullptr)))
		return hr;

	sd = {};
	sd.dwSize = sizeof(DDSURFACEDESC);
	sd.dwFlags = DDSD_CAPS;
	sd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP;

	if (FAILED(hr = ddraw->CreateSurface(&sd, &primarySurface, nullptr)))
		return hr;

	caps = {};
	caps.dwCaps = DDSCAPS_BACKBUFFER;

	if (FAILED(hr = primarySurface->GetAttachedSurface(&caps, &backbufferSurface)))
		return hr;

	if (FAILED(hr = ddraw->QueryInterface(IID_IDirect3D, (LPVOID*)&d3dObject)))
		return hr;

	if (FAILED(hr = backbufferSurface->QueryInterface(IID_IDirect3DHALDevice, (LPVOID*)&d3dDevice)))
		return hr;

	if (FAILED(hr = d3dObject->CreateViewport(&viewport, nullptr)))
		return hr;

	if (FAILED(hr = d3dDevice->AddViewport(viewport)))
		return hr;

	vp = {};
	vp.dwSize = sizeof(D3DVIEWPORT);
	vp.dwWidth = 640;
	vp.dwHeight = 480;

	if (FAILED(hr = viewport->SetViewport(&vp)))
		return hr;

	return S_OK;
}

HRESULT execute_buffer_exit()
{
	HRESULT hr;
	DDSURFACEDESC sd;
	DDSCAPS caps;
	D3DEXECUTEBUFFERDESC ebd;
	D3DEXECUTEDATA ed;
	ComPtr<IDirectDraw> ddraw;
	ComPtr<IDirectDrawSurface> primarySurface;
	ComPtr<IDirectDrawSurface> backbufferSurface;
	ComPtr<IDirect3D> d3dObject;
	ComPtr<IDirect3DDevice> d3dDevice;
	ComPtr<IDirect3DExecuteBuffer> executeBuffer;

	if (FAILED(hr = DirectDrawCreate(nullptr, &ddraw, nullptr)))
		return hr;

	if (FAILED(hr = ddraw->SetCooperativeLevel(nullptr, 0)))
		return hr;

	if (FAILED(hr = ddraw->SetDisplayMode(640, 480, 32)))
		return hr;

	sd = {};
	sd.dwSize = sizeof(DDSURFACEDESC);
	sd.dwFlags = DDSD_CAPS;
	sd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP;

	if (FAILED(hr = ddraw->CreateSurface(&sd, &primarySurface, nullptr)))
		return hr;

	caps = {};
	caps.dwCaps = DDSCAPS_BACKBUFFER;

	if (FAILED(hr = primarySurface->GetAttachedSurface(&caps, &backbufferSurface)))
		return hr;

	if (FAILED(hr = ddraw->QueryInterface(IID_IDirect3D, (LPVOID*)&d3dObject)))
		return hr;

	if (FAILED(hr = backbufferSurface->QueryInterface(IID_IDirect3DHALDevice, (LPVOID*)&d3dDevice)))
		return hr;

	ebd = {};
	ebd.dwSize = sizeof(D3DEXECUTEBUFFERDESC);
	ebd.dwFlags = D3DDEB_BUFSIZE;
	ebd.dwBufferSize = 0x1000;

	if (FAILED(hr = d3dDevice->CreateExecuteBuffer(&ebd, &executeBuffer, nullptr)))
		return hr;

	if (FAILED(hr = executeBuffer->Lock(&ebd)))
		return hr;

	LPD3DINSTRUCTION ebI;

	ebI = (LPD3DINSTRUCTION)ebd.lpData;
	ebI->bOpcode = D3DOP_EXIT;
	ebI->bSize = 0;
	ebI->wCount = 0;
	ebI++;

	if (FAILED(hr = executeBuffer->Unlock()))
		return hr;

	ed = {};
	ed.dwSize = sizeof(D3DEXECUTEDATA);
	ed.dwInstructionLength = (DWORD)ebI - (DWORD)ebd.lpData;

	if (FAILED(hr = executeBuffer->SetExecuteData(&ed)))
		return hr;

	if (FAILED(hr = d3dDevice->BeginScene()))
		return hr;

	if (FAILED(hr = d3dDevice->Execute(executeBuffer, nullptr, 0)))
		return hr;

	if (FAILED(hr = d3dDevice->EndScene()))
		return hr;

	return S_OK;
}

HRESULT create_texture()
{
	HRESULT hr;
	ComPtr<IDirectDraw> ddraw;

	if (FAILED(hr = DirectDrawCreate(nullptr, &ddraw, nullptr)))
		return hr;

	DDSURFACEDESC sd;
	sd = {};
	sd.dwSize = sizeof(DDSURFACEDESC);
	sd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT | DDSD_MIPMAPCOUNT;
	sd.ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_MIPMAP;
	sd.dwWidth = 256;
	sd.dwHeight = 256;
	sd.dwMipMapCount = 2;
	sd.ddpfPixelFormat.dwRGBBitCount = 32;

	ComPtr<IDirectDrawSurface> surface;

	if (FAILED(hr = ddraw->CreateSurface(&sd, &surface, nullptr)))
		return hr;

	LPDIRECTDRAWSURFACE mipmap{ surface.Get() };

	for (DWORD i = 1; i < sd.dwMipMapCount; i++)
	{
		DDSCAPS caps{ DDSCAPS_TEXTURE };

		ComPtr<IDirectDrawSurface> attached;

		if (FAILED(hr = mipmap->GetAttachedSurface(&caps, &attached)))
			return hr;

		mipmap = attached.Get();
	}

	return S_OK;
}

#define RUN_TEST(fct) \
	{ \
	HRESULT hr; \
	cout << "*** " << #fct << " ***" << endl; \
	if(FAILED(hr = (fct))) \
									{ \
		cout << "\t* failed " << (void*)hr << ": " << _com_error(hr).ErrorMessage() << endl; \
									} else { \
		cout << "\t* passed" << endl; \
	} \
}

int main()
{
	setlocale(LC_ALL, "");

	HRESULT hr;
	DDSURFACEDESC sd;
	DDSCAPS caps;
	DDCOLORKEY colorkey;
	DDBLTFX ddBltFx;

#define CALL(fct) cout << #fct << endl; if(FAILED(hr = (fct))) { cout << (void*)hr << ": " << _com_error(hr).ErrorMessage() << endl; return -1; }

	ComPtr<IDirectDraw> ddraw;
	CALL(DirectDrawCreate(nullptr, &ddraw, nullptr));
	CALL(ddraw->SetCooperativeLevel(nullptr, 0));
	CALL(ddraw->SetDisplayMode(640, 480, 32));

	ComPtr<IDirectDrawSurface> primarySurface;
	sd = {};
	sd.dwSize = sizeof(DDSURFACEDESC);
	sd.dwFlags = DDSD_CAPS;
	sd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP;
	CALL(ddraw->CreateSurface(&sd, &primarySurface, nullptr));

	ComPtr<IDirectDrawSurface> backbufferSurface;
	caps = {};
	caps.dwCaps = DDSCAPS_BACKBUFFER;
	CALL(primarySurface->GetAttachedSurface(&caps, &backbufferSurface));

	ComPtr<IDirectDrawSurface> frontbufferSurface;
	sd = {};
	sd.dwSize = sizeof(DDSURFACEDESC);
	sd.dwFlags = DDSD_CAPS;
	sd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
	CALL(ddraw->CreateSurface(&sd, &frontbufferSurface, nullptr));

	colorkey = {};
	colorkey.dwColorSpaceLowValue = 0x2000;
	colorkey.dwColorSpaceHighValue = 0x2000;
	CALL(frontbufferSurface->SetColorKey(DDCKEY_SRCBLT, &colorkey));

	ComPtr<IDirectDrawSurface> depthSurface;
	sd = {};
	sd.dwSize = sizeof(DDSURFACEDESC);
	sd.dwFlags = DDSD_CAPS;
	sd.ddsCaps.dwCaps = DDSCAPS_ZBUFFER;
	CALL(ddraw->CreateSurface(&sd, &depthSurface, nullptr));
	CALL(backbufferSurface->AddAttachedSurface(depthSurface));

	ComPtr<IDirectDrawSurface> offscreenSurface;
	sd = {};
	sd.dwSize = sizeof(DDSURFACEDESC);
	sd.dwFlags = DDSD_CAPS;
	sd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
	CALL(ddraw->CreateSurface(&sd, &offscreenSurface, nullptr));

	ComPtr<IDirect3D> d3dObject;
	CALL(ddraw->QueryInterface(IID_IDirect3D, (LPVOID*)&d3dObject));

	ComPtr<IDirect3DDevice> d3dDevice;
	CALL(backbufferSurface->QueryInterface(IID_IDirect3DHALDevice, (LPVOID*)&d3dDevice));

	CALL(frontbufferSurface->Unlock(nullptr));

	ddBltFx = {};
	ddBltFx.dwSize = sizeof(DDBLTFX);
	ddBltFx.dwFillColor = 0;

	CALL(frontbufferSurface->Blt(nullptr, nullptr, nullptr, DDBLT_COLORFILL, &ddBltFx));

	ddBltFx = {};
	ddBltFx.dwSize = sizeof(DDBLTFX);
	ddBltFx.dwFillColor = 0;

	CALL(offscreenSurface->Blt(nullptr, nullptr, nullptr, DDBLT_COLORFILL, &ddBltFx));

	CALL(backbufferSurface->BltFast(0, 0, frontbufferSurface, nullptr, 0));
	CALL(primarySurface->Flip(backbufferSurface, 0));

	CALL(backbufferSurface->BltFast(0, 0, frontbufferSurface, nullptr, 0));
	CALL(primarySurface->BltFast(0, 0, backbufferSurface, nullptr, 0));

	CALL(primarySurface->Flip(frontbufferSurface, 0));

	CALL(frontbufferSurface->Lock(nullptr, &sd, 0, nullptr));
	CALL(frontbufferSurface->Unlock(nullptr));

	CALL(offscreenSurface->Lock(nullptr, &sd, 0, nullptr));
	CALL(offscreenSurface->Unlock(nullptr));

	CALL(S_OK);

	RUN_TEST(enum_direct_draw_objects());
	RUN_TEST(direct_draw_create());
	RUN_TEST(get_available_video_memory());
	RUN_TEST(enum_display_modes());
	RUN_TEST(set_display_mode());
	RUN_TEST(enum_d3d_devices());
	RUN_TEST(enum_texture_formats());
	RUN_TEST(create_main_surfaces());
	RUN_TEST(create_viewport());
	RUN_TEST(execute_buffer_exit());
	RUN_TEST(create_texture());

	return 0;
}
