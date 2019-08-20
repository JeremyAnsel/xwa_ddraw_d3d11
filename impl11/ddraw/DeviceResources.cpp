// Copyright (c) 2014 Jérémy Ansel
// Licensed under the MIT license. See LICENSE.txt
// Extended for VR by Leo Reyes (c) 2019

#include "common.h"
#include "DeviceResources.h"

#ifdef _DEBUG
#include "../Debug/MainVertexShader.h"
#include "../Debug/MainPixelShader.h"
#include "../Debug/MainPixelShaderBpp2ColorKey20.h"
#include "../Debug/MainPixelShaderBpp2ColorKey00.h"
#include "../Debug/MainPixelShaderBpp4ColorKey20.h"
#include "../Debug/BarrelPixelShader.h"
#include "../Debug/SingleBarrelPixelShader.h"
#include "../Debug/VertexShader.h"
#include "../Release/PassthroughVertexShader.h"
#include "../Debug/SBSVertexShader.h"
#include "../Debug/PixelShaderTexture.h"
#include "../Debug/PixelShaderSolid.h"
#include "../Release/BloomPrePassPS.h"
#include "../Release/BloomHGaussPS.h"
#include "../Release/BloomVGaussPS.h"
#include "../Release/BloomCombinePS.h"
#else
#include "../Release/MainVertexShader.h"
#include "../Release/MainPixelShader.h"
#include "../Release/MainPixelShaderBpp2ColorKey20.h"
#include "../Release/MainPixelShaderBpp2ColorKey00.h"
#include "../Release/MainPixelShaderBpp4ColorKey20.h"
#include "../Release/BarrelPixelShader.h"
#include "../Release/SingleBarrelPixelShader.h"
#include "../Release/VertexShader.h"
#include "../Release/PassthroughVertexShader.h"
#include "../Release/SBSVertexShader.h"
#include "../Release/PixelShaderTexture.h"
#include "../Release/PixelShaderSolid.h"
#include "../Release/BloomPrePassPS.h"
#include "../Release/BloomHGaussPS.h"
#include "../Release/BloomVGaussPS.h"
#include "../Release/BloomCombinePS.h"
#endif

#include <WICTextureLoader.h>
#include <headers/openvr.h>
#include <vector>
//#include <assert.h>

bool g_bWndProcReplaced = false;
bool ReplaceWindowProc(HWND ThisWindow);
extern MainShadersCBuffer g_MSCBuffer;
extern BarrelPixelShaderCBuffer g_BPSCBuffer;
extern float g_fConcourseScale, g_fConcourseAspectRatio, g_fTechLibraryParallax, g_fBrightness;
extern bool /* g_bRendering3D, */ g_bDumpDebug, g_bOverrideAspectRatio;
extern int g_iPresentCounter;
int g_iDraw2DCounter = 0;
extern bool g_bEnableVR, g_bForceViewportChange;
extern Matrix4 g_fullMatrixLeft, g_fullMatrixRight;
extern VertexShaderMatrixCB g_VSMatrixCB;
extern std::vector<dc_element> g_DCElements;
extern char g_sCurrentCockpit[128];

extern DCHUDBoxes g_DCHUDBoxes;
extern DCElemSrcBoxes g_DCElemSrcBoxes;

extern bool g_bReshadeEnabled, g_bBloomEnabled;

extern float g_fHUDDepth;
extern bool g_bHUDVerticesReady;
extern ID3D11Buffer *g_HUDVertexBuffer, *g_ClearHUDVertexBuffer, *g_ClearFullScreenHUDVertexBuffer;
extern float g_fCurInGameWidth, g_fCurInGameHeight, g_fCurScreenWidth, g_fCurScreenHeight;

// SteamVR
#include <headers/openvr.h>
extern bool g_bSteamVRInitialized, g_bUseSteamVR, g_bEnableVR;
extern uint32_t g_steamVRWidth, g_steamVRHeight;
DWORD g_FullScreenWidth = 0, g_FullScreenHeight = 0;
bool InitSteamVR();

/* The different types of Constant Buffers used in the Vertex Shader: */
typedef enum {
	VS_CONSTANT_BUFFER_NONE,
	VS_CONSTANT_BUFFER_2D,
	VS_CONSTANT_BUFFER_3D,
} VSConstantBufferType;
VSConstantBufferType g_LastVSConstantBufferSet = VS_CONSTANT_BUFFER_NONE;

/* The different types of Constant Buffers used in the Pixel Shader: */
typedef enum {
	PS_CONSTANT_BUFFER_NONE,
	PS_CONSTANT_BUFFER_BARREL,
	PS_CONSTANT_BUFFER_2D,
	PS_CONSTANT_BUFFER_3D,
} PSConstantBufferType;
PSConstantBufferType g_LastPSConstantBufferSet = PS_CONSTANT_BUFFER_NONE;

extern bool g_bDynCockpitEnabled;
//bool g_bNewCockpitTexturesLoaded = false;
ComPtr<ID3D11ShaderResourceView> g_NewHUDLeftRadar = NULL;
ComPtr<ID3D11ShaderResourceView> g_NewHUDRightRadar = NULL;

//ComPtr<ID3D11ShaderResourceView> g_NewDCTargetCompCover = NULL;
//ComPtr<ID3D11ShaderResourceView> g_NewDCLeftRadarCover = NULL;
//ComPtr<ID3D11ShaderResourceView> g_NewDCRightRadarCover = NULL;
//ComPtr<ID3D11ShaderResourceView> g_NewDCShieldsCover = NULL;
//ComPtr<ID3D11ShaderResourceView> g_NewDCLasersCover = NULL;
//ComPtr<ID3D11ShaderResourceView> g_NewDCFrontPanelCover = NULL;

FILE *g_DebugFile = NULL;

/* SteamVR HMD */
extern vr::IVRSystem *g_pHMD;
extern vr::IVRCompositor *g_pVRCompositor;
extern bool g_bSteamVREnabled, g_bUseSteamVR;
//void ProcessVREvent(const vr::VREvent_t & event);
//vr::TrackedDevicePose_t m_rTrackedDevicePose[vr::k_unMaxTrackedDeviceCount];

//ID3D11ShaderResourceView *g_RebelLaser = NULL;

void ClearDynCockpitVector(std::vector<dc_element> &DCElements);

void log_err(const char *format, ...)
{
	char buf[120];

	if (g_DebugFile == NULL)
		fopen_s(&g_DebugFile, "./vrerror.log", "wt");

	va_list args;
	va_start(args, format);

	vsprintf_s(buf, 120, format, args);
	fprintf(g_DebugFile, buf);

	va_end(args);
}

void close_error_file() {
	if (g_DebugFile != NULL)
		fclose(g_DebugFile);
}

void log_err_desc(char *step, HWND hWnd, HRESULT hr, CD3D11_TEXTURE2D_DESC desc) {
	log_err("step: %s\n", step);
	log_err("hWnd: 0x%x\n", hWnd);
	log_err("TEXTURE2D_DESC:\n");
	log_err("hr: 0x%x, %s\n", hr, _com_error(hr).ErrorMessage());
	log_err("ArraySize: %d\n", desc.ArraySize);
	log_err("Format: %d\n", desc.Format);
	log_err("BindFlags: %d\n", desc.BindFlags);
	log_err("CPUAccessFlags: %d\n", desc.CPUAccessFlags);
	log_err("MipLevels: %d\n", desc.MipLevels);
	log_err("MiscFlags: %d\n", desc.MiscFlags);
	log_err("SampleDesc: %d\n", desc.SampleDesc);
	log_err("Usage: %d\n", desc.Usage);
	log_err("Width: %d\n", desc.Width);
	log_err("Height: %d\n", desc.Height);
	log_err("Sample Count: %d\n", desc.SampleDesc.Count);
	log_err("Sample Quality: %d\n", desc.SampleDesc.Quality);
}

void log_shaderres_view(char *step, HWND hWnd, HRESULT hr, D3D11_SHADER_RESOURCE_VIEW_DESC desc) {
	log_err("step: %s\n", step);
	log_err("hWnd: 0x%x\n", hWnd);
	log_err("SHADER_RESOURCE_VIEW_DESC:\n");
	log_err("hr: 0x%x, %s\n", hr, _com_error(hr).ErrorMessage());
	log_err("Format: %d\n", desc.Format);
	log_err("ViewDimension: %d\n", desc.ViewDimension);
	log_err("MostDetailedMip: %d\n", desc.Texture2D.MostDetailedMip);
	log_err("MipLevels: %d\n", desc.Texture2D.MipLevels);
}

struct MainVertex
{
	float pos[2];
	float tex[2];

	MainVertex()
	{
		this->pos[0] = 0;
		this->pos[1] = 0;
		this->tex[0] = 0;
		this->tex[1] = 0;
	}

	MainVertex(float x, float y, float u, float v)
	{
		this->pos[0] = x;
		this->pos[1] = y;
		this->tex[0] = u;
		this->tex[1] = v;
	}
};

const char *DC_TARGET_COMP_SRC_RESNAME		= "dat,12000,1100,";
const char *DC_LEFT_SENSOR_SRC_RESNAME		= "dat,12000,4500,";
const char *DC_RIGHT_SENSOR_SRC_RESNAME		= "dat,12000,4600,";
const char *DC_RIGHT_SENSOR_2_SRC_RESNAME	= "dat,12000,400,";
const char *DC_SHIELDS_SRC_RESNAME			= "dat,12000,4300,";
const char *DC_SOLID_MSG_SRC_RESNAME			= "dat,12000,100,";
const char *DC_BORDER_MSG_SRC_RESNAME		= "dat,12000,200,";
const char *DC_LASER_BOX_SRC_RESNAME			= "dat,12000,2300,";
const char *DC_ION_BOX_SRC_RESNAME			= "dat,12000,2500,";
const char *DC_BEAM_BOX_SRC_RESNAME			= "dat,12000,4400,";
const char *DC_TOP_LEFT_SRC_RESNAME			= "dat,12000,2700,";
const char *DC_TOP_RIGHT_SRC_RESNAME			= "dat,12000,2800,";


std::vector<const char *>g_HUDRegionNames = {
	"LEFT_SENSOR_REGION",		// 0
	"RIGHT_SENSOR_REGION",		// 1
	"SHIELDS_REGION",			// 2
	"BEAM_REGION",				// 3
	"TARGET_AND_LASERS_REGION",	// 4
	"LEFT_TEXT_BOX_REGION",		// 5
	"RIGHT_TEXT_BOX_REGION",		// 6
	"TOP_LEFT_REGION",			// 7
	"TOP_RIGHT_REGION"			// 8
};
//const int MAX_HUD_BOXES = (int )g_HUDRegionNames.size();

std::vector<const char *>g_DCElemSrcNames = {
	"LEFT_SENSOR_SRC",			// 0
	"RIGHT_SENSOR_SRC",			// 1
	"LASER_RECHARGE_SRC",		// 2
	"SHIELD_RECHARGE_SRC",		// 3
	"ENGINE_POWER_SRC",			// 4
	"BEAM_RECHARGE_SRC",			// 5
	"SHIELDS_SRC",				// 6
	"BEAM_LEVEL_SRC"	,			// 7
	"TARGETING_COMPUTER_SRC",	// 8
	"QUAD_LASERS_LEFT_SRC",		// 9
	"QUAD_LASERS_RIGHT_SRC",		// 10
	"LEFT_TEXT_BOX_SRC",			// 11
	"RIGHT_TEXT_BOX_SRC",		// 12
	"SPEED_THROTTLE_SRC",		// 13
	"MISSILES_SRC",				// 14
	"NAME_TIME_SRC",				// 15
	"NUM_SHIPS_SRC",				// 16
	"QUAD_LASERS_BOTH_SRC",		// 17
	"DUAL_LASERS_L_SRC",			// 18
	"DUAL_LASERS_R_SRC",			// 19
	"DUAL_LASERS_BOTH_SRC",		// 20
	"B_WING_LASERS_SRC",			// 21
	"SIX_LASERS_BOTH_SRC",		// 22
	"SIX_LASERS_L_SRC",			// 23
	"SIX_LASERS_R_SRC",			// 24
};
//const int MAX_DC_SRC_ELEMENTS = (int )g_DCElemSrcNames.size();

int HUDRegionNameToIndex(char *name) {
	if (name == NULL || name[0] == '\0')
		return -1;
	for (int i = 0; i < (int )g_HUDRegionNames.size(); i++)
		if (_stricmp(name, g_HUDRegionNames[i]) == 0)
			return i;
	return -1;
}

int DCSrcElemNameToIndex(char *name) {
	if (name == NULL || name[0] == '\0')
		return -1;
	for (int i = 0; i < (int )g_DCElemSrcNames.size(); i++)
		if (_stricmp(name, g_DCElemSrcNames[i]) == 0)
			return i;
	return -1;
}

DeviceResources::DeviceResources()
{
	this->_primarySurface = nullptr;
	this->_depthSurface = nullptr;
	this->_backbufferSurface = nullptr;
	this->_frontbufferSurface = nullptr;
	this->_offscreenSurface = nullptr;

	this->_useAnisotropy = g_config.AnisotropicFilteringEnabled ? TRUE : FALSE;
	this->_useMultisampling = g_config.MultisamplingAntialiasingEnabled ? TRUE : FALSE;

	this->_sampleDesc.Count = 1;
	this->_sampleDesc.Quality = 0;

	this->_backbufferWidth = 0;
	this->_backbufferHeight = 0;
	this->_refreshRate = { 0, 1 };
	this->_are16BppTexturesSupported = false;
	this->_use16BppMainDisplayTexture = false;

	const float color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	memcpy(this->clearColor, &color, sizeof(color));

	this->clearDepth = 1.0f;

	this->sceneRendered = false;
	this->sceneRenderedEmpty = false;
	this->inScene = false;
}

HRESULT DeviceResources::Initialize()
{
	HRESULT hr;

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		//D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};

	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	UINT createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#ifdef _DEBUG
	//createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	this->_d3dDriverType = D3D_DRIVER_TYPE_HARDWARE;

	hr = D3D11CreateDevice(nullptr, this->_d3dDriverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
		D3D11_SDK_VERSION, &this->_d3dDevice, &this->_d3dFeatureLevel, &this->_d3dDeviceContext);

	if (FAILED(hr))
	{
		this->_d3dDriverType = D3D_DRIVER_TYPE_WARP;
		hr = D3D11CreateDevice(nullptr, this->_d3dDriverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels, D3D11_SDK_VERSION, &this->_d3dDevice, &this->_d3dFeatureLevel, &this->_d3dDeviceContext);
	}

	/*
	if (SUCCEEDED(hr)) {
		log_debug("[DBG] Getting the DX11.1 device");
		hr = this->_d3dDevice->QueryInterface(__uuidof(ID3D11Device1),
			reinterpret_cast<void**>(&this->_d3dDevice1));
		log_debug("[DBG] hr: 0x%x", hr);
	} else {
		this->_d3dDevice1 = NULL;
	}

	if (SUCCEEDED(hr)) {
		log_debug("[DBG] Getting ID3D11DeviceContext1 ");
		hr = this->_d3dDeviceContext->QueryInterface(__uuidof(ID3D11DeviceContext1),
			reinterpret_cast<void**>(&this->_d3dDeviceContext1));
		log_debug("[DBG] hr: 0x%x", hr);
	} else {
		this->_d3dDeviceContext1 = NULL;
	}
	*/

	if (SUCCEEDED(hr))
	{
		this->CheckMultisamplingSupport();
	}

	if (SUCCEEDED(hr))
	{
		this->_are16BppTexturesSupported =
			this->IsTextureFormatSupported(DXGI_FORMAT_B4G4R4A4_UNORM)
			&& this->IsTextureFormatSupported(DXGI_FORMAT_B5G5R5A1_UNORM)
			&& this->IsTextureFormatSupported(DXGI_FORMAT_B5G6R5_UNORM);

		this->_use16BppMainDisplayTexture = this->_are16BppTexturesSupported && (this->_d3dFeatureLevel >= D3D_FEATURE_LEVEL_10_0);
	}

	if (SUCCEEDED(hr))
	{
		hr = this->LoadMainResources();
	}

	if (SUCCEEDED(hr))
	{
		hr = this->LoadResources();
	}

	if (FAILED(hr))
	{
		static bool messageShown = false;

		if (!messageShown)
		{
			MessageBox(nullptr, _com_error(hr).ErrorMessage(), __FUNCTION__, MB_ICONERROR);
		}

		messageShown = true;
	}

	return hr;
}

void BuildHUDVertexBuffer(ComPtr<ID3D11Device> device, UINT width, UINT height) {
	HRESULT hr;
	D3DCOLOR color = 0xFFFFFFFF; // AABBGGRR
	//float depth = g_fHUDDepth;
	// The values for rhw_depth and sz_depth were taken from an actual sample from the X-Wing's front panel
	float rhw_depth = 34.0f;
	float sz_depth  = 0.98f;
	D3DTLVERTEX HUDVertices[6] = { 0 };

	HUDVertices[0].sx  = 0;
	HUDVertices[0].sy  = 0;
	HUDVertices[0].sz  = sz_depth;
	HUDVertices[0].rhw = rhw_depth;
	HUDVertices[0].tu  = 0;
	HUDVertices[0].tv  = 0;
	HUDVertices[0].color = color;

	HUDVertices[1].sx = (float)width;
	HUDVertices[1].sy = 0;
	HUDVertices[1].sz  = sz_depth;
	HUDVertices[1].rhw = rhw_depth;
	HUDVertices[1].tu  = 1;
	HUDVertices[1].tv  = 0;
	HUDVertices[1].color = color;
	
	HUDVertices[2].sx = (float)width;
	HUDVertices[2].sy = (float)height;
	HUDVertices[2].sz  = sz_depth;
	HUDVertices[2].rhw = rhw_depth;
	HUDVertices[2].tu  = 1;
	HUDVertices[2].tv  = 1;
	HUDVertices[2].color = color;
	
	HUDVertices[3].sx = (float)width;
	HUDVertices[3].sy = (float)height;
	HUDVertices[3].sz  = sz_depth;
	HUDVertices[3].rhw = rhw_depth;
	HUDVertices[3].tu  = 1;
	HUDVertices[3].tv  = 1;
	HUDVertices[3].color = color;
	
	HUDVertices[4].sx = 0;
	HUDVertices[4].sy = (float)height;
	HUDVertices[4].sz  = sz_depth;
	HUDVertices[4].rhw = rhw_depth;
	HUDVertices[4].tu  = 0;
	HUDVertices[4].tv  = 1;
	HUDVertices[4].color = color;
	
	HUDVertices[5].sx  = 0;
	HUDVertices[5].sy  = 0;
	HUDVertices[5].sz  = sz_depth;
	HUDVertices[5].rhw = rhw_depth;
	HUDVertices[5].tu  = 0;
	HUDVertices[5].tv  = 0;
	HUDVertices[5].color = color;	

	/* Create the VertexBuffer if necessary */
	if (g_HUDVertexBuffer != NULL) {
		g_HUDVertexBuffer->Release();
		g_HUDVertexBuffer = NULL;
	}

	if (g_ClearFullScreenHUDVertexBuffer != NULL) {
		g_ClearFullScreenHUDVertexBuffer->Release();
		g_ClearFullScreenHUDVertexBuffer = NULL;
	}

	if (g_ClearHUDVertexBuffer != NULL) {
		g_ClearHUDVertexBuffer->Release();
		g_ClearHUDVertexBuffer = NULL;
	}

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(D3DTLVERTEX) * ARRAYSIZE(HUDVertices);
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;
	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
	vertexBufferData.pSysMem = HUDVertices;
	hr = device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &g_HUDVertexBuffer);
	if (FAILED(hr)) {
		log_debug("[DBG] Could not create g_HUDVertexBuffer");
		g_bHUDVerticesReady = false;
	}

	// Change the color to 0 to create a vertex buffer that clears the whole HUD
	for (int i = 0; i < 6; i++)
		HUDVertices[i].color = 0;
	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
	vertexBufferData.pSysMem = HUDVertices;
	hr = device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &g_ClearFullScreenHUDVertexBuffer);
	if (FAILED(hr)) {
		log_debug("[DBG] Could not create g_ClearFullScreenHUDVertexBuffer");
		g_bHUDVerticesReady = false;
	}

	// Build the vertex buffer that will be used to clear areas of the offscreen DC buffer:
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.StructureByteStride = 0;
	hr = device->CreateBuffer(&vertexBufferDesc, nullptr, &g_ClearHUDVertexBuffer);
	if (FAILED(hr)) {
		log_debug("[DBG] Could not create g_ClearHUDVertexBuffer");
		g_bHUDVerticesReady = false;
	}

	g_bHUDVerticesReady = true;
}

HRESULT DeviceResources::OnSizeChanged(HWND hWnd, DWORD dwWidth, DWORD dwHeight)
{
	/*
	 * When the concourse is displayed, dwWidth,dwHeight = 640x480 regardless of actual in-game res or screen res.
	 * When 3D content is displayed, dwWidth,dwHeight = in-game resolution (1280x1024, 1600x1200, etc)
	 */
	HRESULT hr;
	char* step = "";
	//log_debug("[DBG] OnSizeChanged called");
	// Generic VR Initialization
	// Replace the game's WndProc
	if (!g_bWndProcReplaced) {
		ReplaceWindowProc(hWnd);
		g_bWndProcReplaced = true;
	}

	//log_debug("[DBG] OnSizeChanged, dwWidth,Height: %d, %d", dwWidth, dwHeight);

	if (g_bUseSteamVR) {
		// dwWidth, dwHeight are the in-game's resolution
		// When using SteamVR, let's override the size with the recommended size
		dwWidth = g_steamVRWidth;
		dwHeight = g_steamVRHeight;
	}

	// Reset the present counter
	g_iPresentCounter = 0;

	this->_depthStencilViewL.Release();
	this->_depthStencilViewR.Release();
	this->_depthStencilL.Release();
	this->_depthStencilR.Release();
	this->_renderTargetView.Release();
	this->_renderTargetViewPost.Release();
	this->_offscreenAsInputShaderResourceView.Release();
	this->_offscreenBuffer.Release();
	this->_offscreenBufferAsInput.Release();
	this->_offscreenBufferPost.Release();
	if (g_bUseSteamVR) {
		this->_offscreenBufferR.Release();
		this->_offscreenBufferAsInputR.Release();
		this->_offscreenBufferPostR.Release();
		this->_offscreenAsInputShaderResourceViewR.Release();
		this->_renderTargetViewR.Release();
		this->_renderTargetViewPostR.Release();
		this->_steamVRPresentBuffer.Release();
		this->_renderTargetViewSteamVRResize.Release();
	}
	if (g_bDynCockpitEnabled) {
		// Reset the HUD boxes: this will force a re-compute of the boxes and the DC elements
		g_DCHUDBoxes.ResetLimits();
		// Reset the Source DC elements so that we know when they get re-computed.
		g_DCElemSrcBoxes.Reset();
		// Reset the cockpit name
		g_sCurrentCockpit[0] = 0;
		// Reset the active slots in g_DCElements
		int size = (int)g_DCElements.size();
		for (int i = 0; i < size; i++)
		{
			dc_element *elem = &g_DCElements[i];
			if (elem->bActive) {
				if (elem->coverTexture != NULL) {
					//log_debug("[DBG] [DC] Releasing %s", elem->coverTextureName);
					elem->coverTexture->Release();
					elem->coverTexture = NULL;
				}
				elem->bActive = false;
				elem->bNameHasBeenTested = false;
			}
		}
		this->_renderTargetViewDynCockpit.Release();
		this->_renderTargetViewDynCockpitBG.Release();
		this->_renderTargetViewDynCockpitAsInput.Release();
		this->_renderTargetViewDynCockpitAsInputBG.Release();
		this->_offscreenBufferDynCockpit.Release();
		this->_offscreenBufferDynCockpitBG.Release();
		this->_offscreenAsInputDynCockpit.Release();
		this->_offscreenAsInputDynCockpitBG.Release();
		this->_offscreenAsInputSRVDynCockpit.Release();
		this->_offscreenAsInputSRVDynCockpitBG.Release();
		// Reshade-test
		//this->_offscreenBufferBloomF.Release();
		//this->_renderTargetViewBloomF.Release();
		//this->_reshadeBloomFSRV.Release();
	}
	if (g_bReshadeEnabled) {
		this->_offscreenBufferAsInputReshade.Release();
		this->_offscreenAsInputReshadeSRV.Release();
		this->_renderTargetViewReshade1.Release();
		this->_renderTargetViewReshade2.Release();
		this->_reshadeOutput1.Release();
		this->_reshadeOutput2.Release();
		//this->_reshadeOutput1AsInput.Release();
		//this->_reshadeOutput2AsInput.Release();
		this->_reshadeOutput1SRV.Release();
		this->_reshadeOutput2SRV.Release();
	}

	this->_backBuffer.Release();
	this->_swapChain.Release();
	//UnloadNewCockpitTextures();

	this->_refreshRate = { 0, 1 };

	if (hWnd != nullptr)
	{
		step = "RenderTarget SwapChain";
		ComPtr<IDXGIDevice> dxgiDevice;
		ComPtr<IDXGIAdapter> dxgiAdapter;
		ComPtr<IDXGIOutput> dxgiOutput;

		hr = this->_d3dDevice.As(&dxgiDevice);

		if (SUCCEEDED(hr))
		{
			hr = dxgiDevice->GetAdapter(&dxgiAdapter);

			if (SUCCEEDED(hr))
			{
				hr = dxgiAdapter->EnumOutputs(0, &dxgiOutput);
			}
		}

		DXGI_MODE_DESC md{};

		if (SUCCEEDED(hr))
		{
			md.Format = DXGI_FORMAT_B8G8R8A8_UNORM;

			hr = dxgiOutput->FindClosestMatchingMode(&md, &md, nullptr);
		}

		if (SUCCEEDED(hr))
		{
			DXGI_SWAP_CHAIN_DESC sd{};
			sd.BufferCount = 2;
			sd.SwapEffect = DXGI_SWAP_EFFECT_SEQUENTIAL;
			//if (g_bUseSteamVR) {
			//	sd.BufferDesc.Width = g_steamVRWidth;
			//	sd.BufferDesc.Height = g_steamVRHeight;
			//}
			//else {
				sd.BufferDesc.Width  = 0;
				sd.BufferDesc.Height = 0;
			//}
			sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
			sd.BufferDesc.RefreshRate = md.RefreshRate;
			sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			sd.OutputWindow = hWnd;
			sd.SampleDesc.Count = 1;
			sd.SampleDesc.Quality = 0;
			sd.Windowed = TRUE;

			ComPtr<IDXGIFactory> dxgiFactory;
			//ComPtr<IDXGIFactory1> dxgiFactory;
			hr = dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory));

			if (SUCCEEDED(hr))
			{
				hr = dxgiFactory->CreateSwapChain(this->_d3dDevice, &sd, &this->_swapChain);
			}
			
			if (SUCCEEDED(hr))
			{
				this->_refreshRate = sd.BufferDesc.RefreshRate;
			}

			if (SUCCEEDED(hr))
			{
				DXGI_SWAP_CHAIN_DESC sd{};
				this->_swapChain->GetDesc(&sd);
				g_FullScreenWidth = sd.BufferDesc.Width;
				g_FullScreenHeight = sd.BufferDesc.Height;
				g_fCurScreenWidth = (float)sd.BufferDesc.Width;
				g_fCurScreenHeight = (float)sd.BufferDesc.Height;
				//log_debug("[DBG] Fullscreen size: %d, %d", g_FullScreenWidth, g_FullScreenHeight);
			}
		}

		if (SUCCEEDED(hr))
		{
			hr = this->_swapChain->GetBuffer(0, IID_PPV_ARGS(&this->_backBuffer));
		}
	}
	else
	{
		step = "RenderTarget Texture2D";

		CD3D11_TEXTURE2D_DESC backBufferDesc(
			DXGI_FORMAT_B8G8R8A8_UNORM,
			dwWidth,
			dwHeight,
			1,
			1,
			D3D11_BIND_RENDER_TARGET,
			D3D11_USAGE_DEFAULT,
			0,
			1,
			0,
			0);

		hr = this->_d3dDevice->CreateTexture2D(&backBufferDesc, nullptr, &this->_backBuffer);
	}

	if (SUCCEEDED(hr))
	{
		D3D11_TEXTURE2D_DESC backBufferDesc;
		this->_backBuffer->GetDesc(&backBufferDesc);

		this->_backbufferWidth = backBufferDesc.Width;
		this->_backbufferHeight = backBufferDesc.Height;
		//g_fCurScreenWidth = _backbufferWidth;
		//g_fCurScreenHeight = _backbufferHeight;
		//log_debug("[DBG] Screen size: %f, %f", g_fCurScreenWidth, g_fCurScreenHeight);
	}

	if (SUCCEEDED(hr))
	{
		CD3D11_TEXTURE2D_DESC desc(
			DXGI_FORMAT_B8G8R8A8_UNORM,
			this->_backbufferWidth,
			this->_backbufferHeight,
			1,
			1,
			D3D11_BIND_RENDER_TARGET,
			D3D11_USAGE_DEFAULT,
			0,
			this->_sampleDesc.Count,
			this->_sampleDesc.Quality,
			0);

		step = "OffscreenBuffer";
		hr = this->_d3dDevice->CreateTexture2D(&desc, nullptr, &this->_offscreenBuffer);
		if (FAILED(hr)) {
			log_err("dwWidth, Height: %u, %u\n", dwWidth, dwHeight);
			log_err_desc(step, hWnd, hr, desc);
			goto out;
		}

		if (g_bUseSteamVR) {
			step = "OffscreenBufferR";
			hr = this->_d3dDevice->CreateTexture2D(&desc, nullptr, &this->_offscreenBufferR);
			if (FAILED(hr)) {
				log_err("dwWidth, Height: %u, %u\n", dwWidth, dwHeight);
				log_err_desc(step, hWnd, hr, desc);
				goto out;
			}
		}

		step = "_offscreenBufferPost";
		// offscreenBufferPost should be just like offscreenBuffer because it will be bound as a renderTarget
		hr = this->_d3dDevice->CreateTexture2D(&desc, nullptr, &this->_offscreenBufferPost);
		if (FAILED(hr)) {
			log_err("dwWidth, Height: %u, %u\n", dwWidth, dwHeight);
			log_err_desc(step, hWnd, hr, desc);
			goto out;
		}

		if (g_bDynCockpitEnabled) {
			step = "_offscreenBufferDynCockpit";
			// _offscreenBufferDynCockpit should be just like offscreenBuffer because it will be used as a renderTarget
			hr = this->_d3dDevice->CreateTexture2D(&desc, nullptr, &this->_offscreenBufferDynCockpit);
			if (FAILED(hr)) {
				log_err("dwWidth, Height: %u, %u\n", dwWidth, dwHeight);
				log_err_desc(step, hWnd, hr, desc);
				goto out;
			}

			step = "_offscreenBufferDynCockpitBG";
			// _offscreenBufferDynCockpit should be just like offscreenBuffer because it will be used as a renderTarget
			hr = this->_d3dDevice->CreateTexture2D(&desc, nullptr, &this->_offscreenBufferDynCockpitBG);
			if (FAILED(hr)) {
				log_err("dwWidth, Height: %u, %u\n", dwWidth, dwHeight);
				log_err_desc(step, hWnd, hr, desc);
				goto out;
			}
		}

		if (g_bUseSteamVR) {
			step = "_offscreenBufferPostR";
			hr = this->_d3dDevice->CreateTexture2D(&desc, nullptr, &this->_offscreenBufferPostR);
			if (FAILED(hr)) {
				log_err("dwWidth, Height: %u, %u\n", dwWidth, dwHeight);
				log_err_desc(step, hWnd, hr, desc);
				goto out;
			}

			step = "_steamVRPresentBuffer";
			hr = this->_d3dDevice->CreateTexture2D(&desc, nullptr, &this->_steamVRPresentBuffer);
			if (FAILED(hr)) {
				log_err("dwWidth, Height: %u, %u\n", dwWidth, dwHeight);
				log_err_desc(step, hWnd, hr, desc);
				goto out;
			}
		}

		/*
		if (g_bReshadeEnabled) {
			UINT curFlags = desc.BindFlags;
			desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
			log_err("Added D3D11_BIND_RENDER_TARGET flag\n");
			log_err("Flags: 0x%x\n", desc.BindFlags);

			step = "_reshadeOutput1";
			hr = this->_d3dDevice->CreateTexture2D(&desc, nullptr, &this->_reshadeOutput1);
			if (FAILED(hr)) {
				log_err("Failed to create _reshadeOutput1\n");
				log_err("GetDeviceRemovedReason: 0x%x\n", this->_d3dDevice->GetDeviceRemovedReason());
				log_err("dwWidth, Height: %u, %u\n", dwWidth, dwHeight);
				log_err_desc(step, hWnd, hr, desc);
				goto out;
			}
			else {
				log_debug("[DBG] _reshadeOutput1 created with combined flags");
				log_err("Successfully created _reshadeOutput1 with combined flags\n");
			}

			step = "_reshadeOutput2";
			hr = this->_d3dDevice->CreateTexture2D(&desc, nullptr, &this->_reshadeOutput2);
			if (FAILED(hr)) {
				log_err("Failed to create _reshadeOutput2\n");
				log_err("GetDeviceRemovedReason: 0x%x\n", this->_d3dDevice->GetDeviceRemovedReason());
				log_err("dwWidth, Height: %u, %u\n", dwWidth, dwHeight);
				log_err_desc(step, hWnd, hr, desc);
				goto out;
			}
			else {
				log_debug("[DBG] _reshadeOutput2 created with combined flags");
				log_err("Successfully created _reshadeOutput2 with combined flags\n");
			}
			// Restore the previous bind flags, just in case there is a dependency on these later on
			desc.BindFlags = curFlags;
		}
		*/

		/*
		UINT curFlags = desc.BindFlags;
		//desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		log_err("Added D3D11_BIND_RENDER_TARGET flag\n");
		log_err("Flags: 0x%x\n", desc.BindFlags);
		step = "_offscreenBufferBloomF";
		hr = this->_d3dDevice->CreateTexture2D(&desc, nullptr, &this->_offscreenBufferBloomF);
		if (FAILED(hr)) {
			log_err("Failed to create _offscreenBufferBloomF\n");
			log_err("GetDeviceRemovedReason: 0x%x\n", this->_d3dDevice->GetDeviceRemovedReason());
			log_err("dwWidth, Height: %u, %u\n", dwWidth, dwHeight);
			log_err_desc(step, hWnd, hr, desc);
			goto out;
		}
		else {
			log_debug("[DBG] _offscreenBufferBloomF FLOAT buffer created with combined flags");
			log_err("Successfully created _offscreenBufferBloomF with combined flags\n");
		}
		*/

		// No MSAA after this point
		// offscreenBufferAsInput must not have MSAA enabled since it will be used as input for the barrel shader.
		step = "offscreenBufferAsInput";
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		hr = this->_d3dDevice->CreateTexture2D(&desc, nullptr, &this->_offscreenBufferAsInput);
		if (FAILED(hr)) {
			log_err("dwWidth, Height: %u, %u\n", dwWidth, dwHeight);
			log_err_desc(step, hWnd, hr, desc);
			goto out;
		}

		if (g_bUseSteamVR) {
			step = "_offscreenBufferAsInputR";
			hr = this->_d3dDevice->CreateTexture2D(&desc, nullptr, &this->_offscreenBufferAsInputR);
			if (FAILED(hr)) {
				log_err("dwWidth, Height: %u, %u\n", dwWidth, dwHeight);
				log_err_desc(step, hWnd, hr, desc);
				goto out;
			}
		}

		if (g_bReshadeEnabled) {
			step = "_offscreenBufferAsInputReshade";
			hr = this->_d3dDevice->CreateTexture2D(&desc, nullptr, &this->_offscreenBufferAsInputReshade);
			if (FAILED(hr)) {
				log_err("dwWidth, Height: %u, %u\n", dwWidth, dwHeight);
				log_err_desc(step, hWnd, hr, desc);
				goto out;
			}

			// These guys should be the last to be created because they modify the BindFlags to
			// add D3D11_BIND_RENDER_TARGET
			UINT curFlags = desc.BindFlags;
			desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
			log_err("Added D3D11_BIND_RENDER_TARGET flag\n");
			log_err("Flags: 0x%x\n", desc.BindFlags);

			step = "_reshadeOutput1";
			hr = this->_d3dDevice->CreateTexture2D(&desc, nullptr, &this->_reshadeOutput1);
			if (FAILED(hr)) {
				log_err("Failed to create _reshadeOutput1\n");
				log_err("GetDeviceRemovedReason: 0x%x\n", this->_d3dDevice->GetDeviceRemovedReason());
				log_err("dwWidth, Height: %u, %u\n", dwWidth, dwHeight);
				log_err_desc(step, hWnd, hr, desc);
				goto out;
			}
			else {
				log_err("Successfully created _reshadeOutput1 with combined flags\n");
			}

			step = "_reshadeOutput2";
			hr = this->_d3dDevice->CreateTexture2D(&desc, nullptr, &this->_reshadeOutput2);
			if (FAILED(hr)) {
				log_err("Failed to create _reshadeOutput2\n");
				log_err("GetDeviceRemovedReason: 0x%x\n", this->_d3dDevice->GetDeviceRemovedReason());
				log_err("dwWidth, Height: %u, %u\n", dwWidth, dwHeight);
				log_err_desc(step, hWnd, hr, desc);
				goto out;
			} else {
				log_err("Successfully created _reshadeOutput2 with combined flags\n");
			}
			// Restore the previous bind flags, just in case there is a dependency on these later on
			desc.BindFlags = curFlags;
		}

		if (g_bDynCockpitEnabled) {
			// This guy should be the last one to be created because it modifies the BindFlags
			// _offscreenBufferAsInputDynCockpit should have the same properties as _offscreenBufferAsInput
			UINT curFlags = desc.BindFlags;
			desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
			log_err("_offscreenBufferAsInputDynCockpit: Added D3D11_BIND_RENDER_TARGET flag\n");
			log_err("Flags: 0x%x\n", desc.BindFlags);

			step = "_offscreenBufferAsInputDynCockpit";
			hr = this->_d3dDevice->CreateTexture2D(&desc, nullptr, &this->_offscreenAsInputDynCockpit);
			if (FAILED(hr)) {
				log_err("Failed to create _offscreenBufferAsInputDynCockpit, error: 0x%x\n", hr);
				log_err("GetDeviceRemovedReason: 0x%x\n", this->_d3dDevice->GetDeviceRemovedReason());
				log_err("dwWidth, Height: %u, %u\n", dwWidth, dwHeight);
				log_err_desc(step, hWnd, hr, desc);
				goto out;
			} else {
				log_err("Successfully created _offscreenBufferAsInputDynCockpit with combined flags\n");
			}

			step = "_offscreenBufferAsInputDynCockpitBG";
			hr = this->_d3dDevice->CreateTexture2D(&desc, nullptr, &this->_offscreenAsInputDynCockpitBG);
			if (FAILED(hr)) {
				log_err("Failed to create _offscreenBufferAsInputDynCockpitBG, error: 0x%x\n", hr);
				log_err("GetDeviceRemovedReason: 0x%x\n", this->_d3dDevice->GetDeviceRemovedReason());
				log_err("dwWidth, Height: %u, %u\n", dwWidth, dwHeight);
				log_err_desc(step, hWnd, hr, desc);
				goto out;
			} else {
				log_err("Successfully created _offscreenBufferAsInputDynCockpitBG with combined flags\n");
			}

			// Restore the previous bind flags, just in case there is a dependency on these later on
			desc.BindFlags = curFlags;
		}

		// Create the shader resource views for both offscreen buffers
		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc{};
		// Create a shader resource view for the _offscreenBuffer
		shaderResourceViewDesc.Format = desc.Format;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;
		D3D11_SRV_DIMENSION curDimension = shaderResourceViewDesc.ViewDimension;

		// Create the shader resource view for offscreenBufferAsInput
		step = "offscreenAsInputShaderResourceView";
		hr = this->_d3dDevice->CreateShaderResourceView(this->_offscreenBufferAsInput,
			&shaderResourceViewDesc, &this->_offscreenAsInputShaderResourceView);
		if (FAILED(hr)) {
			log_err("dwWidth, Height: %u, %u\n", dwWidth, dwHeight);
			log_shaderres_view(step, hWnd, hr, shaderResourceViewDesc);
			goto out;
		}

		/*
		step = "_reshadeBloomFSRV";
		DXGI_FORMAT format = desc.Format;
		shaderResourceViewDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		hr = this->_d3dDevice->CreateShaderResourceView(this->_offscreenBufferBloomF,
			&shaderResourceViewDesc, &this->_reshadeBloomFSRV);
		if (FAILED(hr)) {
			log_err("dwWidth, Height: %u, %u\n", dwWidth, dwHeight);
			log_shaderres_view(step, hWnd, hr, shaderResourceViewDesc);
			goto out;
		}
		desc.Format = format;
		*/

		if (g_bReshadeEnabled) {
			step = "_offscreenAsInputReshadeSRV";
			hr = this->_d3dDevice->CreateShaderResourceView(this->_offscreenBufferAsInputReshade,
				&shaderResourceViewDesc, &this->_offscreenAsInputReshadeSRV);
			if (FAILED(hr)) {
				log_err("dwWidth, Height: %u, %u\n", dwWidth, dwHeight);
				log_shaderres_view(step, hWnd, hr, shaderResourceViewDesc);
				goto out;
			}

			step = "_reshadeOutput1SRV";
			hr = this->_d3dDevice->CreateShaderResourceView(this->_reshadeOutput1,
				&shaderResourceViewDesc, &this->_reshadeOutput1SRV);
			if (FAILED(hr)) {
				log_err("dwWidth, Height: %u, %u\n", dwWidth, dwHeight);
				log_shaderres_view(step, hWnd, hr, shaderResourceViewDesc);
				goto out;
			}

			step = "_reshadeOutput2SRV";
			hr = this->_d3dDevice->CreateShaderResourceView(this->_reshadeOutput2,
				&shaderResourceViewDesc, &this->_reshadeOutput2SRV);
			if (FAILED(hr)) {
				log_err("dwWidth, Height: %u, %u\n", dwWidth, dwHeight);
				log_shaderres_view(step, hWnd, hr, shaderResourceViewDesc);
				goto out;
			}
		}

		if (g_bUseSteamVR) {
			// Create the shader resource view for offscreenBufferAsInputR
			step = "offscreenAsInputShaderResourceViewR";
			hr = this->_d3dDevice->CreateShaderResourceView(this->_offscreenBufferAsInputR,
				&shaderResourceViewDesc, &this->_offscreenAsInputShaderResourceViewR);
			if (FAILED(hr)) {
				log_err("dwWidth, Height: %u, %u\n", dwWidth, dwHeight);
				log_shaderres_view(step, hWnd, hr, shaderResourceViewDesc);
				goto out;
			}
		}

		if (g_bDynCockpitEnabled) {
			// Create the SRSV for _offscreenBufferAsInputDynCockpit
			step = "_offscreenBufferAsInputDynCockpit";
			hr = this->_d3dDevice->CreateShaderResourceView(this->_offscreenAsInputDynCockpit,
				&shaderResourceViewDesc, &this->_offscreenAsInputSRVDynCockpit);
			if (FAILED(hr)) {
				log_err("dwWidth, Height: %u, %u\n", dwWidth, dwHeight);
				log_shaderres_view(step, hWnd, hr, shaderResourceViewDesc);
				goto out;
			}
			// Create the SRV for _offscreenBufferAsInputDynCockpitBG
			step = "_offscreenBufferAsInputDynCockpitBG";
			hr = this->_d3dDevice->CreateShaderResourceView(this->_offscreenAsInputDynCockpitBG,
				&shaderResourceViewDesc, &this->_offscreenAsInputSRVDynCockpitBG);
			if (FAILED(hr)) {
				log_err("dwWidth, Height: %u, %u\n", dwWidth, dwHeight);
				log_shaderres_view(step, hWnd, hr, shaderResourceViewDesc);
				goto out;
			}
		}

		// Dynamic Cockpit: Load the new cockpit textures
		//LoadNewCockpitTextures(_d3dDevice);
		// Build the HUD vertex buffer
		BuildHUDVertexBuffer(_d3dDevice, _displayWidth, _displayHeight);
		g_fCurInGameWidth = (float)_displayWidth;
		g_fCurInGameHeight = (float)_displayHeight;
	}

	if (SUCCEEDED(hr))
	{
		CD3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc(this->_useMultisampling ? D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D);

		step = "_renderTargetView";
		hr = this->_d3dDevice->CreateRenderTargetView(this->_offscreenBuffer, &renderTargetViewDesc, &this->_renderTargetView);
		if (FAILED(hr)) goto out;

		step = "_renderTargetViewPost";
		hr = this->_d3dDevice->CreateRenderTargetView(this->_offscreenBufferPost, &renderTargetViewDesc, &this->_renderTargetViewPost);
		if (FAILED(hr)) goto out;

		if (g_bUseSteamVR) {
			step = "RenderTargetViewR";
			hr = this->_d3dDevice->CreateRenderTargetView(this->_offscreenBufferR, &renderTargetViewDesc, &this->_renderTargetViewR);
			if (FAILED(hr)) goto out;

			step = "RenderTargetViewPostR";
			hr = this->_d3dDevice->CreateRenderTargetView(this->_offscreenBufferPostR, &renderTargetViewDesc, &this->_renderTargetViewPostR);
			if (FAILED(hr)) goto out;

			step = "renderTargetViewSteamVRResize";
			hr = this->_d3dDevice->CreateRenderTargetView(this->_steamVRPresentBuffer, &renderTargetViewDesc, &this->_renderTargetViewSteamVRResize);
			if (FAILED(hr)) goto out;
		}

		if (g_bDynCockpitEnabled) {
			step = "_renderTargetViewDynCockpit";
			hr = this->_d3dDevice->CreateRenderTargetView(this->_offscreenBufferDynCockpit, &renderTargetViewDesc, &this->_renderTargetViewDynCockpit);
			if (FAILED(hr)) goto out;

			step = "_renderTargetViewDynCockpitBG";
			hr = this->_d3dDevice->CreateRenderTargetView(this->_offscreenBufferDynCockpitBG, &renderTargetViewDesc, &this->_renderTargetViewDynCockpitBG);
			if (FAILED(hr)) goto out;

			CD3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDescNoMSAA(D3D11_RTV_DIMENSION_TEXTURE2D);
			step = "_renderTargetViewDynCockpitAsInput";
			// This RTV writes to a non-MSAA texture
			hr = this->_d3dDevice->CreateRenderTargetView(this->_offscreenAsInputDynCockpit, &renderTargetViewDescNoMSAA,
				&this->_renderTargetViewDynCockpitAsInput);
			if (FAILED(hr)) goto out;

			step = "_renderTargetViewDynCockpitAsInputBG";
			// This RTV writes to a non-MSAA texture
			hr = this->_d3dDevice->CreateRenderTargetView(this->_offscreenAsInputDynCockpitBG, &renderTargetViewDescNoMSAA,
				&this->_renderTargetViewDynCockpitAsInputBG);
			if (FAILED(hr)) goto out;
		}

		/*
		CD3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDescNoMSAA(D3D11_RTV_DIMENSION_TEXTURE2D);
		step = "_renderTargetViewBloomF";
		hr = this->_d3dDevice->CreateRenderTargetView(this->_offscreenBufferBloomF, &renderTargetViewDescNoMSAA, &this->_renderTargetViewBloomF);
		if (FAILED(hr)) goto out;
		*/
		if (g_bReshadeEnabled) {
			// These RTVs render to non-MSAA buffers
			CD3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDescNoMSAA(D3D11_RTV_DIMENSION_TEXTURE2D);
			step = "_renderTargetViewReshade1";
			hr = this->_d3dDevice->CreateRenderTargetView(this->_reshadeOutput1, &renderTargetViewDescNoMSAA, &this->_renderTargetViewReshade1);
			//hr = this->_d3dDevice->CreateRenderTargetView(this->_reshadeOutput1, &renderTargetViewDesc, &this->_renderTargetViewReshade1);
			if (FAILED(hr)) goto out;

			step = "_renderTargetViewReshade2";
			hr = this->_d3dDevice->CreateRenderTargetView(this->_reshadeOutput2, &renderTargetViewDescNoMSAA, &this->_renderTargetViewReshade2);
			//hr = this->_d3dDevice->CreateRenderTargetView(this->_reshadeOutput2, &renderTargetViewDesc, &this->_renderTargetViewReshade2);
			if (FAILED(hr)) goto out;
		}
	}

	/* depth stencil */
	if (SUCCEEDED(hr))
	{
		step = "DepthStencil";
		D3D11_TEXTURE2D_DESC depthStencilDesc;
		depthStencilDesc.Width = this->_backbufferWidth;
		depthStencilDesc.Height = this->_backbufferHeight;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.SampleDesc.Count = this->_sampleDesc.Count;
		depthStencilDesc.SampleDesc.Quality = this->_sampleDesc.Quality;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0;
		depthStencilDesc.MiscFlags = 0;

		hr = this->_d3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, &this->_depthStencilL);
		if (SUCCEEDED(hr))
		{
			step = "DepthStencilViewL";
			CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(this->_useMultisampling ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D);
			hr = this->_d3dDevice->CreateDepthStencilView(this->_depthStencilL, &depthStencilViewDesc, &this->_depthStencilViewL);
			if (FAILED(hr)) goto out;
		}

		hr = this->_d3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, &this->_depthStencilR);
		if (SUCCEEDED(hr))
		{
			step = "DepthStencilViewR";
			CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(this->_useMultisampling ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D);
			hr = this->_d3dDevice->CreateDepthStencilView(this->_depthStencilR, &depthStencilViewDesc, &this->_depthStencilViewR);
			if (FAILED(hr)) goto out;
		}
	}

	/* viewport */
	if (SUCCEEDED(hr))
	{
		step = "Viewport";
		this->_d3dDeviceContext->OMSetRenderTargets(1, this->_renderTargetView.GetAddressOf(), this->_depthStencilViewL.Get());

		D3D11_VIEWPORT viewport;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = (float)this->_backbufferWidth;
		viewport.Height = (float)this->_backbufferHeight;
		viewport.MinDepth = D3D11_MIN_DEPTH;
		viewport.MaxDepth = D3D11_MAX_DEPTH;

		this->InitViewport(&viewport);
	}

	if (SUCCEEDED(hr))
	{
		step = "Texture2D displayWidth x displayHeight";

		this->_mainDisplayTextureBpp = (this->_displayBpp == 2 && this->_use16BppMainDisplayTexture) ? 2 : 4;

		D3D11_TEXTURE2D_DESC textureDesc;
		textureDesc.Width = this->_displayWidth;
		textureDesc.Height = this->_displayHeight;
		textureDesc.Format = this->_mainDisplayTextureBpp == 2 ? DXGI_FORMAT_B5G6R5_UNORM : DXGI_FORMAT_B8G8R8A8_UNORM;
		textureDesc.Usage = D3D11_USAGE_DYNAMIC;
		textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		textureDesc.MiscFlags = 0;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		hr = this->_d3dDevice->CreateTexture2D(&textureDesc, nullptr, &this->_mainDisplayTexture);

		if (SUCCEEDED(hr))
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC textureViewDesc{};
			textureViewDesc.Format = textureDesc.Format;
			textureViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			textureViewDesc.Texture2D.MipLevels = 1;
			textureViewDesc.Texture2D.MostDetailedMip = 0;

			hr = this->_d3dDevice->CreateShaderResourceView(this->_mainDisplayTexture, &textureViewDesc, &this->_mainDisplayTextureView);
		}
	}

	this->_displayTempWidth = 0;
	this->_displayTempHeight = 0;
	this->_displayTempBpp = 0;

out:
	if (FAILED(hr))
	{
		static bool messageShown = false;

		if (!messageShown)
		{
			char text[512];
			log_err("g_bWndProcReplaced: %d\n", g_bWndProcReplaced);
			close_error_file();
			strcpy_s(text, step);
			strcat_s(text, "\n");
			strcat_s(text, _com_error(hr).ErrorMessage());

			MessageBox(nullptr, text, __FUNCTION__, MB_ICONERROR);
		}

		messageShown = true;
	}

	return hr;
}

HRESULT DeviceResources::LoadMainResources()
{
	HRESULT hr = S_OK;

	if (FAILED(hr = this->_d3dDevice->CreateVertexShader(g_MainVertexShader, sizeof(g_MainVertexShader), nullptr, &_mainVertexShader)))
		return hr;

	const D3D11_INPUT_ELEMENT_DESC vertexLayoutDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	if (FAILED(hr = this->_d3dDevice->CreateInputLayout(vertexLayoutDesc, ARRAYSIZE(vertexLayoutDesc), g_MainVertexShader, sizeof(g_MainVertexShader), &_mainInputLayout)))
		return hr;

	if (FAILED(hr = this->_d3dDevice->CreatePixelShader(g_MainPixelShader, sizeof(g_MainPixelShader), nullptr, &_mainPixelShader)))
		return hr;

	if (FAILED(hr = this->_d3dDevice->CreatePixelShader(g_BarrelPixelShader, sizeof(g_BarrelPixelShader), nullptr, &_barrelPixelShader)))
		return hr;

	if (g_bBloomEnabled) {
		if (FAILED(hr = this->_d3dDevice->CreatePixelShader(g_BloomPrePassPS, sizeof(g_BloomPrePassPS), 	nullptr, &_bloomPrepassPS)))
			return hr;

		if (FAILED(hr = this->_d3dDevice->CreatePixelShader(g_BloomHGaussPS, sizeof(g_BloomHGaussPS), nullptr, &_bloomHGaussPS)))
			return hr;
		
		if (FAILED(hr = this->_d3dDevice->CreatePixelShader(g_BloomVGaussPS, sizeof(g_BloomVGaussPS), nullptr, &_bloomVGaussPS)))
			return hr;

		if (FAILED(hr = this->_d3dDevice->CreatePixelShader(g_BloomCombinePS, sizeof(g_BloomCombinePS), nullptr, &_bloomCombinePS)))
			return hr;
	}

	if (this->_d3dFeatureLevel >= D3D_FEATURE_LEVEL_10_0)
	{
		if (FAILED(hr = this->_d3dDevice->CreatePixelShader(g_MainPixelShaderBpp2ColorKey20, sizeof(g_MainPixelShaderBpp2ColorKey20), nullptr, &_mainPixelShaderBpp2ColorKey20)))
			return hr;

		if (FAILED(hr = this->_d3dDevice->CreatePixelShader(g_MainPixelShaderBpp2ColorKey00, sizeof(g_MainPixelShaderBpp2ColorKey00), nullptr, &_mainPixelShaderBpp2ColorKey00)))
			return hr;

		if (FAILED(hr = this->_d3dDevice->CreatePixelShader(g_MainPixelShaderBpp4ColorKey20, sizeof(g_MainPixelShaderBpp4ColorKey20), nullptr, &_mainPixelShaderBpp4ColorKey20)))
			return hr;
	}

	D3D11_RASTERIZER_DESC rsDesc;
	rsDesc.FillMode = D3D11_FILL_SOLID;
	rsDesc.CullMode = D3D11_CULL_NONE;
	rsDesc.FrontCounterClockwise = FALSE;
	rsDesc.DepthBias = 0;
	rsDesc.DepthBiasClamp = 0.0f;
	rsDesc.SlopeScaledDepthBias = 0.0f;
	rsDesc.DepthClipEnable = TRUE;
	rsDesc.ScissorEnable = FALSE;
	rsDesc.MultisampleEnable = TRUE;
	rsDesc.AntialiasedLineEnable = FALSE;

	if (FAILED(hr = this->_d3dDevice->CreateRasterizerState(&rsDesc, &this->_mainRasterizerState)))
		return hr;

	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = this->_useAnisotropy ? D3D11_FILTER_ANISOTROPIC : D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.MaxAnisotropy = this->_useAnisotropy ? this->GetMaxAnisotropy() : 1;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = FLT_MAX;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.BorderColor[0] = 0.0f;
	samplerDesc.BorderColor[1] = 0.0f;
	samplerDesc.BorderColor[2] = 0.0f;
	samplerDesc.BorderColor[3] = 0.0f;

	if (FAILED(hr = this->_d3dDevice->CreateSamplerState(&samplerDesc, &this->_mainSamplerState)))
		return hr;

	D3D11_BLEND_DESC blendDesc;
	blendDesc.AlphaToCoverageEnable = FALSE;
	blendDesc.IndependentBlendEnable = FALSE;
	blendDesc.RenderTarget[0].BlendEnable = FALSE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	if (FAILED(hr = this->_d3dDevice->CreateBlendState(&blendDesc, &this->_mainBlendState)))
		return hr;

	D3D11_DEPTH_STENCIL_DESC depthDesc;
	depthDesc.DepthEnable = FALSE;
	depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	depthDesc.StencilEnable = FALSE;

	if (FAILED(hr = this->_d3dDevice->CreateDepthStencilState(&depthDesc, &this->_mainDepthState)))
		return hr;

	MainVertex vertices[4] =
	{
		MainVertex(-1, -1, 0, 1),
		MainVertex( 1, -1, 1, 1),
		MainVertex( 1,  1, 1, 0),
		MainVertex(-1,  1, 0, 0),
	};

	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.ByteWidth = sizeof(MainVertex) * ARRAYSIZE(vertices);
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;
	vertexBufferData.pSysMem = vertices;
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;

	if (FAILED(hr = this->_d3dDevice->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &this->_mainVertexBuffer)))
		return hr;
	this->_steamVRPresentBuffer = NULL;

	WORD indices[6] =
	{
		0, 1, 2,
		0, 2, 3,
	};

	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.ByteWidth = sizeof(WORD) * ARRAYSIZE(indices);
	indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexBufferData;
	indexBufferData.pSysMem = indices;
	indexBufferData.SysMemPitch = 0;
	indexBufferData.SysMemSlicePitch = 0;

	if (FAILED(hr = this->_d3dDevice->CreateBuffer(&indexBufferDesc, &indexBufferData, &this->_mainIndexBuffer)))
		return hr;

	return hr;
}

HRESULT DeviceResources::LoadResources()
{
	HRESULT hr = S_OK;

	if (FAILED(hr = this->_d3dDevice->CreateVertexShader(g_VertexShader, sizeof(g_VertexShader), nullptr, &_vertexShader)))
		return hr;

	if (FAILED(hr = this->_d3dDevice->CreateVertexShader(g_SBSVertexShader, sizeof(g_SBSVertexShader), nullptr, &_sbsVertexShader)))
		return hr;

	if (FAILED(hr = this->_d3dDevice->CreateVertexShader(g_PassthroughVertexShader, sizeof(g_PassthroughVertexShader), nullptr, &_passthroughVertexShader)))
		return hr;

	const D3D11_INPUT_ELEMENT_DESC vertexLayoutDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 1, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	if (FAILED(hr = this->_d3dDevice->CreateInputLayout(vertexLayoutDesc, ARRAYSIZE(vertexLayoutDesc), g_VertexShader, sizeof(g_VertexShader), &_inputLayout)))
		return hr;

	if (FAILED(hr = this->_d3dDevice->CreateInputLayout(vertexLayoutDesc, ARRAYSIZE(vertexLayoutDesc), g_SBSVertexShader, sizeof(g_SBSVertexShader), &_inputLayout)))
		return hr;

	if (FAILED(hr = this->_d3dDevice->CreatePixelShader(g_PixelShaderTexture, sizeof(g_PixelShaderTexture), nullptr, &_pixelShaderTexture)))
		return hr;

	if (FAILED(hr = this->_d3dDevice->CreatePixelShader(g_PixelShaderSolid, sizeof(g_PixelShaderSolid), nullptr, &_pixelShaderSolid)))
		return hr;

	if (FAILED(hr = this->_d3dDevice->CreatePixelShader(g_BarrelPixelShader, sizeof(g_BarrelPixelShader), nullptr, &_barrelPixelShader)))
		return hr;

	if (FAILED(hr = this->_d3dDevice->CreatePixelShader(g_SingleBarrelPixelShader, sizeof(g_SingleBarrelPixelShader), nullptr, &_singleBarrelPixelShader)))
		return hr;

	if (g_bBloomEnabled) {
		if (FAILED(hr = this->_d3dDevice->CreatePixelShader(g_BloomPrePassPS, sizeof(g_BloomPrePassPS), nullptr, &_bloomPrepassPS)))
			return hr;

		if (FAILED(hr = this->_d3dDevice->CreatePixelShader(g_BloomHGaussPS, sizeof(g_BloomHGaussPS), nullptr, &_bloomHGaussPS)))
			return hr;

		if (FAILED(hr = this->_d3dDevice->CreatePixelShader(g_BloomVGaussPS, sizeof(g_BloomVGaussPS), nullptr, &_bloomVGaussPS)))
			return hr;

		if (FAILED(hr = this->_d3dDevice->CreatePixelShader(g_BloomCombinePS, sizeof(g_BloomCombinePS), nullptr, &_bloomCombinePS)))
			return hr;
	}

	D3D11_RASTERIZER_DESC rsDesc;
	rsDesc.FillMode = g_config.WireframeFillMode ? D3D11_FILL_WIREFRAME : D3D11_FILL_SOLID;
	rsDesc.CullMode = D3D11_CULL_NONE;
	rsDesc.FrontCounterClockwise = TRUE;
	rsDesc.DepthBias = 0;
	rsDesc.DepthBiasClamp = 0.0f;
	rsDesc.SlopeScaledDepthBias = 0.0f;
	rsDesc.DepthClipEnable = TRUE;
	rsDesc.ScissorEnable = FALSE;
	rsDesc.MultisampleEnable = TRUE;
	rsDesc.AntialiasedLineEnable = FALSE;

	if (FAILED(hr = this->_d3dDevice->CreateRasterizerState(&rsDesc, &this->_rasterizerState)))
		return hr;

	D3D11_BUFFER_DESC constantBufferDesc;
	constantBufferDesc.ByteWidth = 64;
	constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.CPUAccessFlags = 0;
	constantBufferDesc.MiscFlags = 0;
	constantBufferDesc.StructureByteStride = 0;

	// This was the original constant buffer. Now it's called _VSConstantBuffer
	if (FAILED(hr = this->_d3dDevice->CreateBuffer(&constantBufferDesc, nullptr, &this->_VSConstantBuffer)))
		return hr;

	constantBufferDesc.ByteWidth = 192; // 4x4 elems in a matrix = 16 elems. Each elem is a float, so 4 bytes * 16 = 64 bytes per matrix. This is a multiple of 16
	// 192 bytes is 3 matrices
	if (FAILED(hr = this->_d3dDevice->CreateBuffer(&constantBufferDesc, nullptr, &this->_VSMatrixBuffer)))
		return hr;

	// Create the constant buffer for the (3D) textured pixel shader
	constantBufferDesc.ByteWidth = 320;
	static_assert(sizeof(PixelShaderCBuffer) == 320, "sizeof(PixelShaderCBuffer) must be 320");
	//log_debug("[DBG] PixelShaderCBuffer size: %d", sizeof(PixelShaderCBuffer));
	if (FAILED(hr = this->_d3dDevice->CreateBuffer(&constantBufferDesc, nullptr, &this->_PSConstantBuffer)))
		return hr;

	// Create the constant buffer for the barrel pixel shader
	constantBufferDesc.ByteWidth = 16;
	if (FAILED(hr = this->_d3dDevice->CreateBuffer(&constantBufferDesc, nullptr, &this->_barrelConstantBuffer)))
		return hr;

	// Create the constant buffer for the main pixel shader
	constantBufferDesc.ByteWidth = 32;
	if (FAILED(hr = this->_d3dDevice->CreateBuffer(&constantBufferDesc, nullptr, &this->_mainShadersConstantBuffer)))
		return hr;

	return hr;
}

void DeviceResources::InitInputLayout(ID3D11InputLayout* inputLayout)
{
	static ID3D11InputLayout* currentInputLayout = nullptr;

	if (inputLayout != currentInputLayout)
	{
		currentInputLayout = inputLayout;
		this->_d3dDeviceContext->IASetInputLayout(inputLayout);
	}
}

void DeviceResources::InitVertexShader(ID3D11VertexShader* vertexShader)
{
	static ID3D11VertexShader* currentVertexShader = nullptr;

	if (vertexShader != currentVertexShader)
	{
		currentVertexShader = vertexShader;
		this->_d3dDeviceContext->VSSetShader(vertexShader, nullptr, 0);
	}
}

void DeviceResources::InitPixelShader(ID3D11PixelShader* pixelShader)
{
	static ID3D11PixelShader* currentPixelShader = nullptr;

	if (pixelShader != currentPixelShader)
	{
		currentPixelShader = pixelShader;
		this->_d3dDeviceContext->PSSetShader(pixelShader, nullptr, 0);
	}
}

void DeviceResources::InitTopology(D3D_PRIMITIVE_TOPOLOGY topology)
{
	D3D_PRIMITIVE_TOPOLOGY currentTopology = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;

	if (topology != currentTopology)
	{
		currentTopology = topology;
		this->_d3dDeviceContext->IASetPrimitiveTopology(topology);
	}
}

void DeviceResources::InitRasterizerState(ID3D11RasterizerState* state)
{
	static ID3D11RasterizerState* currentState = nullptr;

	if (state != currentState)
	{
		currentState = state;
		this->_d3dDeviceContext->RSSetState(state);
	}
}

HRESULT DeviceResources::InitSamplerState(ID3D11SamplerState** sampler, D3D11_SAMPLER_DESC* desc)
{
	static ID3D11SamplerState** currentSampler = nullptr;
	static D3D11_SAMPLER_DESC currentDesc{};

	if (sampler == nullptr)
	{
		if (memcmp(desc, &currentDesc, sizeof(D3D11_SAMPLER_DESC)) != 0)
		{
			HRESULT hr;
			ComPtr<ID3D11SamplerState> tempSampler;
			if (FAILED(hr = this->_d3dDevice->CreateSamplerState(desc, &tempSampler)))
				return hr;

			currentDesc = *desc;
			currentSampler = tempSampler.GetAddressOf();
			this->_d3dDeviceContext->PSSetSamplers(0, 1, currentSampler);
		}
	}
	else
	{
		if (sampler != currentSampler)
		{
			currentDesc = {};
			currentSampler = sampler;
			this->_d3dDeviceContext->PSSetSamplers(0, 1, currentSampler);
		}
	}

	return S_OK;
}

HRESULT DeviceResources::InitBlendState(ID3D11BlendState* blend, D3D11_BLEND_DESC* desc)
{
	static ID3D11BlendState* currentBlend = nullptr;
	static D3D11_BLEND_DESC currentDesc{};

	if (blend == nullptr)
	{
		if (memcmp(desc, &currentDesc, sizeof(D3D11_BLEND_DESC)) != 0)
		{
			HRESULT hr;
			ComPtr<ID3D11BlendState> tempBlend;
			if (FAILED(hr = this->_d3dDevice->CreateBlendState(desc, &tempBlend)))
				return hr;

			currentDesc = *desc;
			currentBlend = tempBlend;

			static const FLOAT factors[] = { 0.0f, 0.0f, 0.0f, 0.0f };
			UINT mask = 0xffffffff;
			this->_d3dDeviceContext->OMSetBlendState(currentBlend, factors, mask);
		}
	}
	else
	{
		if (blend != currentBlend)
		{
			currentDesc = {};
			currentBlend = blend;

			static const FLOAT factors[] = { 0.0f, 0.0f, 0.0f, 0.0f };
			UINT mask = 0xffffffff;
			this->_d3dDeviceContext->OMSetBlendState(currentBlend, factors, mask);
		}
	}

	return S_OK;
}

HRESULT DeviceResources::InitDepthStencilState(ID3D11DepthStencilState* depthState, D3D11_DEPTH_STENCIL_DESC* desc)
{
	static ID3D11DepthStencilState* currentDepthState = nullptr;
	static D3D11_DEPTH_STENCIL_DESC currentDesc{};

	if (depthState == nullptr)
	{
		if (memcmp(desc, &currentDesc, sizeof(D3D11_DEPTH_STENCIL_DESC)) != 0)
		{
			HRESULT hr;
			ComPtr<ID3D11DepthStencilState> tempDepthState;
			if (FAILED(hr = this->_d3dDevice->CreateDepthStencilState(desc, &tempDepthState)))
				return hr;

			currentDesc = *desc;
			currentDepthState = tempDepthState;
			this->_d3dDeviceContext->OMSetDepthStencilState(currentDepthState, 0);
		}
	}
	else
	{
		if (depthState != currentDepthState)
		{
			currentDesc = {};
			currentDepthState = depthState;
			this->_d3dDeviceContext->OMSetDepthStencilState(currentDepthState, 0);
		}
	}

	return S_OK;
}

void DeviceResources::InitVertexBuffer(ID3D11Buffer** buffer, UINT* stride, UINT* offset)
{
	static ID3D11Buffer** currentBuffer = nullptr;

	if (buffer != currentBuffer)
	{
		currentBuffer = buffer;
		this->_d3dDeviceContext->IASetVertexBuffers(0, 1, buffer, stride, offset);
	}
}

void DeviceResources::InitIndexBuffer(ID3D11Buffer* buffer)
{
	static ID3D11Buffer* currentBuffer = nullptr;

	if (buffer != currentBuffer)
	{
		currentBuffer = buffer;
		this->_d3dDeviceContext->IASetIndexBuffer(buffer, DXGI_FORMAT_R16_UINT, 0);
	}
}

void DeviceResources::InitViewport(D3D11_VIEWPORT* viewport)
{
	static D3D11_VIEWPORT currentViewport{};

	if (memcmp(viewport, &currentViewport, sizeof(D3D11_VIEWPORT)) != 0)
	{
		currentViewport = *viewport;
		this->_d3dDeviceContext->RSSetViewports(1, viewport);
	}
}

void DeviceResources::InitVSConstantBuffer3D(ID3D11Buffer** buffer, const VertexShaderCBuffer* vsCBuffer)
{
	static ID3D11Buffer** currentBuffer = nullptr;
	static VertexShaderCBuffer currentVSConstants{};
	static int sizeof_constants = sizeof(VertexShaderCBuffer);

	if (g_LastVSConstantBufferSet == VS_CONSTANT_BUFFER_NONE ||
		g_LastVSConstantBufferSet != VS_CONSTANT_BUFFER_3D ||
		memcmp(vsCBuffer, &currentVSConstants, sizeof_constants) != 0)
	{
		memcpy(&currentVSConstants, vsCBuffer, sizeof_constants);
		this->_d3dDeviceContext->UpdateSubresource(buffer[0], 0, nullptr, vsCBuffer, 0, 0);
	}

	if (g_LastVSConstantBufferSet == VS_CONSTANT_BUFFER_NONE ||
		g_LastVSConstantBufferSet != VS_CONSTANT_BUFFER_3D ||
		buffer != currentBuffer)
	{
		currentBuffer = buffer;
		this->_d3dDeviceContext->VSSetConstantBuffers(0, 1, buffer);
	}
	g_LastVSConstantBufferSet = VS_CONSTANT_BUFFER_3D;
}

void DeviceResources::InitVSConstantBufferMatrix(ID3D11Buffer** buffer, const VertexShaderMatrixCB* vsCBuffer)
{
	//static ID3D11Buffer** currentBuffer = nullptr;
	//static VertexShaderCBuffer currentVSConstants{};
	//static int sizeof_constants = sizeof(VertexShaderCBuffer);

	/*
	if (g_LastVSConstantBufferSet == VS_CONSTANT_BUFFER_NONE ||
		g_LastVSConstantBufferSet != VS_CONSTANT_BUFFER_3D ||
		memcmp(vsCBuffer, &currentVSConstants, sizeof_constants) != 0)
	{ 
		memcpy(&currentVSConstants, vsCBuffer, sizeof_constants);
		this->_d3dDeviceContext->UpdateSubresource(buffer[0], 0, nullptr, vsCBuffer, 0, 0);
	}	

	if (g_LastVSConstantBufferSet == VS_CONSTANT_BUFFER_NONE ||
		g_LastVSConstantBufferSet != VS_CONSTANT_BUFFER_3D ||
		buffer != currentBuffer)
	{
		currentBuffer = buffer;
		this->_d3dDeviceContext->VSSetConstantBuffers(0, 1, buffer);
	}
	g_LastVSConstantBufferSet = VS_CONSTANT_BUFFER_3D;
	*/
	this->_d3dDeviceContext->UpdateSubresource(buffer[0], 0, nullptr, vsCBuffer, 0, 0);
	this->_d3dDeviceContext->VSSetConstantBuffers(1, 1, buffer);
}

void DeviceResources::InitVSConstantBuffer2D(ID3D11Buffer** buffer, const float parallax,
	const float aspectRatio, const float scale, const float brightness, const float use_3D)
{
	static ID3D11Buffer** currentBuffer = nullptr;
	if (g_LastVSConstantBufferSet == VS_CONSTANT_BUFFER_NONE ||
		g_LastVSConstantBufferSet != VS_CONSTANT_BUFFER_2D ||
		g_MSCBuffer.parallax != parallax ||
		g_MSCBuffer.aspectRatio != aspectRatio ||
		g_MSCBuffer.scale != scale ||
		g_MSCBuffer.brightness != brightness ||
		g_MSCBuffer.use_3D != use_3D)
	{
		g_MSCBuffer.parallax = parallax;
		g_MSCBuffer.aspectRatio = aspectRatio;
		g_MSCBuffer.scale = scale;
		g_MSCBuffer.brightness = brightness;
		g_MSCBuffer.use_3D = use_3D;
		this->_d3dDeviceContext->UpdateSubresource(buffer[0], 0, nullptr, &g_MSCBuffer, 0, 0);
	}

	if (g_LastVSConstantBufferSet == VS_CONSTANT_BUFFER_NONE ||
		g_LastVSConstantBufferSet != VS_CONSTANT_BUFFER_2D ||
		buffer != currentBuffer)
	{
		currentBuffer = buffer;
		this->_d3dDeviceContext->VSSetConstantBuffers(0, 1, buffer);
	}
	g_LastVSConstantBufferSet = VS_CONSTANT_BUFFER_2D;
}

void DeviceResources::InitPSConstantBuffer2D(ID3D11Buffer** buffer, const float parallax,
	const float aspectRatio, const float scale, const float brightness)
{
	static ID3D11Buffer** currentBuffer = nullptr;
	if (g_LastPSConstantBufferSet == PS_CONSTANT_BUFFER_NONE ||
		g_LastPSConstantBufferSet != PS_CONSTANT_BUFFER_2D ||
		g_MSCBuffer.parallax != parallax ||
		g_MSCBuffer.aspectRatio != aspectRatio ||
		g_MSCBuffer.scale != scale ||
		g_MSCBuffer.brightness != brightness)
	{
		g_MSCBuffer.parallax = parallax;
		g_MSCBuffer.aspectRatio = aspectRatio;
		g_MSCBuffer.scale = scale;
		g_MSCBuffer.brightness = brightness;
		this->_d3dDeviceContext->UpdateSubresource(buffer[0], 0, nullptr, &g_MSCBuffer, 0, 0);
	}

	if (g_LastPSConstantBufferSet == PS_CONSTANT_BUFFER_NONE ||
		g_LastPSConstantBufferSet != PS_CONSTANT_BUFFER_2D ||
		buffer != currentBuffer)
	{
		currentBuffer = buffer;
		this->_d3dDeviceContext->PSSetConstantBuffers(0, 1, buffer);
	}
	g_LastPSConstantBufferSet = PS_CONSTANT_BUFFER_2D;
}

void DeviceResources::InitPSConstantBufferBarrel(ID3D11Buffer** buffer, const float k1, const float k2, const float k3)
{
	static ID3D11Buffer** currentBuffer = nullptr;
	if (g_LastPSConstantBufferSet == PS_CONSTANT_BUFFER_NONE ||
		g_LastPSConstantBufferSet != PS_CONSTANT_BUFFER_BARREL ||
		g_BPSCBuffer.k1 != k1 ||
		g_BPSCBuffer.k2 != k2 ||
		g_BPSCBuffer.k3 != k3)
	{
		g_BPSCBuffer.k1 = k1;
		g_BPSCBuffer.k2 = k2;
		g_BPSCBuffer.k3 = k3;
		this->_d3dDeviceContext->UpdateSubresource(buffer[0], 0, nullptr, &g_BPSCBuffer, 0, 0);
	}

	if (g_LastPSConstantBufferSet == PS_CONSTANT_BUFFER_NONE ||
		g_LastPSConstantBufferSet != PS_CONSTANT_BUFFER_BARREL ||
		buffer != currentBuffer)
	{
		currentBuffer = buffer;
		this->_d3dDeviceContext->PSSetConstantBuffers(0, 1, buffer);
	}
	g_LastPSConstantBufferSet = PS_CONSTANT_BUFFER_BARREL;
}

void DeviceResources::InitPSConstantBuffer3D(ID3D11Buffer** buffer, const PixelShaderCBuffer* psConstants)
{
	static ID3D11Buffer** currentBuffer = nullptr;
	static PixelShaderCBuffer currentPSConstants = {0};
	static int sizeof_constants = sizeof(PixelShaderCBuffer);

	if (g_LastPSConstantBufferSet == PS_CONSTANT_BUFFER_NONE ||
		g_LastPSConstantBufferSet != PS_CONSTANT_BUFFER_3D ||
		memcmp(psConstants, &currentPSConstants, sizeof_constants) != 0)
	{
		memcpy(&currentPSConstants, psConstants, sizeof_constants);
		this->_d3dDeviceContext->UpdateSubresource(buffer[0], 0, nullptr, psConstants, 0, 0);
	}

	if (g_LastPSConstantBufferSet == PS_CONSTANT_BUFFER_NONE ||
		g_LastPSConstantBufferSet != PS_CONSTANT_BUFFER_3D ||
		buffer != currentBuffer)
	{
		currentBuffer = buffer;
		this->_d3dDeviceContext->PSSetConstantBuffers(0, 1, buffer);
	}
	g_LastPSConstantBufferSet = PS_CONSTANT_BUFFER_3D;
}

HRESULT DeviceResources::RenderMain(char* src, DWORD width, DWORD height, DWORD bpp, RenderMainColorKeyType useColorKey)
{
	HRESULT hr = S_OK;
	char* step = "";

	D3D11_MAPPED_SUBRESOURCE displayMap;
	DWORD pitchDelta;

	ID3D11Texture2D* tex = nullptr;
	ID3D11ShaderResourceView** texView = nullptr;

	/*
	if (g_bUseSteamVR) {
		// Process SteamVR events
		vr::VREvent_t event;
		while (g_pHMD->PollNextEvent(&event, sizeof(event)))
		{
			ProcessVREvent(event);
		}
	}
	*/

	if (SUCCEEDED(hr))
	{
		if ((width == this->_displayWidth) && (height == this->_displayHeight) && (bpp == this->_mainDisplayTextureBpp))
		{
			step = "DisplayTexture displayWidth x displayHeight";
			hr = this->_d3dDeviceContext->Map(this->_mainDisplayTexture, 0, D3D11_MAP_WRITE_DISCARD, 0, &displayMap);

			if (SUCCEEDED(hr))
			{
				pitchDelta = displayMap.RowPitch - width * bpp;
				tex = this->_mainDisplayTexture;
				texView = this->_mainDisplayTextureView.GetAddressOf();
			}
		}
		else
		{
			step = "DisplayTexture temp";

			if (width != this->_displayTempWidth || height != this->_displayTempHeight || bpp != this->_displayTempBpp)
			{
				D3D11_TEXTURE2D_DESC textureDesc;
				textureDesc.Width = width;
				textureDesc.Height = height;
				textureDesc.Format = (bpp == 2 && this->_use16BppMainDisplayTexture) ? DXGI_FORMAT_B5G6R5_UNORM : DXGI_FORMAT_B8G8R8A8_UNORM;
				textureDesc.Usage = D3D11_USAGE_DYNAMIC;
				textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
				textureDesc.MiscFlags = 0;
				textureDesc.MipLevels = 1;
				textureDesc.ArraySize = 1;
				textureDesc.SampleDesc.Count = 1;
				textureDesc.SampleDesc.Quality = 0;
				textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

				hr = this->_d3dDevice->CreateTexture2D(&textureDesc, nullptr, &this->_mainDisplayTextureTemp);

				if (SUCCEEDED(hr))
				{
					D3D11_SHADER_RESOURCE_VIEW_DESC textureViewDesc{};
					textureViewDesc.Format = textureDesc.Format;
					textureViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
					textureViewDesc.Texture2D.MipLevels = 1;
					textureViewDesc.Texture2D.MostDetailedMip = 0;

					hr = this->_d3dDevice->CreateShaderResourceView(this->_mainDisplayTextureTemp, &textureViewDesc, &this->_mainDisplayTextureViewTemp);
				}

				if (SUCCEEDED(hr))
				{
					this->_displayTempWidth = width;
					this->_displayTempHeight = height;
					this->_displayTempBpp = bpp;
				}
			}

			if (SUCCEEDED(hr))
			{
				hr = this->_d3dDeviceContext->Map(this->_mainDisplayTextureTemp, 0, D3D11_MAP_WRITE_DISCARD, 0, &displayMap);

				if (SUCCEEDED(hr))
				{
					pitchDelta = displayMap.RowPitch - width * ((bpp == 2 && this->_use16BppMainDisplayTexture) ? 2 : 4);
					tex = this->_mainDisplayTextureTemp;
					texView = this->_mainDisplayTextureViewTemp.GetAddressOf();
				}
			}
		}
	}

	if (SUCCEEDED(hr))
	{
		if (bpp == 2)
		{
			if (this->_use16BppMainDisplayTexture)
			{
				if (pitchDelta == 0)
				{
					memcpy(displayMap.pData, src, width * height * 2);
				}
				else
				{
					unsigned short* srcColors = (unsigned short*)src;
					unsigned short* colors = (unsigned short*)displayMap.pData;

					for (DWORD y = 0; y < height; y++)
					{
						memcpy(colors, srcColors, width * 2);
						srcColors += width;
						colors = (unsigned short*)((char*)(colors + width) + pitchDelta);
					}
				}
			}
			else
			{
				if (useColorKey == RENDERMAIN_COLORKEY_20)
				{
					unsigned short* srcColors = (unsigned short*)src;
					unsigned int* colors = (unsigned int*)displayMap.pData;

					for (DWORD y = 0; y < height; y++)
					{
						for (DWORD x = 0; x < width; x++)
						{
							unsigned short color16 = srcColors[x];

							if (color16 == 0x2000)
							{
								colors[x] = 0xff000000;
							}
							else
							{
								colors[x] = convertColorB5G6R5toB8G8R8X8(color16);
							}
						}

						srcColors += width;
						colors = (unsigned int*)((char*)(colors + width) + pitchDelta);
					}
				}
				else if (useColorKey == RENDERMAIN_COLORKEY_00)
				{
					unsigned short* srcColors = (unsigned short*)src;
					unsigned int* colors = (unsigned int*)displayMap.pData;

					for (DWORD y = 0; y < height; y++)
					{
						for (DWORD x = 0; x < width; x++)
						{
							unsigned short color16 = srcColors[x];

							if (color16 == 0)
							{
								colors[x] = 0xff000000;
							}
							else
							{
								colors[x] = convertColorB5G6R5toB8G8R8X8(color16);
							}
						}

						srcColors += width;
						colors = (unsigned int*)((char*)(colors + width) + pitchDelta);
					}
				}
				else
				{
					unsigned short* srcColors = (unsigned short*)src;
					unsigned int* colors = (unsigned int*)displayMap.pData;

					for (DWORD y = 0; y < height; y++)
					{
						for (DWORD x = 0; x < width; x++)
						{
							unsigned short color16 = srcColors[x];

							colors[x] = convertColorB5G6R5toB8G8R8X8(color16);
						}

						srcColors += width;
						colors = (unsigned int*)((char*)(colors + width) + pitchDelta);
					}
				}
			}
		}
		else
		{
			if (useColorKey && (this->_d3dFeatureLevel < D3D_FEATURE_LEVEL_10_0))
			{
				unsigned int* srcColors = (unsigned int*)src;
				unsigned int* colors = (unsigned int*)displayMap.pData;

				__m128i key = _mm_set1_epi32(0x200000);
				__m128i colorMask = _mm_set1_epi32(0xffffff);

				for (DWORD y = 0; y < height; y++)
				{
					DWORD x = 0;
					for (; x < (width & ~3); x += 4)
					{
						__m128i color = _mm_load_si128((const __m128i*)(srcColors + x));
						__m128i transparent = _mm_cmpeq_epi32(color, key);

						color = _mm_andnot_si128(transparent, color);
						color = _mm_and_si128(color, colorMask);

						transparent = _mm_slli_epi32(transparent, 24);

						color = _mm_or_si128(color, transparent);
						_mm_store_si128((__m128i*)(colors + x), color);
					}

					for (; x < width; x++)
					{
						unsigned int color32 = srcColors[x];

						if (color32 == 0x200000)
						{
							colors[x] = 0xff000000;
						}
						else
						{
							colors[x] = color32 & 0xffffff;
						}
					}

					srcColors += width;
					colors = (unsigned int*)((char*)(colors + width) + pitchDelta);
				}
			}
			else
			{
				if (pitchDelta == 0)
				{
					memcpy(displayMap.pData, src, width * height * 4);
				}
				else
				{
					unsigned int* srcColors = (unsigned int*)src;
					unsigned int* colors = (unsigned int*)displayMap.pData;

					for (DWORD y = 0; y < height; y++)
					{
						memcpy(colors, srcColors, width * 4);
						srcColors += width;
						colors = (unsigned int*)((char*)(colors + width) + pitchDelta);
					}
				}
			}
		}
	}

	if (SUCCEEDED(hr))
	{
		this->_d3dDeviceContext->Unmap(tex, 0);
	}

	this->InitInputLayout(this->_mainInputLayout);
	this->InitTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	this->InitVertexShader(this->_mainVertexShader);

	if (bpp == 2)
	{
		if (useColorKey && this->_use16BppMainDisplayTexture)
		{
			switch (useColorKey)
			{
			case RENDERMAIN_COLORKEY_20:
				this->InitPixelShader(this->_mainPixelShaderBpp2ColorKey20);
				break;

			case RENDERMAIN_COLORKEY_00:
				this->InitPixelShader(this->_mainPixelShaderBpp2ColorKey00);
				break;

			default:
				this->InitPixelShader(this->_mainPixelShader);
				break;
			}
		}
		else
		{
			this->InitPixelShader(this->_mainPixelShader);
		}
	}
	else
	{
		if (useColorKey && this->_d3dFeatureLevel >= D3D_FEATURE_LEVEL_10_0)
		{
			this->InitPixelShader(this->_mainPixelShaderBpp4ColorKey20);
		}
		else
		{
			this->InitPixelShader(this->_mainPixelShader);
		}
	}

	UINT w;
	UINT h;

	if (g_config.AspectRatioPreserved)
	{
		if (this->_backbufferHeight * width <= this->_backbufferWidth * height)
		{
			w = this->_backbufferHeight * width / height;
			h = this->_backbufferHeight;
		}
		else
		{
			w = this->_backbufferWidth;
			h = this->_backbufferWidth * height / width;
		}
		
		if (!g_bOverrideAspectRatio) { // Only compute the aspect ratio if we didn't read it off of the config file
			g_fConcourseAspectRatio = 2.0f * w / this->_backbufferWidth;
			if (g_fConcourseAspectRatio < 2.0f)
				g_fConcourseAspectRatio = 2.0f;
		}
	}
	else
	{
		w = this->_backbufferWidth;
		h = this->_backbufferHeight;
		
		if (!g_bOverrideAspectRatio) {
			float original_aspect = (float)this->_displayWidth / (float)this->_displayHeight;
			float actual_aspect = (float)this->_backbufferWidth / (float)this->_backbufferHeight;
			g_fConcourseAspectRatio = 2.0f; // * (1.0f + (actual_aspect - original_aspect));
			//g_fConcourseAspectRatio = 2.0f * actual_aspect / original_aspect;
		}
	}
	//if (g_bUseSteamVR) g_fConcourseAspectRatio = 1.0f;

	UINT left = (this->_backbufferWidth - w) / 2;
	UINT top = (this->_backbufferHeight - h) / 2;

	if (g_bEnableVR) { // SteamVR and DirectSBS modes
		InitVSConstantBuffer2D(this->_mainShadersConstantBuffer.GetAddressOf(), 0.0f, g_fConcourseAspectRatio, g_fConcourseScale, g_fBrightness, 1.0f); // Use 3D projection matrices
		InitPSConstantBuffer2D(this->_mainShadersConstantBuffer.GetAddressOf(), 0.0f, g_fConcourseAspectRatio, g_fConcourseScale, g_fBrightness);
	} 
	else {
		InitVSConstantBuffer2D(this->_mainShadersConstantBuffer.GetAddressOf(), 0, 1, 1, g_fBrightness, 0.0f); // Don't 3D projection matrices when VR is disabled
		InitPSConstantBuffer2D(this->_mainShadersConstantBuffer.GetAddressOf(), 0, 1, 1, g_fBrightness);
	}

	D3D11_VIEWPORT viewport;
	viewport.TopLeftX = (float)left;
	viewport.TopLeftY = (float)top;
	viewport.Width = (float)w;
	viewport.Height = (float)h;
	viewport.MinDepth = D3D11_MIN_DEPTH;
	viewport.MaxDepth = D3D11_MAX_DEPTH;
	this->InitViewport(&viewport);

	if (SUCCEEDED(hr))
	{
		step = "States";
		this->InitRasterizerState(this->_mainRasterizerState);
		this->InitSamplerState(this->_mainSamplerState.GetAddressOf(), nullptr);
		this->InitBlendState(this->_mainBlendState, nullptr);
		this->InitDepthStencilState(this->_mainDepthState, nullptr);
	}

	if (SUCCEEDED(hr))
	{
		step = "Texture2D ShaderResourceView";

		this->_d3dDeviceContext->PSSetShaderResources(0, 1, texView);
	}

	if (SUCCEEDED(hr))
	{
		step = "Draw";
		D3D11_VIEWPORT viewport = { 0 };
		UINT stride = sizeof(MainVertex);
		UINT offset = 0;

		this->InitVertexBuffer(this->_mainVertexBuffer.GetAddressOf(), &stride, &offset);
		this->InitIndexBuffer(this->_mainIndexBuffer);

		float screen_res_x = (float)this->_backbufferWidth;
		float screen_res_y = (float)this->_backbufferHeight;

		if (!g_bEnableVR) {
			// The Concourse and 2D menu are drawn here... maybe the default starfield too?
			this->_d3dDeviceContext->DrawIndexed(6, 0, 0);
			goto out;
		}

		// Let's do SBS rendering here. That'll make it compatible with the Tech Library where
		// both 2D and 3D are mixed.
		// Left viewport
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		if (g_bUseSteamVR)
			viewport.Width = screen_res_x;
		else
			viewport.Width = screen_res_x / 2.0f;
		viewport.Height = screen_res_y;
		viewport.MaxDepth = D3D11_MAX_DEPTH;
		viewport.MinDepth = D3D11_MIN_DEPTH;
		this->InitViewport(&viewport);
		this->InitVSConstantBuffer2D(_mainShadersConstantBuffer.GetAddressOf(),
			g_fTechLibraryParallax * g_iDraw2DCounter, g_fConcourseAspectRatio, g_fConcourseScale, g_fBrightness, 
			1.0f); // Use 3D projection matrices

		// The Concourse and 2D menu are drawn here... maybe the default starfield too?
		// When SteamVR is not used, the RenderTargets are set in the OnSizeChanged() event above
		g_VSMatrixCB.projEye = g_fullMatrixLeft;
		InitVSConstantBufferMatrix(_VSMatrixBuffer.GetAddressOf(), &g_VSMatrixCB);
		if (g_bUseSteamVR)
			_d3dDeviceContext->OMSetRenderTargets(1, _renderTargetView.GetAddressOf(), _depthStencilViewL.Get());
		else
			_d3dDeviceContext->OMSetRenderTargets(1, _renderTargetView.GetAddressOf(), _depthStencilViewL.Get());
		this->_d3dDeviceContext->DrawIndexed(6, 0, 0);

		// Right viewport
		if (g_bUseSteamVR) {
			viewport.TopLeftX = 0;
			viewport.Width = screen_res_x;
		} else {
			viewport.TopLeftX = screen_res_x / 2.0f;
			viewport.Width = screen_res_x / 2.0f;
		}
		viewport.TopLeftY = 0;
		viewport.Height = screen_res_y;
		viewport.MaxDepth = D3D11_MAX_DEPTH;
		viewport.MinDepth = D3D11_MIN_DEPTH;
		this->InitViewport(&viewport);
		this->InitVSConstantBuffer2D(this->_mainShadersConstantBuffer.GetAddressOf(),
			g_fTechLibraryParallax * g_iDraw2DCounter, g_fConcourseAspectRatio, g_fConcourseScale, g_fBrightness,
			1.0f); // Use 3D projection matrices
		// The Concourse and 2D menu are drawn here... maybe the default starfield too?
		g_VSMatrixCB.projEye = g_fullMatrixRight;
		InitVSConstantBufferMatrix(_VSMatrixBuffer.GetAddressOf(), &g_VSMatrixCB);
		if (g_bUseSteamVR)
			_d3dDeviceContext->OMSetRenderTargets(1, _renderTargetViewR.GetAddressOf(), _depthStencilViewR.Get());
		else
			_d3dDeviceContext->OMSetRenderTargets(1, _renderTargetView.GetAddressOf(), _depthStencilViewL.Get());
		this->_d3dDeviceContext->DrawIndexed(6, 0, 0);
	out:
		// Increase the 2D DrawCounter -- this is used in the Tech Library to increase the parallax when the second 2D
		// layer is rendered on top of the 3D floating object.
		g_iDraw2DCounter++;
	}

	if (FAILED(hr))
	{
		static bool messageShown = false;

		if (!messageShown)
		{
			char text[512];
			strcpy_s(text, step);
			strcat_s(text, "\n");
			strcat_s(text, _com_error(hr).ErrorMessage());

			MessageBox(nullptr, text, __FUNCTION__, MB_ICONERROR);
		}

		messageShown = true;
	}

	return hr;
}

HRESULT DeviceResources::RetrieveBackBuffer(char* buffer, DWORD width, DWORD height, DWORD bpp)
{
	HRESULT hr = S_OK;
	char* step = "";

	memset(buffer, 0, width * height * bpp);

	D3D11_TEXTURE2D_DESC textureDescription;
	this->_backBuffer->GetDesc(&textureDescription);

	textureDescription.BindFlags = 0;
	textureDescription.SampleDesc.Count = 1;
	textureDescription.SampleDesc.Quality = 0;

	ComPtr<ID3D11Texture2D> backBuffer;
	textureDescription.Usage = D3D11_USAGE_DEFAULT;
	textureDescription.CPUAccessFlags = 0;

	step = "Resolve BackBuffer";

	if (SUCCEEDED(hr = this->_d3dDevice->CreateTexture2D(&textureDescription, nullptr, &backBuffer)))
	{
		this->_d3dDeviceContext->ResolveSubresource(backBuffer, D3D11CalcSubresource(0, 0, 1), this->_backBuffer, D3D11CalcSubresource(0, 0, 1), textureDescription.Format);

		step = "Staging Texture2D";

		ComPtr<ID3D11Texture2D> texture;
		textureDescription.Usage = D3D11_USAGE_STAGING;
		textureDescription.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;

		if (SUCCEEDED(hr = this->_d3dDevice->CreateTexture2D(&textureDescription, nullptr, &texture)))
		{
			this->_d3dDeviceContext->CopyResource(texture, backBuffer);

			step = "Map";

			D3D11_MAPPED_SUBRESOURCE map;
			if (SUCCEEDED(hr = this->_d3dDeviceContext->Map(texture, 0, D3D11_MAP_READ, 0, &map)))
			{
				step = "copy";

				if (bpp == 4 && width == this->_backbufferWidth && height == this->_backbufferHeight && this->_backbufferWidth * 4 == map.RowPitch)
				{
					memcpy(buffer, map.pData, width * height * 4);
				}
				else
				{
					if (this->_backbufferWidth * 4 == map.RowPitch)
					{
						scaleSurface(buffer, width, height, bpp, (char*)map.pData, this->_backbufferWidth, this->_backbufferHeight, 4);
					}
					else
					{
						char* buffer2 = new char[this->_backbufferWidth * this->_backbufferHeight * 4];

						unsigned int* srcColors = (unsigned int*)map.pData;
						unsigned int* colors = (unsigned int*)buffer2;

						for (DWORD y = 0; y < this->_backbufferHeight; y++)
						{
							memcpy(colors, srcColors, this->_backbufferWidth * 4);

							srcColors = (unsigned int*)((char*)srcColors + map.RowPitch);
							colors += this->_backbufferWidth;
						}

						scaleSurface(buffer, width, height, bpp, buffer2, this->_backbufferWidth, this->_backbufferHeight, 4);

						delete[] buffer2;
					}
				}

				this->_d3dDeviceContext->Unmap(texture, 0);
			}
		}
	}

	if (FAILED(hr))
	{
		static bool messageShown = false;

		if (!messageShown)
		{
			char text[512];
			strcpy_s(text, step);
			strcat_s(text, "\n");
			strcat_s(text, _com_error(hr).ErrorMessage());

			MessageBox(nullptr, text, __FUNCTION__, MB_ICONERROR);
		}

		messageShown = true;
	}

	return hr;
}

UINT DeviceResources::GetMaxAnisotropy()
{
	return this->_d3dFeatureLevel >= D3D_FEATURE_LEVEL_9_2 ? D3D11_MAX_MAXANISOTROPY : D3D_FL9_1_DEFAULT_MAX_ANISOTROPY;
}

void DeviceResources::CheckMultisamplingSupport()
{
	this->_sampleDesc.Count = 1;
	this->_sampleDesc.Quality = 0;

	if (!this->_useMultisampling)
	{
		return;
	}

	UINT formatSupport;

	if (FAILED(this->_d3dDevice->CheckFormatSupport(DXGI_FORMAT_B8G8R8A8_UNORM, &formatSupport)))
	{
		return;
	}

	bool supported = (formatSupport & D3D11_FORMAT_SUPPORT_MULTISAMPLE_RESOLVE) && (formatSupport & D3D11_FORMAT_SUPPORT_MULTISAMPLE_RENDERTARGET);

	if (supported)
	{
		for (UINT i = 2; i <= D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT; i *= 2)
		{
			UINT numQualityLevels = 0;

			HRESULT hr = this->_d3dDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_B8G8R8A8_UNORM, i, &numQualityLevels);

			if (SUCCEEDED(hr) && (numQualityLevels > 0))
			{
				this->_sampleDesc.Count = i;
				this->_sampleDesc.Quality = numQualityLevels - 1;
			}
		}
	}

	if (this->_sampleDesc.Count <= 1)
	{
		this->_useMultisampling = FALSE;
	}
}

bool DeviceResources::IsTextureFormatSupported(DXGI_FORMAT format)
{
	UINT formatSupport;

	if (FAILED(this->_d3dDevice->CheckFormatSupport(format, &formatSupport)))
	{
		return false;
	}

	const UINT expected = D3D11_FORMAT_SUPPORT_TEXTURE2D | D3D11_FORMAT_SUPPORT_MIP | D3D11_FORMAT_SUPPORT_SHADER_LOAD | D3D11_FORMAT_SUPPORT_CPU_LOCKABLE;

	return (formatSupport & expected) == expected;
}
