// Copyright (c) 2014 Jérémy Ansel
// Licensed under the MIT license. See LICENSE.txt
// Extended for VR by Leo Reyes (c) 2019

#pragma once

class TextureSurface;

class Direct3DTexture : public IDirect3DTexture
{
public:
	// The CRC of the texture
	//uint32_t crc;
	// Used to tell whether the current texture is part of the aiming HUD and should not be scalled.
	// This flag is set during resource Load, by comparing its CRC with the set of known CRCs.
	bool is_HUD;
	// This flag is set to true if this texture is the triangle pointer
	bool is_TrianglePointer;
	// This flag is set to true if this texture is a font/text
	bool is_Text;
	// This flag is set to true if this is a floating GUI element (like the targeting computer)
	bool is_Floating_GUI;
	// This flag is set to true if this is a regular GUI element
	bool is_GUI;
	// This flag is set to true if this is the main targeting computer HUD background (center, low)
	bool is_TargetingComp;
	// True if this texture is a laser or ion and should be shadeless/bright
	bool is_Laser;
	// True if this is an "illumination" or "light" texture
	bool is_LightTexture;
	// True if this is an Engine Glow texture
	bool is_EngineGlow;
	// TODO: Remove this later, I'm only using this to dump the rebel lasers for debugging purposes
	//bool is_RebelLaser;
	// TODO: Remove this later
	//bool is_SpecialDebug;

	// **** DYNAMIC COCKPIT MEMBERS **** //
	// Textures in the cockpit that can be replaced with new textures
	// Index into g_DCElements that holds the Dynamic Cockpit information for this texture
	int DCElementIndex;
	// True for all textures that can be used as destinations for the dynamic cockpit.
	bool is_DynCockpitDst;
	// True for the light textures loaded in Hi-Res mode that provide additional glow
	bool is_DynCockpitAlphaOverlay;
	// True for all the source HUD textures
	bool is_DC_HUDSource;
	// True for specific DC HUD source textures
	bool is_DC_TargetCompSrc;
	bool is_DC_LeftSensorSrc;
	bool is_DC_RightSensorSrc;
	bool is_DC_RightSensor2Src;
	bool is_DC_ShieldsSrc;
	bool is_DC_SolidMsgSrc;
	bool is_DC_BorderMsgSrc;
	bool is_DC_LaserBoxSrc;
	bool is_DC_IonBoxSrc;
	bool is_DC_BeamBoxSrc;
	bool is_DC_TopLeftSrc;
	bool is_DC_TopRightSrc;

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
