// Copyright (c) 2014 Jérémy Ansel
// Licensed under the MIT license. See LICENSE.txt
// Extended for VR by Leo Reyes (c) 2019

#pragma once

class TextureSurface;

#define TRIANGLE_PTR_CRC 0x6cf63265		// Triangle that indicates where is the target (master branch)
#define WARHEAD_CRC 0xa4870ab3			// Main Warhead HUD (master branch)

class Direct3DTexture : public IDirect3DTexture
{
public:
	// The CRC of the texture
	uint32_t crc;
	// Used to tell whether the current texture is part of the aiming HUD and should not be scalled.
	// This flag is set during resource Load, by comparing its CRC with the set of known CRCs.
	bool is_HUD;
	// This flag is set to true if this texture is the triangle pointer
	bool is_TrianglePointer;
	// This flag is set to true if this texture is a font/text
	bool is_Text;
	// This flag is set to true if this is a floating GUI element (like the targetting computer)
	bool is_Floating_GUI;
	// This flag is set to true if this is a regular GUI element
	bool is_GUI;

	Direct3DTexture(DeviceResources* deviceResources, TextureSurface* surface);

	int GetWidth();
	int GetHeight();

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

	ComPtr<ID3D11ShaderResourceView> _textureView;
};
