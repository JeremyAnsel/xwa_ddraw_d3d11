// Copyright (c) 2014 Jérémy Ansel
// Licensed under the MIT license. See LICENSE.txt
// Extended for VR by Leo Reyes (c) 2019

#pragma once
#include "Matrices.h"
#include <vector>

// Also found in the Floating_GUI_RESNAME list:
extern const char *DC_TARGET_COMP_SRC_RESNAME;
extern const char *DC_LEFT_SENSOR_SRC_RESNAME;
extern const char *DC_RIGHT_SENSOR_SRC_RESNAME;
extern const char *DC_RIGHT_SENSOR_2_SRC_RESNAME;
extern const char *DC_SHIELDS_SRC_RESNAME;
extern const char *DC_SOLID_MSG_SRC_RESNAME;
extern const char *DC_BORDER_MSG_SRC_RESNAME;
extern const char *DC_LASER_BOX_SRC_RESNAME;
extern const char *DC_ION_BOX_SRC_RESNAME;
extern const char *DC_BEAM_BOX_SRC_RESNAME;
extern const char *DC_TOP_LEFT_SRC_RESNAME;
extern const char *DC_TOP_RIGHT_SRC_RESNAME;

typedef struct Box_struct {
	float x0, y0, x1, y1;
} Box;

typedef struct uvfloat4_struct {
	float x0, y0, x1, y1;
} uvfloat4;

const int LEFT_RADAR_HUD_BOX_IDX		= 0;
const int RIGHT_RADAR_HUD_BOX_IDX	= 1;
const int SHIELDS_HUD_BOX_IDX		= 2;
const int BEAM_HUD_BOX_IDX			= 3;
const int TARGET_HUD_BOX_IDX			= 4;
const int LEFT_MSG_HUD_BOX_IDX		= 5;
const int RIGHT_MSG_HUD_BOX_IDX		= 6;
const int TOP_LEFT_BOX_IDX			= 7;
const int TOP_RIGHT_BOX_IDX			= 8;
const int MAX_HUD_BOXES				= 9;
extern std::vector<const char *>g_HUDRegionNames;
// Convert a string into a *_HUD_BOX_IDX constant
int HUDRegionNameToIndex(char *name);

class DCHUDBox {
public:
	Box coords;
	Box uv_erase_coords;
	uvfloat4 erase_coords;
	bool bLimitsComputed;
};

/*
 * This class stores the coordinates for each HUD texture: left radar, right radar, text
 * boxes, etc. It does not store the individual HUD elements within each HUD texture. For
 * that, look at DCElementSourceBox
 */
class DCHUDBoxes {
public:
	std::vector<DCHUDBox> boxes;

	DCHUDBoxes();

	void Clear() {
		boxes.clear();
	}

	void ResetLimits() {
		for (unsigned int i = 0; i < boxes.size(); i++) 
			boxes[i].bLimitsComputed = false;
	}
};

const int LEFT_RADAR_DC_ELEM_SRC_IDX = 0;
const int RIGHT_RADAR_DC_ELEM_SRC_IDX = 1;
const int LASER_RECHARGE_DC_ELEM_SRC_IDX = 2;
const int SHIELD_RECHARGE_DC_ELEM_SRC_IDX = 3;
const int ENGINE_RECHARGE_DC_ELEM_SRC_IDX = 4;
const int BEAM_RECHARGE_DC_ELEM_SRC_IDX = 5;
const int SHIELDS_DC_ELEM_SRC_IDX = 6;
const int BEAM_DC_ELEM_SRC_IDX = 7;
const int TARGET_COMP_DC_ELEM_SRC_IDX = 8;
const int QUAD_LASERS_L_DC_ELEM_SRC_IDX = 9;
const int QUAD_LASERS_R_DC_ELEM_SRC_IDX = 10;
const int LEFT_MSG_DC_ELEM_SRC_IDX = 11;
const int RIGHT_MSG_DC_ELEM_SRC_IDX = 12;
const int SPEED_N_THROTTLE_DC_ELEM_SRC_IDX = 13;
const int MISSILES_DC_ELEM_SRC_IDX = 14;
const int NAME_TIME_DC_ELEM_SRC_IDX = 15;
const int NUM_CRAFTS_DC_ELEM_SRC_IDX = 16;
const int QUAD_LASERS_BOTH_DC_ELEM_SRC_IDX = 17;
const int DUAL_LASERS_L_DC_ELEM_SRC_IDX = 18;
const int DUAL_LASERS_R_DC_ELEM_SRC_IDX = 19;
const int DUAL_LASERS_BOTH_DC_ELEM_SRC_IDX = 20;
const int B_WING_LASERS_DC_ELEM_SRC_IDX = 21;
const int SIX_LASERS_BOTH_DC_ELEM_SRC_IDX = 22;
const int SIX_LASERS_L_DC_ELEM_SRC_IDX = 23;
const int SIX_LASERS_R_DC_ELEM_SRC_IDX = 24;
const int MAX_DC_SRC_ELEMENTS = 25;
extern std::vector<const char *>g_DCElemSrcNames;
// Convert a string into a *_DC_ELEM_SRC_IDX constant
int DCSrcElemNameToIndex(char *name);

class DCElemSrcBox {
public:
	Box uv_coords;
	Box coords;
	bool bComputed;

	DCElemSrcBox() {
		bComputed = false;
	}
};

/*
 * Stores the uv_coords and pixel coords for each individual HUD element. Examples of HUD elems
 * are:
 * Laser recharge rate, Shield recharage rate, Radars, etc.
 */
class DCElemSrcBoxes {
public:
	std::vector<DCElemSrcBox> src_boxes;

	void Clear() {
		src_boxes.clear();
	}

	DCElemSrcBoxes();
	void Reset() {
		for (unsigned int i = 0; i < src_boxes.size(); i++)
			src_boxes[i].bComputed = false;
	}
};

enum RenderMainColorKeyType
{
	RENDERMAIN_NO_COLORKEY,
	RENDERMAIN_COLORKEY_20,
	RENDERMAIN_COLORKEY_00,
};

class PrimarySurface;
class DepthSurface;
class BackbufferSurface;
class FrontbufferSurface;
class OffscreenSurface;

/* 2D Constant Buffers */
typedef struct MainShadersCBStruct {
	float scale, aspectRatio, parallax, brightness;
	float use_3D;
} MainShadersCBuffer;

typedef struct BarrelPixelShaderCBStruct {
	float k1, k2, k3;
} BarrelPixelShaderCBuffer;

/* 3D Constant Bufffers */
typedef struct VertexShaderCBStruct {
	float viewportScale[4];
	float aspect_ratio, cockpit_threshold, z_override, sz_override;
	float mult_z_override, bPreventTransform, bFullTransform;
} VertexShaderCBuffer;

typedef struct VertexShaderMatrixCBStruct {
	Matrix4 projEye;
	Matrix4 viewMat;
	Matrix4 fullViewMat;
} VertexShaderMatrixCB;

typedef struct HeadPosStruct {
	float x, y, z;
} HeadPos;

typedef struct float4_struct {
	float x, y, z, w;
} float4;

const int MAX_DC_COORDS = 8;
typedef struct PixelShaderCBStruct {
	float brightness;			// Used to control the brightness of some elements -- mostly for ReShade compatibility
	uint32_t DynCockpitSlots;
	uint32_t bUseCoverTexture;
	uint32_t bRenderHUD;
	// 16 bytes

	//uint32_t bAlphaOnly;
	//uint32_t unused[3];

	uvfloat4 src[MAX_DC_COORDS];
	// 4 * MAX_DC_COORDS * 4 = 128
	uvfloat4 dst[MAX_DC_COORDS];
	// 4 * MAX_DC_COORDS * 4 = 128
	uint32_t bgColor[MAX_DC_COORDS];   // Background colors (dynamic cockpit)
	// 4 * MAX_DC_COORDS = 32
	
	uint32_t bIsLaser;
	uint32_t bIsLightTexture;
	uint32_t bIsEngineGlow;
	float ct_brightness;
	// 16 bytes
	// 320 bytes total
} PixelShaderCBuffer;

typedef struct uv_coords_src_dst_struct {
	int src_slot[MAX_DC_COORDS];
	uvfloat4 dst[MAX_DC_COORDS];
	uint32_t uBGColor[MAX_DC_COORDS];
	int numCoords;
} uv_src_dst_coords;

typedef struct uv_coords_struct {
	uvfloat4 src[MAX_DC_COORDS];
	int numCoords;
} uv_coords;

const int MAX_TEXTURE_NAME = 256;
typedef struct dc_element_struct {
	uv_src_dst_coords coords;
	int erase_slots[MAX_DC_COORDS];
	int num_erase_slots;
	char name[MAX_TEXTURE_NAME];
	char coverTextureName[MAX_TEXTURE_NAME];
	ID3D11ShaderResourceView *coverTexture = NULL;
	bool bActive, bNameHasBeenTested;
} dc_element;

typedef struct move_region_coords_struct {
	int region_slot[MAX_HUD_BOXES];
	uvfloat4 dst[MAX_HUD_BOXES];
	int numCoords;
} move_region_coords;

//extern ID3D11ShaderResourceView *g_RebelLaser;

class DeviceResources
{
public:
	DeviceResources();

	HRESULT Initialize();

	HRESULT OnSizeChanged(HWND hWnd, DWORD dwWidth, DWORD dwHeight);

	HRESULT LoadMainResources();

	HRESULT LoadResources();

	void InitInputLayout(ID3D11InputLayout* inputLayout);
	void InitVertexShader(ID3D11VertexShader* vertexShader);
	void InitPixelShader(ID3D11PixelShader* pixelShader);
	void InitTopology(D3D_PRIMITIVE_TOPOLOGY topology);
	void InitRasterizerState(ID3D11RasterizerState* state);
	HRESULT InitSamplerState(ID3D11SamplerState** sampler, D3D11_SAMPLER_DESC* desc);
	HRESULT InitBlendState(ID3D11BlendState* blend, D3D11_BLEND_DESC* desc);
	HRESULT InitDepthStencilState(ID3D11DepthStencilState* depthState, D3D11_DEPTH_STENCIL_DESC* desc);
	void InitVertexBuffer(ID3D11Buffer** buffer, UINT* stride, UINT* offset);
	void InitIndexBuffer(ID3D11Buffer* buffer);
	void InitViewport(D3D11_VIEWPORT* viewport);
	void InitVSConstantBuffer3D(ID3D11Buffer** buffer, const VertexShaderCBuffer* vsCBuffer);
	void InitVSConstantBufferMatrix(ID3D11Buffer** buffer, const VertexShaderMatrixCB* vsCBuffer);
	void InitVSConstantBuffer2D(ID3D11Buffer** buffer, const float parallax, const float aspectRatio, const float scale, const float brightness, const float use_3D);
	void InitPSConstantBuffer2D(ID3D11Buffer** buffer, const float parallax, const float aspectRatio, const float scale, const float brightness);
	void InitPSConstantBufferBarrel(ID3D11Buffer** buffer, const float k1, const float k2, const float k3);
	void InitPSConstantBuffer3D(ID3D11Buffer** buffer, const PixelShaderCBuffer *psConstants);

	HRESULT RenderMain(char* buffer, DWORD width, DWORD height, DWORD bpp, RenderMainColorKeyType useColorKey = RENDERMAIN_COLORKEY_20);

	HRESULT RetrieveBackBuffer(char* buffer, DWORD width, DWORD height, DWORD bpp);

	UINT GetMaxAnisotropy();

	void CheckMultisamplingSupport();

	bool IsTextureFormatSupported(DXGI_FORMAT format);

	DWORD _displayWidth;
	DWORD _displayHeight;
	DWORD _displayBpp;
	DWORD _displayTempWidth;
	DWORD _displayTempHeight;
	DWORD _displayTempBpp;

	D3D_DRIVER_TYPE _d3dDriverType;
	D3D_FEATURE_LEVEL _d3dFeatureLevel;
	ComPtr<ID3D11Device> _d3dDevice;
	//ComPtr<ID3D11Device1> _d3dDevice1;
	ComPtr<ID3D11DeviceContext> _d3dDeviceContext;
	//ComPtr<ID3D11DeviceContext1> _d3dDeviceContext1;
	ComPtr<IDXGISwapChain> _swapChain;
	ComPtr<ID3D11Texture2D> _backBuffer;
	ComPtr<ID3D11Texture2D> _offscreenBuffer;
	ComPtr<ID3D11Texture2D> _offscreenBufferR; // When SteamVR is used, _offscreenBuffer becomes the left eye and this one becomes the right eye
	ComPtr<ID3D11Texture2D> _offscreenBufferDynCockpit;   // Used to render the targeting computer dynamically <-- Need to re-check this claim
	ComPtr<ID3D11Texture2D> _offscreenBufferDynCockpitBG; // Used to render the targeting computer dynamically <-- Need to re-check this claim
	ComPtr<ID3D11Texture2D> _offscreenBufferAsInput;
	ComPtr<ID3D11Texture2D> _offscreenBufferAsInputR; // When SteamVR is used, this is the right eye as input buffer
	ComPtr<ID3D11Texture2D> _offscreenAsInputDynCockpit;   // HUD elements buffer
	ComPtr<ID3D11Texture2D> _offscreenAsInputDynCockpitBG; // HUD element backgrounds buffer
	ComPtr<ID3D11Texture2D> _offscreenBufferAsInputReshade;
	ComPtr<ID3D11Texture2D> _offscreenBufferPost; // This is the output of the barrel effect
	ComPtr<ID3D11Texture2D> _offscreenBufferPostR; // This is the output of the barrel effect for the right image when using SteamVR
	ComPtr<ID3D11Texture2D> _steamVRPresentBuffer; // This is the buffer that will be presented for SteamVR

	ComPtr<ID3D11Texture2D> _reshadeOutput1; // Output from reshade pass 1
	ComPtr<ID3D11Texture2D> _reshadeOutput2; // Output from reshade pass 2
	//ComPtr<ID3D11Texture2D> _offscreenBufferBloomF; // Float buffer (test)
	ComPtr<ID3D11RenderTargetView> _renderTargetView;
	ComPtr<ID3D11RenderTargetView> _renderTargetViewR; // When SteamVR is used, _renderTargetView is the left eye, and this one is the right eye
	ComPtr<ID3D11RenderTargetView> _renderTargetViewDynCockpit; // Used to render the HUD to an offscreen buffer
	ComPtr<ID3D11RenderTargetView> _renderTargetViewDynCockpitBG; // Used to render the HUD to an offscreen buffer
	ComPtr<ID3D11RenderTargetView> _renderTargetViewDynCockpitAsInput; // RTV that writes to _offscreenBufferAsInputDynCockpit directly
	ComPtr<ID3D11RenderTargetView> _renderTargetViewDynCockpitAsInputBG; // RTV that writes to _offscreenBufferAsInputDynCockpitBG directly
	ComPtr<ID3D11RenderTargetView> _renderTargetViewPost;  // Used for the barrel effect
	ComPtr<ID3D11RenderTargetView> _renderTargetViewPostR; // Used for the barrel effect (right image) when SteamVR is used.
	ComPtr<ID3D11RenderTargetView> _renderTargetViewSteamVRResize; // Used for the barrel effect
	ComPtr<ID3D11RenderTargetView> _renderTargetViewReshade1; // Renders to reshadeOutput1
	ComPtr<ID3D11RenderTargetView> _renderTargetViewReshade2; // Renders to reshadeOutput2
	//ComPtr<ID3D11RenderTargetView> _renderTargetViewBloomF; // Renders to _offscreenBufferBloomF

	ComPtr<ID3D11ShaderResourceView> _offscreenAsInputShaderResourceView;
	ComPtr<ID3D11ShaderResourceView> _offscreenAsInputShaderResourceViewR; // When SteamVR is enabled, this is the SRV for the right eye
	ComPtr<ID3D11ShaderResourceView> _offscreenAsInputSRVDynCockpit;   // SRV for HUD elements without background
	ComPtr<ID3D11ShaderResourceView> _offscreenAsInputSRVDynCockpitBG; // SRV for HUD element backgrounds
	ComPtr<ID3D11ShaderResourceView> _offscreenAsInputReshadeSRV;
	ComPtr<ID3D11ShaderResourceView> _reshadeOutput1SRV; // SRV for reshadeOutput1
	ComPtr<ID3D11ShaderResourceView> _reshadeOutput2SRV; // SRV for reshadeOutput2
	//ComPtr<ID3D11ShaderResourceView> _reshadeBloomFSRV; // SRV for _offscreenBufferBloomF
	ComPtr<ID3D11Texture2D> _depthStencilL;
	ComPtr<ID3D11Texture2D> _depthStencilR;
	ComPtr<ID3D11DepthStencilView> _depthStencilViewL;
	ComPtr<ID3D11DepthStencilView> _depthStencilViewR;

	ComPtr<ID3D11VertexShader> _mainVertexShader;
	ComPtr<ID3D11InputLayout> _mainInputLayout;
	ComPtr<ID3D11PixelShader> _mainPixelShader;
	ComPtr<ID3D11PixelShader> _mainPixelShaderBpp2ColorKey20;
	ComPtr<ID3D11PixelShader> _mainPixelShaderBpp2ColorKey00;
	ComPtr<ID3D11PixelShader> _mainPixelShaderBpp4ColorKey20;
	ComPtr<ID3D11PixelShader> _barrelPixelShader;
	ComPtr<ID3D11PixelShader> _bloomPrepassPS;
	ComPtr<ID3D11PixelShader> _bloomHGaussPS;
	ComPtr<ID3D11PixelShader> _bloomVGaussPS;
	ComPtr<ID3D11PixelShader> _bloomCombinePS;
	ComPtr<ID3D11PixelShader> _singleBarrelPixelShader;
	ComPtr<ID3D11RasterizerState> _mainRasterizerState;
	ComPtr<ID3D11SamplerState> _mainSamplerState;
	ComPtr<ID3D11BlendState> _mainBlendState;
	ComPtr<ID3D11DepthStencilState> _mainDepthState;
	ComPtr<ID3D11Buffer> _mainVertexBuffer;
	ComPtr<ID3D11Buffer> _steamVRPresentVertexBuffer; // Used in SteamVR mode to correct the image presented on the monitor
	ComPtr<ID3D11Buffer> _mainIndexBuffer;
	ComPtr<ID3D11Texture2D> _mainDisplayTexture;
	ComPtr<ID3D11ShaderResourceView> _mainDisplayTextureView;
	ComPtr<ID3D11Texture2D> _mainDisplayTextureTemp;
	ComPtr<ID3D11ShaderResourceView> _mainDisplayTextureViewTemp;

	ComPtr<ID3D11VertexShader> _vertexShader;
	ComPtr<ID3D11VertexShader> _sbsVertexShader;
	ComPtr<ID3D11VertexShader> _passthroughVertexShader;
	ComPtr<ID3D11InputLayout> _inputLayout;
	ComPtr<ID3D11PixelShader> _pixelShaderTexture;
	ComPtr<ID3D11PixelShader> _pixelShaderSolid;
	ComPtr<ID3D11RasterizerState> _rasterizerState;
	ComPtr<ID3D11Buffer> _VSConstantBuffer;
	ComPtr<ID3D11Buffer> _VSMatrixBuffer;
	ComPtr<ID3D11Buffer> _PSConstantBuffer;
	ComPtr<ID3D11Buffer> _barrelConstantBuffer;
	ComPtr<ID3D11Buffer> _mainShadersConstantBuffer;

	BOOL _useAnisotropy;
	BOOL _useMultisampling;
	DXGI_SAMPLE_DESC _sampleDesc;
	UINT _backbufferWidth;
	UINT _backbufferHeight;
	DXGI_RATIONAL _refreshRate;
	bool _are16BppTexturesSupported;
	bool _use16BppMainDisplayTexture;
	DWORD _mainDisplayTextureBpp;

	float clearColor[4];
	float clearDepth;
	bool sceneRendered;
	bool sceneRenderedEmpty;
	bool inScene;
	bool inSceneBackbufferLocked;

	PrimarySurface* _primarySurface;
	DepthSurface* _depthSurface;
	BackbufferSurface* _backbufferSurface;
	FrontbufferSurface* _frontbufferSurface;
	OffscreenSurface* _offscreenSurface;
};
