// Copyright (c) 2014 J�r�my Ansel
// Licensed under the MIT license. See LICENSE.txt
// Extended for VR by Leo Reyes (c) 2019

#include <ScreenGrab.h>
#include <wincodec.h>

#include "common.h"
#include "DeviceResources.h"
#include "PrimarySurface.h"
#include "BackbufferSurface.h"
#include "FrontbufferSurface.h"

#define DBG_MAX_PRESENT_LOGS 0

struct MainVertex
{
	float pos[2];
	float tex[2];
	MainVertex() {}
	MainVertex(float x, float y, float tx, float ty) {
		pos[0] = x; pos[1] = y;
		tex[0] = tx; tex[1] = ty;
	}
};

BarrelPixelShaderCBuffer g_BPSCBuffer;
extern float g_fLensK1, g_fLensK2, g_fLensK3;

// Main Pixel Shader constant buffer
MainShadersCBuffer g_MSCBuffer;
extern float g_fConcourseScale, g_fConcourseAspectRatio;
extern int g_iDraw2DCounter;
extern bool g_bStartedGUI;
extern bool g_bEnableVR, g_bDisableBarrelEffect;
extern bool g_bDumpGUI;
extern int g_iDrawCounter, g_iExecBufCounter, g_iPresentCounter, g_iNonZBufferCounter;
extern bool g_bTargetCompDrawn;
extern unsigned int g_iFloatingGUIDrawnCounter;

bool g_bRendering3D = false; // Set to true when the system is about to render in 3D

// SteamVR
extern DWORD g_FullScreenWidth, g_FullScreenHeight;

void animTickX();
void animTickY();
void animTickZ();

MainVertex g_BarrelEffectVertices[6] = {
	MainVertex(-1, -1, 0, 1),
	MainVertex(1, -1, 1, 1),
	MainVertex(1,  1, 1, 0),

	MainVertex(1,  1, 1, 0),
	MainVertex(-1,  1, 0, 0),
	MainVertex(-1, -1, 0, 1),
};
ID3D11Buffer* g_BarrelEffectVertBuffer = NULL;

#ifdef DBR_VR
extern bool g_bCapture2DOffscreenBuffer;
extern bool g_bStart3DCapture, g_bDo3DCapture;
extern FILE *g_HackFile;
#endif

/* SteamVR HMD */
#include <headers/openvr.h>
extern vr::IVRSystem *g_pHMD;
extern vr::IVRCompositor *g_pVRCompositor;
extern bool g_bSteamVREnabled, g_bUseSteamVR, g_bRunDedicatedSteamVRThread, g_bSteamVRTexturesInitialized;
extern uint32_t g_steamVRWidth, g_steamVRHeight;
extern HANDLE g_hDedicatedSteamVRThread, g_hCompositorTexMutex, g_hWaitGetPosesSignal, g_hCanSubmit;
extern vr::TrackedDevicePose_t g_rTrackedDevicePose;
void *g_pSurface = NULL;
void WaitGetPoses();

// void capture()
#ifdef DBR_VR
void PrimarySurface::capture(int time_delay, ComPtr<ID3D11Texture2D> buffer, const wchar_t *filename)
{
	bool bDoCapture = false;
	HRESULT hr = D3D_OK;

	hr = DirectX::SaveWICTextureToFile(this->_deviceResources->_d3dDeviceContext.Get(),
		buffer.Get(), GUID_ContainerFormatJpeg, filename);

	if (SUCCEEDED(hr))
		log_debug("[DBG] CAPTURED (1)!");
	else
		log_debug("[DBG] NOT captured, hr: %d", hr);
}
#endif

PrimarySurface::PrimarySurface(DeviceResources* deviceResources, bool hasBackbufferAttached)
{
	this->_refCount = 1;
	this->_deviceResources = deviceResources;

	this->_hasBackbufferAttached = hasBackbufferAttached;

	if (this->_hasBackbufferAttached)
	{
		*this->_backbufferSurface.GetAddressOf() = new BackbufferSurface(this->_deviceResources);
		this->_deviceResources->_backbufferSurface = this->_backbufferSurface;
	}

	this->_flipFrames = 0;
}

PrimarySurface::~PrimarySurface()
{
	if (this->_hasBackbufferAttached)
	{
		for (ULONG ref = this->_backbufferSurface->AddRef(); ref > 1; ref--)
		{
			this->_backbufferSurface->Release();
		}
	}

	if (this->_deviceResources->_primarySurface == this)
	{
		this->_deviceResources->_primarySurface = nullptr;
	}
}

HRESULT PrimarySurface::QueryInterface(
	REFIID riid,
	LPVOID* obp
	)
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	LogText(str.str());
#endif

#if LOGGER
	str.str("\tE_NOINTERFACE");
	LogText(str.str());
#endif

	return E_NOINTERFACE;
}

ULONG PrimarySurface::AddRef()
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	LogText(str.str());
#endif

	this->_refCount++;

#if LOGGER
	str.str("");
	str << "\t" << this->_refCount;
	LogText(str.str());
#endif

	return this->_refCount;
}

ULONG PrimarySurface::Release()
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	LogText(str.str());
#endif

	this->_refCount--;

#if LOGGER
	str.str("");
	str << "\t" << this->_refCount;
	LogText(str.str());
#endif

	if (this->_refCount == 0)
	{
		delete this;
		return 0;
	}

	return this->_refCount;
}

HRESULT PrimarySurface::AddAttachedSurface(
	LPDIRECTDRAWSURFACE lpDDSAttachedSurface
	)
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	LogText(str.str());
#endif

#if LOGGER
	str.str("\tDDERR_UNSUPPORTED");
	LogText(str.str());
#endif

	return DDERR_UNSUPPORTED;
}

HRESULT PrimarySurface::AddOverlayDirtyRect(
	LPRECT lpRect
	)
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	LogText(str.str());
#endif

#if LOGGER
	str.str("\tDDERR_UNSUPPORTED");
	LogText(str.str());
#endif

	return DDERR_UNSUPPORTED;
}

HRESULT PrimarySurface::Blt(
	LPRECT lpDestRect,
	LPDIRECTDRAWSURFACE lpDDSrcSurface,
	LPRECT lpSrcRect,
	DWORD dwFlags,
	LPDDBLTFX lpDDBltFx
	)
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	str << tostr_RECT(lpDestRect);

	if (lpDDSrcSurface == this->_deviceResources->_backbufferSurface)
	{
		str << " BackbufferSurface";
	}
	else
	{
		str << " " << lpDDSrcSurface;
	}

	str << tostr_RECT(lpSrcRect);

	if ((dwFlags & DDBLT_COLORFILL) != 0 && lpDDBltFx != nullptr)
	{
		str << " " << (void*)lpDDBltFx->dwFillColor;
	}

	LogText(str.str());
#endif

	if (dwFlags & DDBLT_COLORFILL)
	{
		if (lpDDBltFx == nullptr)
		{
			return DDERR_INVALIDPARAMS;
		}

		return DD_OK;
	}

	if (lpDDSrcSurface != nullptr)
	{
		if (lpDDSrcSurface == this->_deviceResources->_backbufferSurface)
		{
			return this->Flip(NULL, 0);
		}
	}

#if LOGGER
	str.str("\tDDERR_UNSUPPORTED");
	LogText(str.str());
#endif

	return DDERR_UNSUPPORTED;
}

HRESULT PrimarySurface::BltBatch(
	LPDDBLTBATCH lpDDBltBatch,
	DWORD dwCount,
	DWORD dwFlags
	)
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	LogText(str.str());
#endif

#if LOGGER
	str.str("\tDDERR_UNSUPPORTED");
	LogText(str.str());
#endif

	return DDERR_UNSUPPORTED;
}

HRESULT PrimarySurface::BltFast(
	DWORD dwX,
	DWORD dwY,
	LPDIRECTDRAWSURFACE lpDDSrcSurface,
	LPRECT lpSrcRect,
	DWORD dwTrans
	)
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	str << " " << dwX << " " << dwY;

	if (lpDDSrcSurface == nullptr)
	{
		str << " NULL";
	}
	else if (lpDDSrcSurface == this->_deviceResources->_backbufferSurface)
	{
		str << " BackbufferSurface";
	}
	else
	{
		str << " " << lpDDSrcSurface;
	}

	str << tostr_RECT(lpSrcRect);

	if (dwTrans & DDBLTFAST_SRCCOLORKEY)
	{
		str << " SRCCOLORKEY";
	}

	if (dwTrans & DDBLTFAST_DESTCOLORKEY)
	{
		str << " DESTCOLORKEY";
	}

	if (dwTrans & DDBLTFAST_WAIT)
	{
		str << " WAIT";
	}

	if (dwTrans & DDBLTFAST_DONOTWAIT)
	{
		str << " DONOTWAIT";
	}

	LogText(str.str());
#endif

	if (lpDDSrcSurface != nullptr)
	{
		if (lpDDSrcSurface == this->_deviceResources->_backbufferSurface)
		{
			return this->Flip(this->_deviceResources->_backbufferSurface, 0);
		}
	}

#if LOGGER
	str.str("\tDDERR_UNSUPPORTED");
	LogText(str.str());
#endif

	return DDERR_UNSUPPORTED;
}

HRESULT PrimarySurface::DeleteAttachedSurface(
	DWORD dwFlags,
	LPDIRECTDRAWSURFACE lpDDSAttachedSurface
	)
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	LogText(str.str());
#endif

#if LOGGER
	str.str("\tDDERR_UNSUPPORTED");
	LogText(str.str());
#endif

	return DDERR_UNSUPPORTED;
}

HRESULT PrimarySurface::EnumAttachedSurfaces(
	LPVOID lpContext,
	LPDDENUMSURFACESCALLBACK lpEnumSurfacesCallback
	)
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	LogText(str.str());
#endif

#if LOGGER
	str.str("\tDDERR_UNSUPPORTED");
	LogText(str.str());
#endif

	return DDERR_UNSUPPORTED;
}

HRESULT PrimarySurface::EnumOverlayZOrders(
	DWORD dwFlags,
	LPVOID lpContext,
	LPDDENUMSURFACESCALLBACK lpfnCallback
	)
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	LogText(str.str());
#endif

#if LOGGER
	str.str("\tDDERR_UNSUPPORTED");
	LogText(str.str());
#endif

	return DDERR_UNSUPPORTED;
}

/*
 * Applies the barrel distortion effect on the 2D window (Concourse, menus, etc).
 */
void PrimarySurface::barrelEffect2D(int iteration) {
	/*
	We need to avoid resolving the offscreen buffer multiple times. It's probably easier to
	skip method this altogether if we already rendered all this in the first iteration.
	*/
	if (iteration > 0)
		return;

	D3D11_VIEWPORT viewport{};
	auto& resources = this->_deviceResources;
	auto& device = resources->_d3dDevice;
	auto& context = resources->_d3dDeviceContext;

	float screen_res_x = (float)resources->_backbufferWidth;
	float screen_res_y = (float)resources->_backbufferHeight;
	float bgColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	/* Create the VertexBuffer if necessary */
	if (g_BarrelEffectVertBuffer == NULL)
	{
		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = sizeof(MainVertex) * ARRAYSIZE(g_BarrelEffectVertices);
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA vertexBufferData;

		ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
		vertexBufferData.pSysMem = g_BarrelEffectVertices;
		device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &g_BarrelEffectVertBuffer);
	}

	// Set the vertex buffer
	UINT stride = sizeof(MainVertex);
	UINT offset = 0;
	resources->InitVertexBuffer(&g_BarrelEffectVertBuffer, &stride, &offset);

	// Set Primitive Topology
	// This is probably an opportunity for an optimization: let's use the same topology everywhere?
	resources->InitTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	resources->InitInputLayout(resources->_mainInputLayout);

	// At this point, offscreenBuffer contains the image that would be rendered to the screen by
	// copying to the backbuffer. So, resolve the offscreen buffer into offscreenBuffer2 to use it
	// as input, so that we can render the image twice.
	context->ResolveSubresource(resources->_offscreenBufferAsInput, 0, resources->_offscreenBuffer,
		0, DXGI_FORMAT_B8G8R8A8_UNORM);

#ifdef DBG_VR
	if (g_bCapture2DOffscreenBuffer) {
		static int frame = 0;
		if (frame == 0) {
			log_debug("[DBG] [Capture] display Width, Height: %d, %d",
				this->_deviceResources->_displayWidth, this->_deviceResources->_displayHeight);
			log_debug("[DBG] [Capture] backBuffer Width, Height: %d, %d",
				this->_deviceResources->_backbufferWidth, this->_deviceResources->_backbufferHeight);
		}
		wchar_t filename[120];
		swprintf_s(filename, 120, L"c:\\temp\\offscreenBuf-%d-A.jpg", frame++);
		capture(0, this->_deviceResources->_offscreenBuffer2, filename);
		if (frame >= 40)
			g_bCapture2DOffscreenBuffer = false;
	}
#endif

	// Render the barrel effect
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = screen_res_x;
	viewport.Height = screen_res_y;
	viewport.MaxDepth = D3D11_MAX_DEPTH;
	viewport.MinDepth = D3D11_MIN_DEPTH;

	resources->InitVSConstantBuffer2D(resources->_mainShadersConstantBuffer.GetAddressOf(), 0.0f, 1.0f, 1.0f, 1.0f);
	resources->InitVertexShader(resources->_mainVertexShader);
	resources->InitPixelShader(resources->_barrelPixelShader);
	
	// Set the lens distortion constants for the barrel shader
	resources->InitPSConstantBufferBarrel(resources->_barrelConstantBuffer.GetAddressOf(), g_fLensK1, g_fLensK2, g_fLensK3);

	context->ClearDepthStencilView(this->_deviceResources->_depthStencilViewL, D3D11_CLEAR_DEPTH, 1.0f, 0);
	context->ClearRenderTargetView(this->_deviceResources->_renderTargetViewPost, bgColor);
	context->OMSetRenderTargets(1, this->_deviceResources->_renderTargetViewPost.GetAddressOf(), this->_deviceResources->_depthStencilViewL.Get());
	context->PSSetShaderResources(0, 1, this->_deviceResources->_offscreenAsInputShaderResourceView.GetAddressOf());

	resources->InitViewport(&viewport);
	context->Draw(6, 0);

	// Restore the original rendertargetview
	context->OMSetRenderTargets(1, this->_deviceResources->_renderTargetView.GetAddressOf(), this->_deviceResources->_depthStencilViewL.Get());
}

/*
 * Applies the barrel distortion effect on the 3D window.
 */
void PrimarySurface::barrelEffect3D() {
	auto& resources = this->_deviceResources;
	auto& device = resources->_d3dDevice;
	auto& context = resources->_d3dDeviceContext;

	// Create the VertexBuffer if necessary
	if (g_BarrelEffectVertBuffer == NULL) {
		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = sizeof(MainVertex) * ARRAYSIZE(g_BarrelEffectVertices);
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA vertexBufferData;

		ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
		vertexBufferData.pSysMem = g_BarrelEffectVertices;
		device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &g_BarrelEffectVertBuffer);
	}
	// Set the vertex buffer
	UINT stride = sizeof(MainVertex);
	UINT offset = 0;
	resources->InitVertexBuffer(&g_BarrelEffectVertBuffer, &stride, &offset);

	// Set Primitive Topology
	// Opportunity for optimization? Make all draw calls use the same topology?
	resources->InitTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	resources->InitInputLayout(resources->_mainInputLayout);
	
	// Temporarily disable ZWrite: we won't need it for the barrel effect
	D3D11_DEPTH_STENCIL_DESC desc;
	ComPtr<ID3D11DepthStencilState> depthState;
	desc.DepthEnable = FALSE;
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	desc.StencilEnable = FALSE;
	resources->InitDepthStencilState(depthState, &desc);

	// Create a new viewport to render the offscreen buffer as a texture
	D3D11_VIEWPORT viewport{};
	float screen_res_x = (float)resources->_backbufferWidth;
	float screen_res_y = (float)resources->_backbufferHeight;
	float bgColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	viewport.TopLeftX = (float)0;
	viewport.TopLeftY = (float)0;
	viewport.Width = (float)screen_res_x;
	viewport.Height = (float)screen_res_y;
	viewport.MaxDepth = D3D11_MAX_DEPTH;
	viewport.MinDepth = D3D11_MIN_DEPTH;

	context->ResolveSubresource(resources->_offscreenBufferAsInput, 0, resources->_offscreenBuffer,
		0, DXGI_FORMAT_B8G8R8A8_UNORM);

	resources->InitVSConstantBuffer2D(resources->_mainShadersConstantBuffer.GetAddressOf(), 0.0f, 1.0f, 1.0f, 1.0f);
	resources->InitVertexShader(resources->_mainVertexShader);
	resources->InitPixelShader(resources->_barrelPixelShader);
	
	context->PSSetShaderResources(0, 1, resources->_offscreenAsInputShaderResourceView.GetAddressOf());
	// Set the lens distortion constants for the barrel shader
	resources->InitPSConstantBufferBarrel(resources->_barrelConstantBuffer.GetAddressOf(), g_fLensK1, g_fLensK2, g_fLensK3);

	// Clear the depth stencil
	context->ClearDepthStencilView(resources->_depthStencilViewL, D3D11_CLEAR_DEPTH, 1.0f, 0);
	// Clear the render target
	context->ClearRenderTargetView(resources->_renderTargetViewPost, bgColor);
	context->OMSetRenderTargets(1, resources->_renderTargetViewPost.GetAddressOf(), 
		this->_deviceResources->_depthStencilViewL.Get());
	resources->InitViewport(&viewport);
	context->IASetInputLayout(resources->_mainInputLayout);
	context->Draw(6, 0);

#ifdef DBG_VR
	if (g_bCapture2DOffscreenBuffer) {
		static int frame = 0;
		wchar_t filename[120];

		swprintf_s(filename, 120, L"c:\\temp\\offscreenBuf-%d.jpg", frame);
		capture(0, this->_deviceResources->_offscreenBuffer, filename);

		swprintf_s(filename, 120, L"c:\\temp\\offscreenBuf2-%d.jpg", frame);
		capture(0, this->_deviceResources->_offscreenBuffer2, filename);

		swprintf_s(filename, 120, L"c:\\temp\\offscreenBuf3-%d.jpg", frame);
		capture(0, this->_deviceResources->_offscreenBuffer3, filename);

		frame++;
		if (frame >= 5)
			g_bCapture2DOffscreenBuffer = false;
	}
#endif

	// Restore previous rendertarget, etc
	resources->InitInputLayout(resources->_inputLayout);
	context->OMSetRenderTargets(1, this->_deviceResources->_renderTargetView.GetAddressOf(), 
		this->_deviceResources->_depthStencilViewL.Get());
}

/*
 * Applies the barrel distortion effect on the 3D window for SteamVR (so each image is
 * independent and the singleBarrelPixelShader is used.
 * Input: _offscreenBuffer and _offscreenBufferR
 * Output: _offscreenBufferPost and _offscreenBufferPostR
 */
void PrimarySurface::barrelEffectSteamVR() {
	auto& resources = this->_deviceResources;
	auto& device = resources->_d3dDevice;
	auto& context = resources->_d3dDeviceContext;

	// Create the VertexBuffer if necessary
	if (g_BarrelEffectVertBuffer == NULL) {
		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = sizeof(MainVertex) * ARRAYSIZE(g_BarrelEffectVertices);
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA vertexBufferData;
		ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
		vertexBufferData.pSysMem = g_BarrelEffectVertices;
		device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &g_BarrelEffectVertBuffer);
	}
	// Set the vertex buffer
	UINT stride = sizeof(MainVertex);
	UINT offset = 0;
	resources->InitVertexBuffer(&g_BarrelEffectVertBuffer, &stride, &offset);

	// Set Primitive Topology
	// Opportunity for optimization? Make all draw calls use the same topology?
	resources->InitTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	resources->InitInputLayout(resources->_mainInputLayout);

	// Temporarily disable ZWrite: we won't need it for the barrel effect
	D3D11_DEPTH_STENCIL_DESC desc;
	ComPtr<ID3D11DepthStencilState> depthState;
	desc.DepthEnable = FALSE;
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	desc.StencilEnable = FALSE;
	resources->InitDepthStencilState(depthState, &desc);

	// Create a new viewport to render the offscreen buffer as a texture
	D3D11_VIEWPORT viewport{};
	float screen_res_x = (float)resources->_backbufferWidth;
	float screen_res_y = (float)resources->_backbufferHeight;
	float bgColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	viewport.TopLeftX = (float)0;
	viewport.TopLeftY = (float)0;
	viewport.Width = (float)screen_res_x;
	viewport.Height = (float)screen_res_y;
	viewport.MaxDepth = D3D11_MAX_DEPTH;
	viewport.MinDepth = D3D11_MIN_DEPTH;
	resources->InitViewport(&viewport);

	// Resolve both images
	context->ResolveSubresource(resources->_offscreenBufferAsInput, 0, resources->_offscreenBuffer,
		0, DXGI_FORMAT_B8G8R8A8_UNORM);
	context->ResolveSubresource(resources->_offscreenBufferAsInputR, 0, resources->_offscreenBufferR,
		0, DXGI_FORMAT_B8G8R8A8_UNORM);

#ifdef DBG_VR
	if (g_bCapture2DOffscreenBuffer) {
		static int frame = 0;
		wchar_t filename[120];

		log_debug("[DBG] Capturing buffers");
		swprintf_s(filename, 120, L"c:\\temp\\offscreenBuf-%d.jpg", frame);
		capture(0, this->_deviceResources->_offscreenBuffer, filename);

		swprintf_s(filename, 120, L"c:\\temp\\offscreenBufAsInput-%d.jpg", frame);
		capture(0, this->_deviceResources->_offscreenBufferAsInput, filename);

		frame++;
		if (frame >= 1)
			g_bCapture2DOffscreenBuffer = false;
	}
#endif

	resources->InitVSConstantBuffer2D(resources->_mainShadersConstantBuffer.GetAddressOf(), 0.0f, 1.0f, 1.0f, 1.0f);
	resources->InitVertexShader(resources->_mainVertexShader);
	resources->InitPixelShader(resources->_singleBarrelPixelShader);

	// Set the lens distortion constants for the barrel shader
	resources->InitPSConstantBufferBarrel(resources->_barrelConstantBuffer.GetAddressOf(), g_fLensK1, g_fLensK2, g_fLensK3);

	// Clear the depth stencil and render target
	context->ClearDepthStencilView(resources->_depthStencilViewL, D3D11_CLEAR_DEPTH, 1.0f, 0);
	context->ClearDepthStencilView(resources->_depthStencilViewR, D3D11_CLEAR_DEPTH, 1.0f, 0);
	context->ClearRenderTargetView(resources->_renderTargetViewPost, bgColor);
	context->ClearRenderTargetView(resources->_renderTargetViewPostR, bgColor);
	context->IASetInputLayout(resources->_mainInputLayout);

	context->PSSetShaderResources(0, 1, resources->_offscreenAsInputShaderResourceView.GetAddressOf());
	context->OMSetRenderTargets(1, resources->_renderTargetViewPost.GetAddressOf(),
		resources->_depthStencilViewL.Get());
	context->Draw(6, 0);

	context->PSSetShaderResources(0, 1, resources->_offscreenAsInputShaderResourceViewR.GetAddressOf());
	context->OMSetRenderTargets(1, resources->_renderTargetViewPostR.GetAddressOf(),
		resources->_depthStencilViewR.Get());
	context->Draw(6, 0);

#ifdef DBG_VR
	if (g_bCapture2DOffscreenBuffer) {
		static int frame = 0;
		wchar_t filename[120];

		swprintf_s(filename, 120, L"c:\\temp\\offscreenBufR-%d.jpg", frame);
		capture(0, resources->_offscreenBufferR, filename);

		swprintf_s(filename, 120, L"c:\\temp\\offscreenBufAsInputR-%d.jpg", frame);
		capture(0, resources->_offscreenBufferAsInputR, filename);

		swprintf_s(filename, 120, L"c:\\temp\\offscreenBufPostR-%d.jpg", frame);
		capture(0, resources->_offscreenBufferPostR, filename);

		frame++;
		if (frame >= 1)
			g_bCapture2DOffscreenBuffer = false;
	}
#endif

	// Restore previous rendertarget, etc
	resources->InitInputLayout(resources->_inputLayout);
	context->OMSetRenderTargets(1, resources->_renderTargetView.GetAddressOf(),
		resources->_depthStencilViewL.Get());
}

/*
 * When rendering for SteamVR, we're usually rendering at half the width; but the Present is done
 * at full resolution, so we need to resize the offscreenBuffer before presenting it.
 * Input: _offscreenBuffer
 * Output: _steamVRPresentBuffer
 */
void PrimarySurface::resizeForSteamVR(int iteration) {
	/*
	We need to avoid resolving the offscreen buffer multiple times. It's probably easier to
	skip method this altogether if we already rendered all this in the first iteration.
	*/
	if (iteration > 0)
		return;

	D3D11_VIEWPORT viewport{};
	auto& resources = this->_deviceResources;
	auto& device = resources->_d3dDevice;
	auto& context = resources->_d3dDeviceContext;

	float screen_res_x = (float)g_FullScreenWidth;
	float screen_res_y = (float)g_FullScreenHeight;
	float bgColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	// Set the vertex buffer
	UINT stride = sizeof(MainVertex);
	UINT offset = 0;
	resources->InitVertexBuffer(resources->_mainVertexBuffer.GetAddressOf(), &stride, &offset);
	resources->InitIndexBuffer(resources->_mainIndexBuffer);

	// Set Primitive Topology
	// This is probably an opportunity for an optimization: let's use the same topology everywhere?
	resources->InitTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	resources->InitInputLayout(resources->_mainInputLayout);

	// Temporarily disable ZWrite: we won't need it for the barrel effect
	D3D11_DEPTH_STENCIL_DESC desc;
	ComPtr<ID3D11DepthStencilState> depthState;
	desc.DepthEnable = FALSE;
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	desc.StencilEnable = FALSE;
	resources->InitDepthStencilState(depthState, &desc);

	// At this point, offscreenBuffer contains the image that would be rendered to the screen by
	// copying to the backbuffer. So, resolve the offscreen buffer into offscreenBufferAsInput to
	// use it as input
	context->ResolveSubresource(resources->_offscreenBufferAsInput, 0, resources->_offscreenBuffer,
		0, DXGI_FORMAT_B8G8R8A8_UNORM);

#ifdef DBG_VR
	if (g_bCapture2DOffscreenBuffer) {
		static int frame = 0;
		if (frame == 0) {
			log_debug("[DBG] [Capture] display Width, Height: %d, %d",
				this->_deviceResources->_displayWidth, this->_deviceResources->_displayHeight);
			log_debug("[DBG] [Capture] backBuffer Width, Height: %d, %d",
				this->_deviceResources->_backbufferWidth, this->_deviceResources->_backbufferHeight);
		}
		wchar_t filename[120];
		swprintf_s(filename, 120, L"c:\\temp\\offscreenBuf-%d-A.jpg", frame++);
		capture(0, this->_deviceResources->_offscreenBuffer2, filename);
		if (frame >= 40)
			g_bCapture2DOffscreenBuffer = false;
	}
#endif

	// Resize the buffer to be presented for SteamVR
	float scale_x = screen_res_x / g_steamVRWidth;
	float scale_y = screen_res_y / g_steamVRHeight;
	float scale = (scale_x + scale_y) / 2.0f;
	float newWidth = g_steamVRWidth * scale;
	float newHeight = g_steamVRHeight * scale;

	//viewport.TopLeftX = (screen_res_x - g_steamVRWidth) / 2.0f;
	//viewport.TopLeftY = 0.0f;
	//viewport.Width = (float)g_steamVRWidth;
	//viewport.Height = (float)g_steamVRHeight;

	viewport.TopLeftX = (screen_res_x - newWidth) / 2.0f;
	viewport.TopLeftY = (screen_res_y - newHeight) / 2.0f;
	viewport.Width = (float)newWidth;
	viewport.Height = (float)newHeight;
	viewport.MaxDepth = D3D11_MAX_DEPTH;
	viewport.MinDepth = D3D11_MIN_DEPTH;
	resources->InitViewport(&viewport);

	resources->InitVSConstantBuffer2D(resources->_mainShadersConstantBuffer.GetAddressOf(),
		0.0f, 1.0f, 1.0f, 1.0f);
	resources->InitPSConstantBuffer2D(resources->_mainShadersConstantBuffer.GetAddressOf(),
		0.0f, 1.0f, 1.0f, 1.0f);
	resources->InitVertexShader(resources->_mainVertexShader);
	resources->InitPixelShader(resources->_mainPixelShader);

	context->ClearDepthStencilView(resources->_depthStencilViewL, D3D11_CLEAR_DEPTH, 1.0f, 0);
	context->ClearRenderTargetView(resources->_renderTargetViewSteamVRResize, bgColor);
	context->OMSetRenderTargets(1, resources->_renderTargetViewSteamVRResize.GetAddressOf(),
		resources->_depthStencilViewL.Get());
	context->PSSetShaderResources(0, 1, resources->_offscreenAsInputShaderResourceView.GetAddressOf());
	context->DrawIndexed(6, 0, 0);

#ifdef DBG_VR
	if (g_bCapture2DOffscreenBuffer) {
		static int frame = 0;
		wchar_t filename[120];
		swprintf_s(filename, 120, L"c:\\temp\\offscreenBuf-%d.jpg", frame);
		capture(0, this->_deviceResources->_offscreenBuffer, filename);

		swprintf_s(filename, 120, L"c:\\temp\\offscreenBufAsInput-%d.jpg", frame);
		capture(0, this->_deviceResources->_offscreenBufferAsInput, filename);

		swprintf_s(filename, 120, L"c:\\temp\\steamVRPresentBuf-%d.jpg", frame);
		capture(0, this->_deviceResources->_steamVRPresentBuffer, filename);

		log_debug("[DBG] viewport: (%f,%f)-(%f,%f)", viewport.TopLeftX, viewport.TopLeftY,
			viewport.Width, viewport.Height);

		frame++;
		if (frame >= 0)
			g_bCapture2DOffscreenBuffer = false;
	}
#endif

	// Restore previous rendertarget, etc
	//resources->InitInputLayout(resources->_inputLayout);
	context->OMSetRenderTargets(1, resources->_renderTargetView.GetAddressOf(),
		resources->_depthStencilViewL.Get());
}

/* Convenience function to call WaitGetPoses() */
void WaitGetPoses() {
	// We need to call WaitGetPoses so that SteamVR gets the focus, otherwise we'll just get
	// error 101 when doing VRCompositor->Submit()
	if (g_pVRCompositor == NULL) log_debug("[DBG] VRCompositor is NULL");
	vr::EVRCompositorError error = g_pVRCompositor->WaitGetPoses(&g_rTrackedDevicePose,
		0, NULL, 0);
	if (error) log_debug("[DBG] WaitGetPoses error: %d", error);
}

HRESULT PrimarySurface::Flip(
	LPDIRECTDRAWSURFACE lpDDSurfaceTargetOverride,
	DWORD dwFlags
	)
{
	static uint64_t frame, lastFrame = 0;
	static float seconds;

	/*
	if (g_hDedicatedSteamVRThread == NULL) {
		// Create the Compositor Texture Mutex
		g_hCompositorTexMutex = CreateMutex(NULL, FALSE, NULL);
		g_hWaitGetPosesSignal = CreateEvent(NULL, TRUE, FALSE, NULL);
		g_hCanSubmit = CreateEvent(NULL, TRUE, FALSE, NULL);
		// Create and start the dedicated SteamVR thread
		g_bRunDedicatedSteamVRThread = true;
		g_hDedicatedSteamVRThread = CreateThread(NULL, 0, DedicatedSteamVRThread, (void *)this, 0, NULL);
		log_debug("[DBG] Created Dedicated thread");
	}
	//if (g_pSurface != NULL && g_pSurface != this) log_debug("[DBG] Warning g_pSurface changed!");
	*/

#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	if (lpDDSurfaceTargetOverride == nullptr)
	{
		str << " NULL";
	}
	else if (lpDDSurfaceTargetOverride == this->_deviceResources->_backbufferSurface)
	{
		str << " BackbufferSurface";
	}
	else if (lpDDSurfaceTargetOverride == this->_deviceResources->_frontbufferSurface)
	{
		str << " FrontbufferSurface";
	}
	else
	{
		str << " " << lpDDSurfaceTargetOverride;
	}

	if (dwFlags & DDFLIP_WAIT)
	{
		str << " WAIT";
	}

	LogText(str.str());
#endif

	this->_deviceResources->sceneRenderedEmpty = this->_deviceResources->sceneRendered == false;
	this->_deviceResources->sceneRendered = false;

	if (this->_deviceResources->sceneRenderedEmpty && this->_deviceResources->_frontbufferSurface != nullptr && this->_deviceResources->_frontbufferSurface->wasBltFastCalled)
	{
		this->_deviceResources->_d3dDeviceContext->ClearRenderTargetView(this->_deviceResources->_renderTargetView, this->_deviceResources->clearColor);
		if (g_bUseSteamVR) {
			this->_deviceResources->_d3dDeviceContext->ClearRenderTargetView(this->_deviceResources->_renderTargetViewR, this->_deviceResources->clearColor);
			this->_deviceResources->_d3dDeviceContext->ClearRenderTargetView(this->_deviceResources->_renderTargetViewPostR, this->_deviceResources->clearColor);
		}
		// Do we need to clear renderTargetViewPost?
		this->_deviceResources->_d3dDeviceContext->ClearRenderTargetView(this->_deviceResources->_renderTargetViewPost, this->_deviceResources->clearColor);
		this->_deviceResources->_d3dDeviceContext->ClearDepthStencilView(this->_deviceResources->_depthStencilViewL, D3D11_CLEAR_DEPTH, this->_deviceResources->clearDepth, 0);
		this->_deviceResources->_d3dDeviceContext->ClearDepthStencilView(this->_deviceResources->_depthStencilViewR, D3D11_CLEAR_DEPTH, this->_deviceResources->clearDepth, 0);
	}

	/* Present 2D content */
	if (lpDDSurfaceTargetOverride != nullptr)
	{
		if (lpDDSurfaceTargetOverride == this->_deviceResources->_frontbufferSurface)
		{
			HRESULT hr;

			if (FAILED(hr = this->_deviceResources->_backbufferSurface->BltFast(0, 0, this->_deviceResources->_frontbufferSurface, nullptr, 0)))
				return hr;

			return this->Flip(this->_deviceResources->_backbufferSurface, 0);
		}

		/* Present 2D content */
		if (lpDDSurfaceTargetOverride == this->_deviceResources->_backbufferSurface)
		{
			if (this->_deviceResources->_frontbufferSurface == nullptr)
			{
				if (FAILED(this->_deviceResources->RenderMain(this->_deviceResources->_backbufferSurface->_buffer, this->_deviceResources->_displayWidth, this->_deviceResources->_displayHeight, this->_deviceResources->_displayBpp)))
					return DDERR_GENERIC;
			}
			else
			{
				const unsigned short colorKey = 0x8080;

				int width = 640;
				int height = 480;

				int topBlack = 0;

				if (this->_deviceResources->_displayBpp == 2)
				{
					int w = this->_deviceResources->_displayWidth;
					int x = (this->_deviceResources->_displayWidth - width) / 2;
					int y = (this->_deviceResources->_displayHeight - height) / 2;
					unsigned short* buffer = (unsigned short*)this->_deviceResources->_backbufferSurface->_buffer + y * w + x;

					for (int i = 0; i < height / 2; i++)
					{
						if (buffer[i * w] != colorKey)
							break;

						topBlack++;
					}
				}
				else
				{
					const unsigned int colorKey32 = convertColorB5G6R5toB8G8R8X8(colorKey);

					int w = this->_deviceResources->_displayWidth;
					int x = (this->_deviceResources->_displayWidth - width) / 2;
					int y = (this->_deviceResources->_displayHeight - height) / 2;
					unsigned int* buffer = (unsigned int*)this->_deviceResources->_backbufferSurface->_buffer + y * w + x;

					for (int i = 0; i < height / 2; i++)
					{
						if (buffer[i * w] != colorKey32)
							break;

						topBlack++;
					}
				}

				if (topBlack < height / 2)
				{
					height -= topBlack * 2;
				}

				RECT rc;
				rc.left = (this->_deviceResources->_displayWidth - width) / 2;
				rc.top = (this->_deviceResources->_displayHeight - height) / 2;
				rc.right = rc.left + width;
				rc.bottom = rc.top + height;

				int length = width * height * this->_deviceResources->_displayBpp;
				char* buffer = new char[length];

				copySurface(buffer, width, height, this->_deviceResources->_displayBpp, this->_deviceResources->_backbufferSurface->_buffer, this->_deviceResources->_displayWidth, this->_deviceResources->_displayHeight, this->_deviceResources->_displayBpp, 0, 0, &rc, false);

				HRESULT hr = this->_deviceResources->RenderMain(buffer, width, height, this->_deviceResources->_displayBpp);

				delete[] buffer;

				if (FAILED(hr))
					return DDERR_GENERIC;
			}

			HRESULT hr;

			/* Present Concourse, ESC Screen Menu */
			if (this->_deviceResources->_swapChain)
			{
				UINT rate = 90 * this->_deviceResources->_refreshRate.Denominator;
				UINT numerator = this->_deviceResources->_refreshRate.Numerator + this->_flipFrames;

				UINT interval = numerator / rate;
				this->_flipFrames = numerator % rate;

				interval = max(interval, 1);

				hr = DD_OK;

				for (UINT i = 0; i < 1 /* interval */; i++)
				{
					// In the original code the offscreenBuffer is simply resolved into the backBuffer.
					// this->_deviceResources->_d3dDeviceContext->ResolveSubresource(this->_deviceResources->_backBuffer, 0, this->_deviceResources->_offscreenBuffer, 0, DXGI_FORMAT_B8G8R8A8_UNORM);

					if (!g_bDisableBarrelEffect && g_bEnableVR && !g_bUseSteamVR) {
						// Barrel effect enabled for DirectSBS mode
						barrelEffect2D(i);
						this->_deviceResources->_d3dDeviceContext->ResolveSubresource(this->_deviceResources->_backBuffer, 0,
							this->_deviceResources->_offscreenBufferPost, 0, DXGI_FORMAT_B8G8R8A8_UNORM);
					}
					else {
						/*
						if (g_bUseSteamVR) {
							// Try to copy the images to the stagingBuffers so that they can be submitted to SteamVR
							this->_deviceResources->_d3dDeviceContext->CopyResource(this->_deviceResources->_stagingBufferLeft,
								this->_deviceResources->_offscreenBuffer);
							this->_deviceResources->_d3dDeviceContext->CopyResource(this->_deviceResources->_stagingBufferRight,
								this->_deviceResources->_offscreenBufferR);							
						}
						*/
						// In SteamVR mode this will display the left image:
						if (g_bUseSteamVR) {
							resizeForSteamVR(0);
							this->_deviceResources->_d3dDeviceContext->ResolveSubresource(this->_deviceResources->_backBuffer, 0,
								this->_deviceResources->_steamVRPresentBuffer, 0, DXGI_FORMAT_B8G8R8A8_UNORM);
						}
						else {
							this->_deviceResources->_d3dDeviceContext->ResolveSubresource(this->_deviceResources->_backBuffer, 0,
								this->_deviceResources->_offscreenBuffer, 0, DXGI_FORMAT_B8G8R8A8_UNORM);
						}
					}
					
					// Capture the backBuffer to a JPG file
#ifdef DBG_VR
					/*
					if (g_bCapture2DOffscreenBuffer) {
						static int frame = 0;
						wchar_t filename[120];
						swprintf_s(filename, 120, L"c:\\temp\\backbuffer-%d.jpg", frame++);
						capture(0, this->_deviceResources->_backBuffer, filename);
						g_bCapture2DOffscreenBuffer = false;
					}
					*/
#endif

					// Enable capturing 3D objects even when rendering 2D. This happens in the Tech Library
#ifdef DBG_VR
					if (g_bStart3DCapture && !g_bDo3DCapture) {
						g_bDo3DCapture = true;
					}
					else if (g_bStart3DCapture && g_bDo3DCapture) {
						g_bDo3DCapture = false;
						g_bStart3DCapture = false;
						fclose(g_hack_file);
						g_hack_file = NULL;
					}
#endif

					// Reset the 2D draw counter -- that'll help us increase the parallax for the Tech Library
					g_iDraw2DCounter = 0;				
					if (g_bRendering3D) {
						// We're about to switch from 3D to 2D rendering --> This means we're in the Tech Library
						// Let's clear the render target for the next iteration or we'll get multiple images during
						// the animation
						float bgColor[4] = { 0, 0, 0, 0 };
						this->_deviceResources->_d3dDeviceContext->ClearRenderTargetView(
							this->_deviceResources->_renderTargetView, bgColor);
						if (g_bUseSteamVR) {
							this->_deviceResources->_d3dDeviceContext->ClearRenderTargetView(
								this->_deviceResources->_renderTargetViewR, bgColor);
							this->_deviceResources->_d3dDeviceContext->ClearRenderTargetView(
								this->_deviceResources->_renderTargetViewSteamVRResize, bgColor);
						}
					}

					if (g_bUseSteamVR) {					
						vr::EVRCompositorError error = vr::VRCompositorError_None;
						vr::Texture_t leftEyeTexture = { this->_deviceResources->_offscreenBuffer.Get(), vr::TextureType_DirectX, vr::ColorSpace_Auto };
						vr::Texture_t rightEyeTexture = { this->_deviceResources->_offscreenBufferR.Get(), vr::TextureType_DirectX, vr::ColorSpace_Auto };
						error = g_pVRCompositor->Submit(vr::Eye_Left, &leftEyeTexture);
						//if (error) log_debug("[DBG] SteamVR (L) error: %d", error);
						error = g_pVRCompositor->Submit(vr::Eye_Right, &rightEyeTexture);
						//if (error) log_debug("[DBG] SteamVR (R) error: %d", error);
						//g_pVRCompositor->PostPresentHandoff();
					}
					
					g_bRendering3D = false;
					// Present 2D
					if (FAILED(hr = this->_deviceResources->_swapChain->Present(0, 0)))
					{
						static bool messageShown = false;

						if (!messageShown)
						{
							MessageBox(nullptr, _com_error(hr).ErrorMessage(), __FUNCTION__, MB_ICONERROR);
						}

						messageShown = true;

						hr = DDERR_SURFACELOST;
						break;
					}
					if (g_bUseSteamVR) {
						g_pVRCompositor->PostPresentHandoff();
						//g_pHMD->GetTimeSinceLastVsync(&seconds, &frame);
						//if (seconds > 0.008)
						WaitGetPoses();
					}		
				}
			}
			else
			{
				hr = DD_OK;
			}

			if (this->_deviceResources->_frontbufferSurface != nullptr)
			{
				if (this->_deviceResources->_frontbufferSurface->wasBltFastCalled)
				{
					memcpy(this->_deviceResources->_frontbufferSurface->_buffer2, this->_deviceResources->_frontbufferSurface->_buffer, this->_deviceResources->_frontbufferSurface->_bufferSize);
				}

				this->_deviceResources->_frontbufferSurface->wasBltFastCalled = false;
			}

			return hr;
		}
	}
	/* Present 3D content */
	else
	{
		HRESULT hr;

		if (this->_deviceResources->_swapChain)
		{
			hr = DD_OK;

			// In the original code, the offscreenBuffer is resolved to the backBuffer
			//this->_deviceResources->_d3dDeviceContext->ResolveSubresource(this->_deviceResources->_backBuffer, 0, this->_deviceResources->_offscreenBuffer, 0, DXGI_FORMAT_B8G8R8A8_UNORM);

			// The offscreenBuffer contains the fully-rendered image at this point.
			if (g_bEnableVR) {
				if (g_bUseSteamVR) {
					if (!g_bDisableBarrelEffect) {
						// Do the barrel effect (_offscreenBuffer -> _offscreenBufferPost)
						barrelEffectSteamVR();
						this->_deviceResources->_d3dDeviceContext->CopyResource(this->_deviceResources->_offscreenBuffer,
							this->_deviceResources->_offscreenBufferPost);
					}
					// Resize the buffer to be presented (_offscreenBuffer -> _steamVRPresentBuffer)
					resizeForSteamVR(0);
					// Resolve steamVRPresentBuffer to backBuffer so that it gets presented
					this->_deviceResources->_d3dDeviceContext->ResolveSubresource(this->_deviceResources->_backBuffer, 0,
						this->_deviceResources->_steamVRPresentBuffer, 0, DXGI_FORMAT_B8G8R8A8_UNORM);
				} else { // Direct SBS mode
					if (!g_bDisableBarrelEffect) {
						barrelEffect3D();
						this->_deviceResources->_d3dDeviceContext->ResolveSubresource(this->_deviceResources->_backBuffer, 0,
							this->_deviceResources->_offscreenBufferPost, 0, DXGI_FORMAT_B8G8R8A8_UNORM);
					} else
						this->_deviceResources->_d3dDeviceContext->ResolveSubresource(this->_deviceResources->_backBuffer, 0,
							this->_deviceResources->_offscreenBuffer, 0, DXGI_FORMAT_B8G8R8A8_UNORM);
				}
			} else // Non-VR mode
				this->_deviceResources->_d3dDeviceContext->ResolveSubresource(this->_deviceResources->_backBuffer, 0,
					this->_deviceResources->_offscreenBuffer, 0, DXGI_FORMAT_B8G8R8A8_UNORM);

			// Let's reset some frame counters and other control variables
			g_iDrawCounter = 0; g_iExecBufCounter = 0; g_iNonZBufferCounter = 0;
			g_iFloatingGUIDrawnCounter = 0;
			g_bTargetCompDrawn = false;
			g_bStartedGUI = false;
			
			// Perform the lean left/right etc animations
			//animTickX();
			//animTickY();
			//animTickZ();

#ifdef DBG_VR
			if (g_bStart3DCapture && !g_bDo3DCapture) {
				g_bDo3DCapture = true;
			}
			else if (g_bStart3DCapture && g_bDo3DCapture) {
				g_bDo3DCapture = false;
				g_bStart3DCapture = false;
				fclose(g_hack_file);
				g_hack_file = NULL;
			}
#endif

			if (g_bUseSteamVR) {
				//if (!g_pHMD->GetTimeSinceLastVsync(&seconds, &frame))
				//	log_debug("[DBG] No Vsync info available");

				vr::EVRCompositorError error = vr::VRCompositorError_None;
				vr::Texture_t leftEyeTexture;
				vr::Texture_t rightEyeTexture;

				if (g_bDisableBarrelEffect) {
					leftEyeTexture = { this->_deviceResources->_offscreenBuffer.Get(), vr::TextureType_DirectX, vr::ColorSpace_Auto };
					rightEyeTexture = { this->_deviceResources->_offscreenBufferR.Get(), vr::TextureType_DirectX, vr::ColorSpace_Auto };
				}
				else {
					leftEyeTexture = { this->_deviceResources->_offscreenBufferPost.Get(), vr::TextureType_DirectX, vr::ColorSpace_Auto };
					rightEyeTexture = { this->_deviceResources->_offscreenBufferPostR.Get(), vr::TextureType_DirectX, vr::ColorSpace_Auto };
				}
				error = g_pVRCompositor->Submit(vr::Eye_Left, &leftEyeTexture);
				//if (error) log_debug("[DBG] 3D Present (left) SteamVR error: %d", error);
				error = g_pVRCompositor->Submit(vr::Eye_Right, &rightEyeTexture);
				//if (error) log_debug("[DBG] 3D Present (right) SteamVR error: %d", error);
				//g_pVRCompositor->PostPresentHandoff();
			}

			//float timeRemaining = g_pVRCompositor->GetFrameTimeRemaining();
			//log_debug("[DBG] Time remaining: %0.3f", timeRemaining);
			//if (timeRemaining < 0.005) WaitGetPoses();

			// We're about to show 3D content, so let's set the corresponding flag
			g_bRendering3D = true;
			// Doing Present(1, 0) limits the framerate to 30fps, without it, it can go up to 60; but usually
			// stays around 45 in my system
			if (FAILED(hr = this->_deviceResources->_swapChain->Present(0, 0)))
			{
				static bool messageShown = false;

				if (!messageShown)
				{
					MessageBox(nullptr, _com_error(hr).ErrorMessage(), __FUNCTION__, MB_ICONERROR);
				}

				messageShown = true;

				hr = DDERR_SURFACELOST;
			}
			if (g_bUseSteamVR) {
				g_pVRCompositor->PostPresentHandoff();
				//g_pHMD->GetTimeSinceLastVsync(&seconds, &frame);
				//if (seconds > 0.008)
				WaitGetPoses();
			}
		}
		else
		{
			hr = DD_OK;
		}

		return hr;
	}

#if LOGGER
	str.str("\tDDERR_UNSUPPORTED");
	LogText(str.str());
#endif

	return DDERR_UNSUPPORTED;
}

HRESULT PrimarySurface::GetAttachedSurface(
	LPDDSCAPS lpDDSCaps,
	LPDIRECTDRAWSURFACE FAR *lplpDDAttachedSurface
	)
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	LogText(str.str());
#endif

	if (lpDDSCaps == nullptr)
	{
#if LOGGER
		str.str("\tDDERR_INVALIDPARAMS");
		LogText(str.str());
#endif

		return DDERR_INVALIDPARAMS;
	}

	if (lpDDSCaps->dwCaps & DDSCAPS_BACKBUFFER)
	{
		if (!this->_hasBackbufferAttached)
		{
#if LOGGER
			str.str("\tDDERR_INVALIDOBJECT");
			LogText(str.str());
#endif

			return DDERR_INVALIDOBJECT;
		}

		*lplpDDAttachedSurface = this->_backbufferSurface;
		this->_backbufferSurface->AddRef();

#if LOGGER
		str.str("");
		str << "\tBackbufferSurface\t" << *lplpDDAttachedSurface;
		LogText(str.str());
#endif

		return DD_OK;
	}

	return DDERR_UNSUPPORTED;
}

HRESULT PrimarySurface::GetBltStatus(
	DWORD dwFlags
	)
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	LogText(str.str());
#endif

#if LOGGER
	str.str("\tDDERR_UNSUPPORTED");
	LogText(str.str());
#endif

	return DDERR_UNSUPPORTED;
}

HRESULT PrimarySurface::GetCaps(
	LPDDSCAPS lpDDSCaps
	)
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	LogText(str.str());
#endif

#if LOGGER
	str.str("\tDDERR_UNSUPPORTED");
	LogText(str.str());
#endif

	return DDERR_UNSUPPORTED;
}

HRESULT PrimarySurface::GetClipper(
	LPDIRECTDRAWCLIPPER FAR *lplpDDClipper
	)
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	LogText(str.str());
#endif

#if LOGGER
	str.str("\tDDERR_UNSUPPORTED");
	LogText(str.str());
#endif

	return DDERR_UNSUPPORTED;
}

HRESULT PrimarySurface::GetColorKey(
	DWORD dwFlags,
	LPDDCOLORKEY lpDDColorKey
	)
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	LogText(str.str());
#endif

#if LOGGER
	str.str("\tDDERR_UNSUPPORTED");
	LogText(str.str());
#endif

	return DDERR_UNSUPPORTED;
}

HRESULT PrimarySurface::GetDC(
	HDC FAR *lphDC
	)
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	LogText(str.str());
#endif

#if LOGGER
	str.str("\tDDERR_UNSUPPORTED");
	LogText(str.str());
#endif

	return DDERR_UNSUPPORTED;
}

HRESULT PrimarySurface::GetFlipStatus(
	DWORD dwFlags
	)
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	LogText(str.str());
#endif

#if LOGGER
	str.str("\tDDERR_UNSUPPORTED");
	LogText(str.str());
#endif

	return DDERR_UNSUPPORTED;
}

HRESULT PrimarySurface::GetOverlayPosition(
	LPLONG lplX,
	LPLONG lplY
	)
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	LogText(str.str());
#endif

#if LOGGER
	str.str("\tDDERR_UNSUPPORTED");
	LogText(str.str());
#endif

	return DDERR_UNSUPPORTED;
}

HRESULT PrimarySurface::GetPalette(
	LPDIRECTDRAWPALETTE FAR *lplpDDPalette
	)
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	LogText(str.str());
#endif

#if LOGGER
	str.str("\tDDERR_UNSUPPORTED");
	LogText(str.str());
#endif

	return DDERR_UNSUPPORTED;
}

HRESULT PrimarySurface::GetPixelFormat(
	LPDDPIXELFORMAT lpDDPixelFormat
	)
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	LogText(str.str());
#endif

#if LOGGER
	str.str("\tDDERR_UNSUPPORTED");
	LogText(str.str());
#endif

	return DDERR_UNSUPPORTED;
}

HRESULT PrimarySurface::GetSurfaceDesc(
	LPDDSURFACEDESC lpDDSurfaceDesc
	)
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	LogText(str.str());
#endif

	if (lpDDSurfaceDesc == nullptr)
	{
#if LOGGER
		str.str("\tDDERR_INVALIDPARAMS");
		LogText(str.str());
#endif

		return DDERR_INVALIDPARAMS;
	}

	*lpDDSurfaceDesc = {};
	lpDDSurfaceDesc->dwSize = sizeof(DDSURFACEDESC);
	lpDDSurfaceDesc->dwFlags = DDSD_CAPS | DDSD_PIXELFORMAT | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PITCH;
	lpDDSurfaceDesc->ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_VIDEOMEMORY;
	lpDDSurfaceDesc->ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
	lpDDSurfaceDesc->ddpfPixelFormat.dwFlags = DDPF_RGB;
	lpDDSurfaceDesc->ddpfPixelFormat.dwRGBBitCount = 16;
	lpDDSurfaceDesc->ddpfPixelFormat.dwRBitMask = 0xF800;
	lpDDSurfaceDesc->ddpfPixelFormat.dwGBitMask = 0x7E0;
	lpDDSurfaceDesc->ddpfPixelFormat.dwBBitMask = 0x1F;
	lpDDSurfaceDesc->dwHeight = this->_deviceResources->_displayHeight;
	lpDDSurfaceDesc->dwWidth = this->_deviceResources->_displayWidth;
	lpDDSurfaceDesc->lPitch = this->_deviceResources->_displayWidth * 2;

#if LOGGER
	str.str("");
	str << "\t" << tostr_DDSURFACEDESC(lpDDSurfaceDesc);
	LogText(str.str());
#endif

	return DD_OK;
}

HRESULT PrimarySurface::Initialize(
	LPDIRECTDRAW lpDD,
	LPDDSURFACEDESC lpDDSurfaceDesc
	)
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	LogText(str.str());
#endif

#if LOGGER
	str.str("\tDDERR_UNSUPPORTED");
	LogText(str.str());
#endif

	return DDERR_UNSUPPORTED;
}

HRESULT PrimarySurface::IsLost()
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	LogText(str.str());
#endif

#if LOGGER
	str.str("\tDDERR_UNSUPPORTED");
	LogText(str.str());
#endif

	return DDERR_UNSUPPORTED;
}

HRESULT PrimarySurface::Lock(
	LPRECT lpDestRect,
	LPDDSURFACEDESC lpDDSurfaceDesc,
	DWORD dwFlags,
	HANDLE hEvent
	)
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	LogText(str.str());
#endif

#if LOGGER
	str.str("\tDDERR_UNSUPPORTED");
	LogText(str.str());
#endif

	return DDERR_UNSUPPORTED;
}

HRESULT PrimarySurface::ReleaseDC(
	HDC hDC
	)
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	LogText(str.str());
#endif

#if LOGGER
	str.str("\tDDERR_UNSUPPORTED");
	LogText(str.str());
#endif

	return DDERR_UNSUPPORTED;
}

HRESULT PrimarySurface::Restore()
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	LogText(str.str());
#endif

#if LOGGER
	str.str("\tDDERR_UNSUPPORTED");
	LogText(str.str());
#endif

	return DDERR_UNSUPPORTED;
}

HRESULT PrimarySurface::SetClipper(
	LPDIRECTDRAWCLIPPER lpDDClipper
	)
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	LogText(str.str());
#endif

#if LOGGER
	str.str("\tDDERR_UNSUPPORTED");
	LogText(str.str());
#endif

	return DDERR_UNSUPPORTED;
}

HRESULT PrimarySurface::SetColorKey(
	DWORD dwFlags,
	LPDDCOLORKEY lpDDColorKey
	)
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	LogText(str.str());
#endif

#if LOGGER
	str.str("\tDDERR_UNSUPPORTED");
	LogText(str.str());
#endif

	return DDERR_UNSUPPORTED;
}

HRESULT PrimarySurface::SetOverlayPosition(
	LONG lX,
	LONG lY
	)
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	LogText(str.str());
#endif

#if LOGGER
	str.str("\tDDERR_UNSUPPORTED");
	LogText(str.str());
#endif

	return DDERR_UNSUPPORTED;
}

HRESULT PrimarySurface::SetPalette(
	LPDIRECTDRAWPALETTE lpDDPalette
	)
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	LogText(str.str());
#endif

#if LOGGER
	str.str("\tDDERR_UNSUPPORTED");
	LogText(str.str());
#endif

	return DDERR_UNSUPPORTED;
}

HRESULT PrimarySurface::Unlock(
	LPVOID lpSurfaceData
	)
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	LogText(str.str());
#endif

#if LOGGER
	str.str("\tDDERR_UNSUPPORTED");
	LogText(str.str());
#endif

	return DDERR_UNSUPPORTED;
}

HRESULT PrimarySurface::UpdateOverlay(
	LPRECT lpSrcRect,
	LPDIRECTDRAWSURFACE lpDDDestSurface,
	LPRECT lpDestRect,
	DWORD dwFlags,
	LPDDOVERLAYFX lpDDOverlayFx
	)
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	LogText(str.str());
#endif

#if LOGGER
	str.str("\tDDERR_UNSUPPORTED");
	LogText(str.str());
#endif

	return DDERR_UNSUPPORTED;
}

HRESULT PrimarySurface::UpdateOverlayDisplay(
	DWORD dwFlags
	)
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	LogText(str.str());
#endif

#if LOGGER
	str.str("\tDDERR_UNSUPPORTED");
	LogText(str.str());
#endif

	return DDERR_UNSUPPORTED;
}

HRESULT PrimarySurface::UpdateOverlayZOrder(
	DWORD dwFlags,
	LPDIRECTDRAWSURFACE lpDDSReference
	)
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	LogText(str.str());
#endif

#if LOGGER
	str.str("\tDDERR_UNSUPPORTED");
	LogText(str.str());
#endif

	return DDERR_UNSUPPORTED;
}
