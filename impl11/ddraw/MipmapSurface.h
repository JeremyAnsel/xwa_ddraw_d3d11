// Copyright (c) 2014 J�r�my Ansel
// Licensed under the MIT license. See LICENSE.txt

#pragma once

class TextureSurface;

class MipmapSurface : public IDirectDrawSurface
{
public:
	MipmapSurface(DeviceResources* deviceResources, TextureSurface* surface, DWORD width, DWORD height, DDPIXELFORMAT& pixelFormat, DWORD mipmapCount, char* buffer);

	virtual ~MipmapSurface();

	/*** IUnknown methods ***/

	STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID FAR * ppvObj);

	STDMETHOD_(ULONG, AddRef) (THIS);

	STDMETHOD_(ULONG, Release) (THIS);

	/*** IDirectDrawSurface methods ***/

	STDMETHOD(AddAttachedSurface)(THIS_ LPDIRECTDRAWSURFACE);

	STDMETHOD(AddOverlayDirtyRect)(THIS_ LPRECT);

	STDMETHOD(Blt)(THIS_ LPRECT, LPDIRECTDRAWSURFACE, LPRECT, DWORD, LPDDBLTFX);

	STDMETHOD(BltBatch)(THIS_ LPDDBLTBATCH, DWORD, DWORD);

	STDMETHOD(BltFast)(THIS_ DWORD, DWORD, LPDIRECTDRAWSURFACE, LPRECT, DWORD);

	STDMETHOD(DeleteAttachedSurface)(THIS_ DWORD, LPDIRECTDRAWSURFACE);

	STDMETHOD(EnumAttachedSurfaces)(THIS_ LPVOID, LPDDENUMSURFACESCALLBACK);

	STDMETHOD(EnumOverlayZOrders)(THIS_ DWORD, LPVOID, LPDDENUMSURFACESCALLBACK);

	STDMETHOD(Flip)(THIS_ LPDIRECTDRAWSURFACE, DWORD);

	STDMETHOD(GetAttachedSurface)(THIS_ LPDDSCAPS, LPDIRECTDRAWSURFACE FAR *);

	STDMETHOD(GetBltStatus)(THIS_ DWORD);

	STDMETHOD(GetCaps)(THIS_ LPDDSCAPS);

	STDMETHOD(GetClipper)(THIS_ LPDIRECTDRAWCLIPPER FAR*);

	STDMETHOD(GetColorKey)(THIS_ DWORD, LPDDCOLORKEY);

	STDMETHOD(GetDC)(THIS_ HDC FAR *);

	STDMETHOD(GetFlipStatus)(THIS_ DWORD);

	STDMETHOD(GetOverlayPosition)(THIS_ LPLONG, LPLONG);

	STDMETHOD(GetPalette)(THIS_ LPDIRECTDRAWPALETTE FAR*);

	STDMETHOD(GetPixelFormat)(THIS_ LPDDPIXELFORMAT);

	STDMETHOD(GetSurfaceDesc)(THIS_ LPDDSURFACEDESC);

	STDMETHOD(Initialize)(THIS_ LPDIRECTDRAW, LPDDSURFACEDESC);

	STDMETHOD(IsLost)(THIS);

	STDMETHOD(Lock)(THIS_ LPRECT, LPDDSURFACEDESC, DWORD, HANDLE);

	STDMETHOD(ReleaseDC)(THIS_ HDC);

	STDMETHOD(Restore)(THIS);

	STDMETHOD(SetClipper)(THIS_ LPDIRECTDRAWCLIPPER);

	STDMETHOD(SetColorKey)(THIS_ DWORD, LPDDCOLORKEY);

	STDMETHOD(SetOverlayPosition)(THIS_ LONG, LONG);

	STDMETHOD(SetPalette)(THIS_ LPDIRECTDRAWPALETTE);

	STDMETHOD(Unlock)(THIS_ LPVOID);

	STDMETHOD(UpdateOverlay)(THIS_ LPRECT, LPDIRECTDRAWSURFACE, LPRECT, DWORD, LPDDOVERLAYFX);

	STDMETHOD(UpdateOverlayDisplay)(THIS_ DWORD);

	STDMETHOD(UpdateOverlayZOrder)(THIS_ DWORD, LPDIRECTDRAWSURFACE);

	ULONG _refCount;

	DeviceResources* _deviceResources;

	TextureSurface* _surface;

	DWORD _width;
	DWORD _height;
	DDPIXELFORMAT _pixelFormat;
	DWORD _mipmapCount;

	DWORD _bufferSize;
	char* _buffer;

	ComPtr<MipmapSurface> _mipmap;
};
