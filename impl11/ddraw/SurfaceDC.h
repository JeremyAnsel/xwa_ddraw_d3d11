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
};
