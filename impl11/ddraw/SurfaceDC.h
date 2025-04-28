#pragma once

struct SurfaceDC
{
	char* buffer32;
	int width;
	int height;
	int displayWidth;
	int displayHeight;
	bool aspectRatioPreserved;
	void (**callback)();
	ID2D1RenderTarget* d2d1RenderTarget;
	ID3D11RenderTargetView* d3d11RenderTargetView;
	ID3D11Device* d3d11Device;
	ID3D11DeviceContext* d3d11DeviceContext;
};
