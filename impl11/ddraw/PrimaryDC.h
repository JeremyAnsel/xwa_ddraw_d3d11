#pragma once

struct PrimaryDC
{
	int width;
	int height;
	int displayWidth;
	int displayHeight;
	bool aspectRatioPreserved;
	ID2D1Factory* d2d1Factory;
	ID2D1RenderTarget* d2d1RenderTarget;
	ID2D1DrawingStateBlock* d2d1DrawingStateBlock;
	IDWriteFactory* dwriteFactory;
	ID3D11RenderTargetView* d3d11RenderTargetView;
	ID3D11Device* d3d11Device;
	ID3D11DeviceContext* d3d11DeviceContext;
	IDXGISwapChain* dxgiSwapChain;
};
