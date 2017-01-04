// Copyright (c) 2014 J�r�my Ansel
// Licensed under the MIT license. See LICENSE.txt

#include "common.h"
#include <emmintrin.h>
#include "DeviceResources.h"
#include "DirectDrawPalette.h"
#include "PrimarySurface.h"

#ifdef _DEBUG
#include "../Debug/LanczosScalePixelShader.h"
#include "../Debug/MainVertexShader.h"
#include "../Debug/MainPixelShader.h"
#include "../Debug/SmoothScalePixelShader.h"
#include "../Debug/VertexShader.h"
#include "../Debug/PixelShaderAtestTexture.h"
#include "../Debug/PixelShaderAtestTextureNoAlpha.h"
#include "../Debug/PixelShaderTexture.h"
#include "../Debug/PixelShaderSolid.h"
#else
#include "../Release/LanczosScalePixelShader.h"
#include "../Release/MainVertexShader.h"
#include "../Release/MainPixelShader.h"
#include "../Release/SmoothScalePixelShader.h"
#include "../Release/VertexShader.h"
#include "../Release/PixelShaderAtestTexture.h"
#include "../Release/PixelShaderAtestTextureNoAlpha.h"
#include "../Release/PixelShaderTexture.h"
#include "../Release/PixelShaderSolid.h"
#endif

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

DeviceResources::DeviceResources()
{
	this->_displayWidth = 0;
	this->_displayHeight = 0;
	this->_displayBpp = 0;

	this->_d3dDriverType = D3D_DRIVER_TYPE_UNKNOWN;
	this->_d3dFeatureLevel = D3D_FEATURE_LEVEL_9_1;
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

	const float color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	memcpy(this->clearColor, &color, sizeof(color));
	this->clearColorSet = true;

	this->clearDepth = 1.0f;
	this->clearDepthSet = true;

	this->sceneRendered = false;
	this->sceneRenderedEmpty = false;
	this->inScene = false;
	this->inSceneBackbufferLocked = false;
}

DeviceResources::~DeviceResources()
{
	// Releasing a swap chain is only allowed after switching
	// to windowed mode.
	if (this->_swapChain)
	{
		this->_swapChain->SetFullscreenState(FALSE, NULL);
	}
}

HRESULT DeviceResources::Initialize()
{
	HRESULT hr;

	D3D_FEATURE_LEVEL featureLevels[] =
	{
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
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	this->_d3dDriverType = D3D_DRIVER_TYPE_HARDWARE;

	hr = D3D11CreateDevice(nullptr, this->_d3dDriverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels, D3D11_SDK_VERSION, &this->_d3dDevice, &this->_d3dFeatureLevel, &this->_d3dDeviceContext);

	if (FAILED(hr))
	{
		this->_d3dDriverType = D3D_DRIVER_TYPE_WARP;

		hr = D3D11CreateDevice(nullptr, this->_d3dDriverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels, D3D11_SDK_VERSION, &this->_d3dDevice, &this->_d3dFeatureLevel, &this->_d3dDeviceContext);
	}

	if (SUCCEEDED(hr))
	{
		this->CheckMultisamplingSupport();
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

HRESULT DeviceResources::OnSizeChanged(HWND hWnd, DWORD dwWidth, DWORD dwHeight)
{
	HRESULT hr;
	const char* step = "";

	this->_depthStencilView.Release();
	this->_depthStencil.Release();
	this->_renderTargetView.Release();
	this->_offscreenBuffer.Release();
	this->_backBuffer.Release();
	// Releasing a swap chain is only allowed after switching
	// to windowed mode.
	if (this->_swapChain)
	{
		this->_swapChain->SetFullscreenState(FALSE, NULL);
	}
	this->_swapChain.Release();
	this->_swapChain = nullptr;

	this->_refreshRate = { 0, 1 };

	if (hWnd != nullptr)
	{
		step = "RenderTarget SwapChain";
		ComPtr<IDXGIDevice> dxgiDevice;
		ComPtr<IDXGIAdapter> dxgiAdapter;

		hr = this->_d3dDevice.As(&dxgiDevice);

		if (SUCCEEDED(hr))
		{
			hr = dxgiDevice->GetAdapter(&dxgiAdapter);

			if (SUCCEEDED(hr))
			{
				hr = dxgiAdapter->EnumOutputs(0, &_output);
			}
		}

		DXGI_MODE_DESC md{};

		if (SUCCEEDED(hr))
		{
			md.Format = DXGI_FORMAT_B8G8R8A8_UNORM;

			hr = _output->FindClosestMatchingMode(&md, &md, nullptr);
		} else {
			_output = NULL;
		}

		if (SUCCEEDED(hr))
		{
			DXGI_SWAP_CHAIN_DESC sd{};
			sd.BufferCount = 2;
			sd.SwapEffect = DXGI_SWAP_EFFECT_SEQUENTIAL;
			sd.BufferDesc.Width = g_config.Width;
			sd.BufferDesc.Height = g_config.Height;
			sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
			sd.BufferDesc.RefreshRate = md.RefreshRate;
			sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			sd.OutputWindow = hWnd;
			sd.SampleDesc.Count = 1;
			sd.SampleDesc.Quality = 0;
			sd.Windowed = TRUE;

			ComPtr<IDXGIFactory> dxgiFactory;
			hr = dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory));

			if (SUCCEEDED(hr))
			{
				hr = dxgiFactory->CreateSwapChain(this->_d3dDevice, &sd, &this->_swapChain);
			}

			if (SUCCEEDED(hr))
			{
				this->_refreshRate = sd.BufferDesc.RefreshRate;
				if (g_config.Fullscreen == 1)
				{
					// A separate SetFullscreenState is recommended
					// as setting Windowed to FALSE during creation
					// just triggers bugs all over
					this->_swapChain->SetFullscreenState(TRUE, NULL);
				}
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
	}

	if (SUCCEEDED(hr))
	{
		step = "OffscreenBuffer";

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

		hr = this->_d3dDevice->CreateTexture2D(&desc, nullptr, &this->_offscreenBuffer);
	}

	if (SUCCEEDED(hr))
	{
		step = "RenderTargetView";
		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
		renderTargetViewDesc.Format = DXGI_FORMAT_UNKNOWN;
		renderTargetViewDesc.Texture2D.MipSlice = 0;
		renderTargetViewDesc.ViewDimension = this->_useMultisampling ? D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D;
		hr = this->_d3dDevice->CreateRenderTargetView(this->_offscreenBuffer, &renderTargetViewDesc, &this->_renderTargetView);
	}

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

		hr = this->_d3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, &this->_depthStencil);

		if (SUCCEEDED(hr))
		{
			step = "DepthStencilView";
			D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
			depthStencilViewDesc.Format = DXGI_FORMAT_UNKNOWN;
			depthStencilViewDesc.Flags = 0;
			depthStencilViewDesc.Texture2D.MipSlice = 0;
			depthStencilViewDesc.ViewDimension = this->_useMultisampling ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
			hr = this->_d3dDevice->CreateDepthStencilView(this->_depthStencil, &depthStencilViewDesc, &this->_depthStencilView);
		}
	}

	if (SUCCEEDED(hr))
	{
		step = "Viewport";
		this->_d3dDeviceContext->OMSetRenderTargets(1, this->_renderTargetView.GetAddressOf(), this->_depthStencilView.Get());

		D3D11_VIEWPORT viewport;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = (float)this->_backbufferWidth;
		viewport.Height = (float)this->_backbufferHeight;
		viewport.MinDepth = D3D11_MIN_DEPTH;
		viewport.MaxDepth = D3D11_MAX_DEPTH;

		this->_d3dDeviceContext->RSSetViewports(1, &viewport);
	}

	if (SUCCEEDED(hr))
	{
		step = "Texture2D";
		D3D11_TEXTURE2D_DESC textureDesc;
		textureDesc.Width = this->_displayWidth;
		textureDesc.Height = this->_displayHeight;
		textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
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
			textureViewDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
			textureViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			textureViewDesc.Texture2D.MipLevels = 1;
			textureViewDesc.Texture2D.MostDetailedMip = 0;

			hr = this->_d3dDevice->CreateShaderResourceView(this->_mainDisplayTexture, &textureViewDesc, &this->_mainDisplayTextureView);
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

	if (g_config.ScalingType == 2)
	{
		if (FAILED(hr = this->_d3dDevice->CreatePixelShader(g_LanczosScalePixelShader, sizeof(g_LanczosScalePixelShader), nullptr, &_mainPixelShader)))
			return hr;
	}
	else if (g_config.ScalingType && g_config.ScalingType != 3)
	{
		if (FAILED(hr = this->_d3dDevice->CreatePixelShader(g_SmoothScalePixelShader, sizeof(g_SmoothScalePixelShader), nullptr, &_mainPixelShader)))
			return hr;
	}
	else
	{
		if (FAILED(hr = this->_d3dDevice->CreatePixelShader(g_MainPixelShader, sizeof(g_MainPixelShader), nullptr, &_mainPixelShader)))
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
	// Note: anisotropy makes no sense for this full-screen, non-mip-mapped texture
	samplerDesc.Filter = g_config.ScalingType == 3 ? D3D11_FILTER_MIN_MAG_MIP_POINT : D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.MaxAnisotropy = 1;
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

	if (1 || g_config.ScalingType)
	{
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_SRC_ALPHA;
	}

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
		MainVertex(1, -1, 1, 1),
		MainVertex(1, 1, 1, 0),
		MainVertex(-1, 1, 0, 0),
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

	const D3D11_INPUT_ELEMENT_DESC vertexLayoutDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 1, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	if (FAILED(hr = this->_d3dDevice->CreateInputLayout(vertexLayoutDesc, ARRAYSIZE(vertexLayoutDesc), g_VertexShader, sizeof(g_VertexShader), &_inputLayout)))
		return hr;

	if (FAILED(hr = this->_d3dDevice->CreatePixelShader(g_PixelShaderTexture, sizeof(g_PixelShaderTexture), nullptr, &_pixelShaderTexture)))
		return hr;

	if (FAILED(hr = this->_d3dDevice->CreatePixelShader(g_PixelShaderAtestTexture, sizeof(g_PixelShaderAtestTexture), nullptr, &_pixelShaderAtestTexture)))
		return hr;

	if (FAILED(hr = this->_d3dDevice->CreatePixelShader(g_PixelShaderAtestTextureNoAlpha, sizeof(g_PixelShaderAtestTextureNoAlpha), nullptr, &_pixelShaderAtestTextureNoAlpha)))
		return hr;

	if (FAILED(hr = this->_d3dDevice->CreatePixelShader(g_PixelShaderSolid, sizeof(g_PixelShaderSolid), nullptr, &_pixelShaderSolid)))
		return hr;

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
	constantBufferDesc.ByteWidth = 16;
	constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.CPUAccessFlags = 0;
	constantBufferDesc.MiscFlags = 0;
	constantBufferDesc.StructureByteStride = 0;

	if (FAILED(hr = this->_d3dDevice->CreateBuffer(&constantBufferDesc, nullptr, &this->_constantBuffer)))
		return hr;

	return hr;
}

HRESULT DeviceResources::RenderMain(char* src, DWORD width, DWORD height, DWORD bpp, bool useColorKey)
{
	bool useMainDisplayTexture = (width == this->_displayWidth) && (height == this->_displayHeight);

	HRESULT hr = S_OK;
	const char* step = "";

	D3D11_MAPPED_SUBRESOURCE displayMap;
	char* tempBuffer = nullptr;
	ComPtr<ID3D11Texture2D> tempTexture;
	ComPtr<ID3D11ShaderResourceView> tempTextureView;
	DWORD pitchDelta;

	void* buffer = nullptr;

	if (SUCCEEDED(hr))
	{
		if (useMainDisplayTexture)
		{
			step = "DisplayTexture";
			hr = this->_d3dDeviceContext->Map(this->_mainDisplayTexture, 0, D3D11_MAP_WRITE_DISCARD, 0, &displayMap);

			if (SUCCEEDED(hr))
			{
				buffer = displayMap.pData;
				pitchDelta = displayMap.RowPitch - width * 4;
			}
		}
		else
		{
			step = "tempTexture";
			tempBuffer = new char[width * height * 4];
			buffer = tempBuffer;
			pitchDelta = 0;
		}
	}

	if (SUCCEEDED(hr))
	{
		if (bpp == 1)
		{
			const char* srcColors = src;
			unsigned int* colors = (unsigned int*)buffer;
			const unsigned *palette = (_primarySurface && _primarySurface->palette) ? _primarySurface->palette->palette : nullptr;

			if (palette)
			{
				for (unsigned y = 0; y < height; y++)
				{
					for (unsigned x = 0; x < width; x++)
					{
						unsigned char color8 = *srcColors++;
						*colors++ = palette[color8];
					}
					colors = (unsigned int*)((char*)colors + pitchDelta);
				}
			}
			else {
				for (unsigned y = 0; y < height; y++)
				{
					for (unsigned x = 0; x < width; x++)
					{
						unsigned char color8 = *srcColors++;
						*colors++ = (color8 << 16) | (color8 << 8) | color8;
					}
					colors = (unsigned int*)((char*)colors + pitchDelta);
				}
			}

		}
		else if(bpp == 2)
		{
			if (useColorKey)
			{
				unsigned short* srcColors = (unsigned short*)src;
				unsigned int* colors = (unsigned int*)buffer;

				for (unsigned y = 0; y < height; y++)
				{
					unsigned x;
					// The loop condition shouldn't need to be so obfuscated,
					// but MSVC is stupid and generates slow loop conditions otherwise
					for (x = 8; x <= width; x += 8)
					{
						__m128i red = _mm_loadu_si128((const __m128i *)(srcColors + x - 8));
						__m128i transparent = _mm_cmpeq_epi16(red, _mm_set1_epi16(0x2000));
						__m128i blue = _mm_and_si128(red, _mm_set1_epi16(0x1f));
						blue = _mm_or_si128(blue, _mm_slli_epi16(blue, 5));
						blue = _mm_srli_epi16(blue, 2);
						__m128i green = _mm_srli_epi16(red, 5);
						green = _mm_and_si128(green, _mm_set1_epi16(0x3f));
						green = _mm_or_si128(green, _mm_slli_epi16(green, 6));
						green = _mm_srli_epi16(green, 4);
						green = _mm_slli_epi16(green, 8);
						green = _mm_or_si128(green, blue);
						green = _mm_andnot_si128(transparent, green);
						red = _mm_srli_epi16(red, 11);
						red = _mm_or_si128(red, _mm_slli_epi16(red, 5));
						red = _mm_srli_epi16(red, 2);
						red = _mm_andnot_si128(transparent, red);
						transparent = _mm_slli_epi16(transparent, 8);
						red = _mm_or_si128(red, transparent);
						_mm_storeu_si128((__m128i *)(colors + x - 8), _mm_unpacklo_epi16(green, red));
						_mm_storeu_si128((__m128i *)(colors + x - 4), _mm_unpackhi_epi16(green, red));
					}
					x -= 8;
					for (; x < width; x++) {
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
					colors += width;
					colors = (unsigned int*)((char*)colors + pitchDelta);
				}
			}
			else
			{
				unsigned short* srcColors = (unsigned short*)src;
				unsigned int* colors = (unsigned int*)buffer;

				for (unsigned y = 0; y < height; y++)
				{
					for (unsigned x = 0; x < width; x++)
					{
						unsigned short color16 = *srcColors;
						srcColors++;

						*colors = convertColorB5G6R5toB8G8R8X8(color16);
						colors++;
					}

					colors = (unsigned int*)((char*)colors + pitchDelta);
				}
			}
		}
		else
		{
			if (useColorKey)
			{
				unsigned int* srcColors = (unsigned int*)src;
				unsigned int* colors = (unsigned int*)buffer;

				for (unsigned y = 0; y < height; y++)
				{
					for (unsigned x = 0; x < width; x++)
					{
						unsigned int color32 = *srcColors;
						srcColors++;

						if (color32 == 0x200000)
						{
							*colors = 0xff000000;
						}
						else
						{
							*colors = color32 & 0xffffff;
						}

						colors++;
					}

					colors = (unsigned int*)((char*)colors + pitchDelta);
				}
			}
			else
			{
				if (pitchDelta == 0)
				{
					memcpy(buffer, src, width * height * 4);
				}
				else
				{
					unsigned int* srcColors = (unsigned int*)src;
					unsigned int* colors = (unsigned int*)buffer;

					for (unsigned y = 0; y < height; y++)
					{
						memcpy(colors, srcColors, width * 4);
						srcColors += width;
						colors += width;

						colors = (unsigned int*)((char*)colors + pitchDelta);
					}
				}
			}
		}
	}

	if (SUCCEEDED(hr))
	{
		if (useMainDisplayTexture)
		{
			this->_d3dDeviceContext->Unmap(this->_mainDisplayTexture, 0);
		}
		else
		{
			D3D11_TEXTURE2D_DESC textureDesc;
			textureDesc.Width = width;
			textureDesc.Height = height;
			textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
			textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
			textureDesc.CPUAccessFlags = 0;
			textureDesc.MiscFlags = 0;
			textureDesc.MipLevels = 1;
			textureDesc.ArraySize = 1;
			textureDesc.SampleDesc.Count = 1;
			textureDesc.SampleDesc.Quality = 0;
			textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

			D3D11_SUBRESOURCE_DATA textureData;
			textureData.pSysMem = tempBuffer;
			textureData.SysMemPitch = width * 4;
			textureData.SysMemSlicePitch = 0;

			hr = this->_d3dDevice->CreateTexture2D(&textureDesc, &textureData, &tempTexture);

			delete[] tempBuffer;

			if (SUCCEEDED(hr))
			{
				D3D11_SHADER_RESOURCE_VIEW_DESC textureViewDesc{};
				textureViewDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
				textureViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				textureViewDesc.Texture2D.MipLevels = 1;
				textureViewDesc.Texture2D.MostDetailedMip = 0;

				hr = this->_d3dDevice->CreateShaderResourceView(tempTexture, &textureViewDesc, &tempTextureView);
			}
		}
	}

	this->_d3dDeviceContext->IASetInputLayout(this->_mainInputLayout);
	this->_d3dDeviceContext->VSSetShader(this->_mainVertexShader, nullptr, 0);
	this->_d3dDeviceContext->PSSetShader(this->_mainPixelShader, nullptr, 0);
	this->_d3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

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
	}
	else
	{
		w = this->_backbufferWidth;
		h = this->_backbufferHeight;
	}

	UINT left = (this->_backbufferWidth - w) / 2;
	UINT top = (this->_backbufferHeight - h) / 2;

	D3D11_VIEWPORT viewport;
	viewport.TopLeftX = (float)left;
	viewport.TopLeftY = (float)top;
	viewport.Width = (float)w;
	viewport.Height = (float)h;
	viewport.MinDepth = D3D11_MIN_DEPTH;
	viewport.MaxDepth = D3D11_MAX_DEPTH;

	this->_d3dDeviceContext->RSSetViewports(1, &viewport);

	if (SUCCEEDED(hr))
	{
		step = "States";

		this->_d3dDeviceContext->RSSetState(this->_mainRasterizerState);

		this->_d3dDeviceContext->PSSetSamplers(0, 1, this->_mainSamplerState.GetAddressOf());

		if (g_config.ScalingType && g_config.ScalingType != 3)
		{
			float texsize[4] = { static_cast<float>(width), static_cast<float>(height) };
			D3D11_BUFFER_DESC cbDesc;
			cbDesc.ByteWidth = 16; // We only use 8, but 16 is the minimum
			cbDesc.Usage = D3D11_USAGE_IMMUTABLE;
			cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			cbDesc.CPUAccessFlags = 0;
			cbDesc.MiscFlags = 0;
			cbDesc.StructureByteStride = 0;
			D3D11_SUBRESOURCE_DATA InitData;
			InitData.pSysMem = texsize;
			InitData.SysMemPitch = 0;
			InitData.SysMemSlicePitch = 0;

			ID3D11Buffer *texsizeBuf = NULL;
			hr = this->_d3dDevice->CreateBuffer(&cbDesc, &InitData, &texsizeBuf);

			this->_d3dDeviceContext->PSSetConstantBuffers(0, 1, &texsizeBuf);
		}

		const FLOAT factors[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		UINT mask = 0xffffffff;
		this->_d3dDeviceContext->OMSetBlendState(this->_mainBlendState, factors, mask);

		this->_d3dDeviceContext->OMSetDepthStencilState(this->_mainDepthState, 0);
	}

	if (SUCCEEDED(hr))
	{
		step = "Texture2D ShaderResourceView";

		if (useMainDisplayTexture)
		{
			this->_d3dDeviceContext->PSSetShaderResources(0, 1, this->_mainDisplayTextureView.GetAddressOf());
		}
		else
		{
			this->_d3dDeviceContext->PSSetShaderResources(0, 1, tempTextureView.GetAddressOf());
		}
	}

	if (SUCCEEDED(hr))
	{
		step = "Draw";

		UINT stride = sizeof(MainVertex);
		UINT offset = 0;

		this->_d3dDeviceContext->IASetVertexBuffers(0, 1, this->_mainVertexBuffer.GetAddressOf(), &stride, &offset);
		this->_d3dDeviceContext->IASetIndexBuffer(this->_mainIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
		this->_d3dDeviceContext->DrawIndexed(6, 0, 0);
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
	const char* step = "";

	memset(buffer, 0, width * height * bpp);

	D3D11_TEXTURE2D_DESC textureDescription;
	this->_backBuffer->GetDesc(&textureDescription);

	textureDescription.BindFlags = 0;
	textureDescription.SampleDesc.Count = 1;
	textureDescription.SampleDesc.Quality = 0;

		step = "Staging Texture2D";

		ComPtr<ID3D11Texture2D> texture;
		textureDescription.Usage = D3D11_USAGE_STAGING;
		textureDescription.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;

		if (SUCCEEDED(hr = this->_d3dDevice->CreateTexture2D(&textureDescription, nullptr, &texture)))
		{
			this->_d3dDeviceContext->CopyResource(texture, this->_backBuffer);

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

						for (unsigned y = 0; y < this->_backbufferHeight; y++)
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

void DeviceResources::DefaultSurfaceDesc(LPDDSURFACEDESC lpDDSurfaceDesc, DWORD caps)
{
	unsigned bpp = this->_displayBpp;
	*lpDDSurfaceDesc = {};
	lpDDSurfaceDesc->dwSize = sizeof(DDSURFACEDESC);
	lpDDSurfaceDesc->dwFlags = DDSD_CAPS | DDSD_PIXELFORMAT | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PITCH;
	lpDDSurfaceDesc->ddsCaps.dwCaps = caps | (bpp == 1 ? DDSCAPS_PALETTE : 0);
	lpDDSurfaceDesc->ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
	lpDDSurfaceDesc->ddpfPixelFormat.dwFlags = bpp == 1 ? DDPF_PALETTEINDEXED8 : DDPF_RGB;
	lpDDSurfaceDesc->ddpfPixelFormat.dwRGBBitCount = 16;
	lpDDSurfaceDesc->ddpfPixelFormat.dwRBitMask = 0xF800;
	lpDDSurfaceDesc->ddpfPixelFormat.dwGBitMask = 0x7E0;
	lpDDSurfaceDesc->ddpfPixelFormat.dwBBitMask = 0x1F;
	lpDDSurfaceDesc->dwHeight = this->_displayHeight;
	lpDDSurfaceDesc->dwWidth = this->_displayWidth;
	lpDDSurfaceDesc->lPitch = this->_displayWidth * (bpp == 1 ? 1 : 2);
}
