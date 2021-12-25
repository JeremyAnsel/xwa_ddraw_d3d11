// Copyright (c) 2014 Jérémy Ansel
// Licensed under the MIT license. See LICENSE.txt

#pragma once

class TextureSurface;

class Direct3DTexture : public IDirect3DTexture
{
public:
	Direct3DTexture(DeviceResources* deviceResources, TextureSurface* surface);

	virtual ~Direct3DTexture();

	/*** IUnknown methods ***/
	
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj);
	
	STDMETHOD_(ULONG, AddRef)(THIS);
	
	STDMETHOD_(ULONG, Release)(THIS);

	/*** IDirect3DTexture methods ***/
	
	STDMETHOD(Initialize)(THIS_ LPDIRECT3DDEVICE, LPDIRECTDRAWSURFACE);
	
	STDMETHOD(GetHandle)(THIS_ LPDIRECT3DDEVICE, LPD3DTEXTUREHANDLE);
	
	STDMETHOD(PaletteChanged)(THIS_ DWORD, DWORD);
	
	STDMETHOD(Load)(THIS_ LPDIRECT3DTEXTURE);
	
	STDMETHOD(Unload)(THIS);

	ULONG _refCount;

	DeviceResources* _deviceResources;

	TextureSurface* _surface;
	std::string _name;

	ComPtr<ID3D11ShaderResourceView> _textureView;
};
