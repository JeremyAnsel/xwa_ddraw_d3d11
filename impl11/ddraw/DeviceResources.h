// Copyright (c) 2014 J�r�my Ansel
// Licensed under the MIT license. See LICENSE.txt

#pragma once

enum RenderMainColorKeyType
{
	RENDERMAIN_NO_COLORKEY,
	RENDERMAIN_COLORKEY_20,
	RENDERMAIN_COLORKEY_00,
};

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

class PrimarySurface;
class DepthSurface;
class BackbufferSurface;
class FrontbufferSurface;
class OffscreenSurface;

class DeviceResources
{
public:
	DeviceResources();

	~DeviceResources();

	HRESULT Initialize();

	HRESULT OnSizeChanged(HWND hWnd, DWORD dwWidth, DWORD dwHeight);

	HRESULT LoadMainResources();

	HRESULT LoadResources();

	void InitInputLayout(ID3D11InputLayout* inputLayout);
	void InitVertexShader(ID3D11VertexShader* vertexShader);
	void InitPixelShader(ID3D11PixelShader* pixelShader);
	void InitGeometryShader(ID3D11GeometryShader* geometryShader);
	void InitTopology(D3D_PRIMITIVE_TOPOLOGY topology);
	void InitRasterizerState(ID3D11RasterizerState* state);
	void InitPSShaderResourceView(ID3D11ShaderResourceView* texView, ID3D11ShaderResourceView* texView2 = nullptr);
	HRESULT InitSamplerState(ID3D11SamplerState** sampler, D3D11_SAMPLER_DESC* desc);
	HRESULT InitBlendState(ID3D11BlendState* blend, D3D11_BLEND_DESC* desc);
	HRESULT InitDepthStencilState(ID3D11DepthStencilState* depthState, D3D11_DEPTH_STENCIL_DESC* desc, UINT stencilReference = 0);
	void InitVertexBuffer(ID3D11Buffer** buffer, UINT* stride, UINT* offset);
	void InitIndexBuffer(ID3D11Buffer* buffer, bool isFormat32);
	void InitViewport(D3D11_VIEWPORT* viewport);
	void InitScissorRect(D3D11_RECT* rect);
	void InitConstantBuffer(ID3D11Buffer** buffer, const float* viewportScale);

	HRESULT RenderMain(char* buffer, DWORD width, DWORD height, DWORD bpp, RenderMainColorKeyType useColorKey = RENDERMAIN_COLORKEY_20);

	HRESULT RetrieveBackBuffer(char* buffer, DWORD width, DWORD height, DWORD bpp);
	HRESULT RetrieveTextureBuffer(ID3D11Texture2D* textureBuffer, char* buffer, DWORD width, DWORD height, DWORD bpp);

	UINT GetMaxAnisotropy();

	void CheckMultisamplingSupport();

	bool IsTextureFormatSupported(DXGI_FORMAT format);

	bool BeginAnnotatedEvent(_In_ LPCWSTR Name);
	bool EndAnnotatedEvent();

	bool IsInConcourseHd();

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
	ComPtr<ID3D11Texture2D> _offscreenBufferHdBackground;
	ComPtr<ID3D11Texture2D> _offscreenBufferResolved;
	ComPtr<ID3D11ShaderResourceView> _offscreenBufferResolvedView;
	ComPtr<ID3D11RenderTargetView> _renderTargetView;
	ComPtr<ID3D11RenderTargetView> _backBufferRenderTargetView;
	ComPtr<ID3D11Texture2D> _depthStencil;
	ComPtr<ID3D11DepthStencilView> _depthStencilView;
	ComPtr<ID3DUserDefinedAnnotation> _d3dAnnotation;

	ComPtr<ID2D1Factory> _d2d1Factory;
	ComPtr<IDWriteFactory> _dwriteFactory;
	ComPtr<ID2D1RenderTarget> _d2d1RenderTarget;
	ComPtr<ID2D1DrawingStateBlock> _d2d1DrawingStateBlock;

	ComPtr<ID3D11VertexShader> _mainVertexShader;
	ComPtr<ID3D11InputLayout> _mainInputLayout;
	ComPtr<ID3D11PixelShader> _mainPixelShader;
	ComPtr<ID3D11PixelShader> _mainPixelShaderBpp2ColorKey20;
	ComPtr<ID3D11PixelShader> _mainPixelShaderBpp2ColorKey00;
	ComPtr<ID3D11PixelShader> _mainPixelShaderBpp4ColorKey20;
	ComPtr<ID3D11PixelShader> _pixelShaderResize;
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
	ComPtr<ID3D11InputLayout> _inputLayout;
	ComPtr<ID3D11PixelShader> _pixelShaderTexture;
	ComPtr<ID3D11PixelShader> _pixelShaderSolid;
	ComPtr<ID3D11RasterizerState> _rasterizerState;
	ComPtr<ID3D11Buffer> _constantBuffer;

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

	void (*_surfaceDcCallback)();
};
