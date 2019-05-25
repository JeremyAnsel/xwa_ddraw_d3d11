// Copyright (c) 2014 Jérémy Ansel
// Licensed under the MIT license. See LICENSE.txt
// Extended for VR by Leo Reyes (c) 2019

#pragma once

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
} MainShadersCBuffer;

typedef struct BarrelPixelShaderCBStruct {
	float k1, k2, k3;
} BarrelPixelShaderCBuffer;

/* 3D Constant Bufffers */
typedef struct VertexShaderCBStruct {
	float viewportScale[4];
	float aspect_ratio, parallax, z_override;
} VertexShaderCBuffer;

typedef struct PixelShaderCBStruct {
	float brightness; // Used to control the brightness of some elements -- mostly for ReShade compatibility
} PixelShaderCBuffer;

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
	void InitVSConstantBuffer2D(ID3D11Buffer** buffer, const float parallax, const float aspectRatio, const float scale, const float brightness);
	void InitPSConstantBuffer2D(ID3D11Buffer** buffer, const float parallax, const float aspectRatio, const float scale, const float brightness);
	void InitPSConstantBufferBarrel(ID3D11Buffer** buffer, const float k1, const float k2, const float k3);
	void InitPSConstantBufferBrightness(ID3D11Buffer** buffer, const PixelShaderCBuffer *psConstants);

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
	ComPtr<ID3D11DeviceContext> _d3dDeviceContext;
	ComPtr<IDXGISwapChain> _swapChain;
	ComPtr<ID3D11Texture2D> _backBuffer;
	ComPtr<ID3D11Texture2D> _offscreenBuffer;
	ComPtr<ID3D11Texture2D> _offscreenBufferR; // When SteamVR is used, _offscreenBuffer becomes the left eye and this one becomes the right eye
	ComPtr<ID3D11Texture2D> _offscreenBufferAsInput;
	ComPtr<ID3D11Texture2D> _offscreenBufferAsInputRight; // When SteamVR is used, these buffers can be used to make copies of the
	ComPtr<ID3D11Texture2D> _offscreenBuffer3; // This is the output of the barrel effect when not using SteamVR
	ComPtr<ID3D11RenderTargetView> _renderTargetView;
	ComPtr<ID3D11RenderTargetView> _renderTargetViewR; // When SteamVR is used, _renderTargetView is the left eye, and this one is the right eye
	ComPtr<ID3D11RenderTargetView> _renderTargetView3; // Used for the barrel effect when SteamVR is not used.
	ComPtr<ID3D11ShaderResourceView> _offscreenAsInputShaderResourceView;
	ComPtr<ID3D11ShaderResourceView> _offscreenAsInputShaderResourceViewR; // When SteamVR is enabled, this is the SRV for the right eye
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
	ComPtr<ID3D11RasterizerState> _mainRasterizerState;
	ComPtr<ID3D11SamplerState> _mainSamplerState;
	ComPtr<ID3D11BlendState> _mainBlendState;
	ComPtr<ID3D11DepthStencilState> _mainDepthState;
	ComPtr<ID3D11Buffer> _mainVertexBuffer;
	ComPtr<ID3D11Buffer> _mainIndexBuffer;
	ComPtr<ID3D11Texture2D> _mainDisplayTexture;
	ComPtr<ID3D11ShaderResourceView> _mainDisplayTextureView;
	ComPtr<ID3D11Texture2D> _mainDisplayTextureTemp;
	ComPtr<ID3D11ShaderResourceView> _mainDisplayTextureViewTemp;

	ComPtr<ID3D11VertexShader> _vertexShader;
	ComPtr<ID3D11VertexShader> _sbsVertexShader;
	ComPtr<ID3D11InputLayout> _inputLayout;
	ComPtr<ID3D11PixelShader> _pixelShaderTexture;
	ComPtr<ID3D11PixelShader> _pixelShaderSolid;
	ComPtr<ID3D11RasterizerState> _rasterizerState;
	ComPtr<ID3D11Buffer> _VSConstantBuffer;
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
