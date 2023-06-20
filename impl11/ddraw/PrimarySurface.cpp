// Copyright (c) 2014 Jérémy Ansel
// Licensed under the MIT license. See LICENSE.txt

#include "common.h"
#include "DeviceResources.h"
#include "PrimarySurface.h"
#include "BackbufferSurface.h"
#include "FrontbufferSurface.h"
#include "XwaDrawTextHook.h"
#include "XwaDrawRadarHook.h"
#include "XwaDrawBracketHook.h"
#include "XwaConcourseHook.h"

static bool g_PrimarySurfaceInitialized = false;

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

	g_PrimarySurfaceInitialized = true;
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

	g_PrimarySurfaceInitialized = false;
	this->RenderText();
	this->RenderRadar();
	this->RenderBracket();
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

HRESULT PrimarySurface::Flip(
	LPDIRECTDRAWSURFACE lpDDSurfaceTargetOverride,
	DWORD dwFlags
	)
{
	_deviceResources->BeginAnnotatedEvent(L"PrimarySurfaceFlip");

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
		if (!this->_deviceResources->IsInConcourseHd())
		{
			this->_deviceResources->_d3dDeviceContext->ClearRenderTargetView(this->_deviceResources->_renderTargetView, this->_deviceResources->clearColor);
		}

		this->_deviceResources->_d3dDeviceContext->ClearDepthStencilView(this->_deviceResources->_depthStencilView, D3D11_CLEAR_DEPTH, this->_deviceResources->clearDepth, 0);
	}

	if (lpDDSurfaceTargetOverride != nullptr)
	{
		if (lpDDSurfaceTargetOverride == this->_deviceResources->_frontbufferSurface)
		{
			HRESULT hr;

			if (FAILED(hr = this->_deviceResources->_backbufferSurface->BltFast(0, 0, this->_deviceResources->_frontbufferSurface, nullptr, 0)))
				return hr;

			this->_deviceResources->EndAnnotatedEvent();
			return this->Flip(this->_deviceResources->_backbufferSurface, 0);
		}

		if (lpDDSurfaceTargetOverride == this->_deviceResources->_backbufferSurface)
		{
			bool isInConcourseHd = this->_deviceResources->IsInConcourseHd();

			if (!isInConcourseHd && this->_deviceResources->_frontbufferSurface == nullptr)
			{
				if (FAILED(this->_deviceResources->RenderMain(this->_deviceResources->_backbufferSurface->_buffer, this->_deviceResources->_displayWidth, this->_deviceResources->_displayHeight, this->_deviceResources->_displayBpp)))
					return DDERR_GENERIC;
			}
			else if (!isInConcourseHd)
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

			if (this->_deviceResources->_swapChain)
			{
				UINT rate = 24 * this->_deviceResources->_refreshRate.Denominator;
				UINT numerator = this->_deviceResources->_refreshRate.Numerator + this->_flipFrames;

				UINT interval = numerator / rate;
				this->_flipFrames = numerator % rate;

				interval = max(interval, 1);

				hr = DD_OK;

				for (UINT i = 0; i < interval; i++)
				{
					this->_deviceResources->_d3dDeviceContext->ResolveSubresource(this->_deviceResources->_offscreenBufferResolved, 0, this->_deviceResources->_offscreenBuffer, 0, DXGI_FORMAT_B8G8R8A8_UNORM);
					ResizeForPresent();

					if (FAILED(hr = this->_deviceResources->_swapChain->Present(1, 0)))
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

				if (g_config.HDConcourseEnabled)
				{
					ConcourseTakeScreenshot();
				}

				this->_deviceResources->_frontbufferSurface->wasBltFastCalled = false;
			}

			if (g_config.HDConcourseEnabled)
			{
				const int currentGameState = *(int*)(0x009F60E0 + 0x25FA9);
				const int updateCallback = *(int*)(0x009F60E0 + 0x25FB1 + 0x850 * currentGameState + 0x0844);
				const bool isConfigMenuGameStateUpdate = updateCallback == 0x0051D100;

				if (!isConfigMenuGameStateUpdate && this->_deviceResources->IsInConcourseHd())
				{
					this->_deviceResources->_d3dDeviceContext->CopyResource(this->_deviceResources->_offscreenBuffer, this->_deviceResources->_offscreenBufferHdBackground);
				}
			}

			this->_deviceResources->EndAnnotatedEvent();
			return hr;
		}
	}
	else
	{
		HRESULT hr;

		if (this->_deviceResources->_swapChain)
		{
			hr = DD_OK;

			if (g_config.Radar2DRendererEnabled)
			{
				this->RenderBracket();
				this->RenderRadar();
			}

			if (g_config.Text2DRendererEnabled)
			{
				this->RenderText();
			}

			this->_deviceResources->_d3dDeviceContext->ResolveSubresource(this->_deviceResources->_offscreenBufferResolved, 0, this->_deviceResources->_offscreenBuffer, 0, DXGI_FORMAT_B8G8R8A8_UNORM);
			ResizeForPresent();

			if (FAILED(hr = this->_deviceResources->_swapChain->Present(g_config.VSyncEnabled ? 1 : 0, 0)))
			{
				static bool messageShown = false;

				if (!messageShown)
				{
					MessageBox(nullptr, _com_error(hr).ErrorMessage(), __FUNCTION__, MB_ICONERROR);
				}

				messageShown = true;

				hr = DDERR_SURFACELOST;
			}
		}
		else
		{
			hr = DD_OK;
		}
		this->_deviceResources->EndAnnotatedEvent();
		return hr;
	}

#if LOGGER
	str.str("\tDDERR_UNSUPPORTED");
	LogText(str.str());
#endif

	this->_deviceResources->EndAnnotatedEvent();
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

void PrimarySurface::RenderText()
{
	static ID2D1RenderTarget* s_d2d1RenderTarget = nullptr;
	static DWORD s_displayWidth = 0;
	static DWORD s_displayHeight = 0;
	static int s_fontSizes[3] = { 12, 16, 10 };
	static ComPtr<IDWriteTextFormat> s_textFormats[3];
	static ComPtr<IDWriteTextLayout> s_textLayouts[3 * 256];
	static ComPtr<ID2D1SolidColorBrush> s_brush;
	static UINT s_left;
	static UINT s_top;
	static float s_scaleX;
	static float s_scaleY;

	if (!g_PrimarySurfaceInitialized)
	{
		s_d2d1RenderTarget = nullptr;
		s_displayWidth = 0;
		s_displayHeight = 0;

		for (int index = 0; index < 3; index++)
		{
			s_textFormats[index].Release();

			for (int c = 0; c < 256; c++)
			{
				int layoutIndex = index * 256 + c;
				s_textLayouts[layoutIndex].Release();
			}
		}

		s_brush.Release();
		return;
	}

	this->_deviceResources->BeginAnnotatedEvent(L"RenderText");

	if (this->_deviceResources->_d2d1RenderTarget != s_d2d1RenderTarget || this->_deviceResources->_displayWidth != s_displayWidth || this->_deviceResources->_displayHeight != s_displayHeight)
	{
		s_d2d1RenderTarget = this->_deviceResources->_d2d1RenderTarget;
		s_displayWidth = this->_deviceResources->_displayWidth;
		s_displayHeight = this->_deviceResources->_displayHeight;

		//UINT w;
		//UINT h;

		//if (g_config.AspectRatioPreserved)
		//{
		//	if (this->_deviceResources->_backbufferHeight * this->_deviceResources->_displayWidth <= this->_deviceResources->_backbufferWidth * this->_deviceResources->_displayHeight)
		//	{
		//		w = this->_deviceResources->_backbufferHeight * this->_deviceResources->_displayWidth / this->_deviceResources->_displayHeight;
		//		h = this->_deviceResources->_backbufferHeight;
		//	}
		//	else
		//	{
		//		w = this->_deviceResources->_backbufferWidth;
		//		h = this->_deviceResources->_backbufferWidth * this->_deviceResources->_displayHeight / this->_deviceResources->_displayWidth;
		//	}
		//}
		//else
		//{
		//	w = this->_deviceResources->_backbufferWidth;
		//	h = this->_deviceResources->_backbufferHeight;
		//}

		//s_left = (this->_deviceResources->_backbufferWidth - w) / 2;
		//s_top = (this->_deviceResources->_backbufferHeight - h) / 2;

		//s_scaleX = (float)w / (float)this->_deviceResources->_displayWidth;
		//s_scaleY = (float)h / (float)this->_deviceResources->_displayHeight;

		//w = this->_deviceResources->_displayWidth;
		//h = this->_deviceResources->_displayHeight;

		s_left = 0;
		s_top = 0;

		s_scaleX = 1.0f;
		s_scaleY = 1.0f;

		for (int index = 0; index < 3; index++)
		{
			this->_deviceResources->_dwriteFactory->CreateTextFormat(
				g_config.TextFontFamily.c_str(),
				nullptr,
				DWRITE_FONT_WEIGHT_NORMAL,
				DWRITE_FONT_STYLE_NORMAL,
				DWRITE_FONT_STRETCH_NORMAL,
				(float)s_fontSizes[index] * min(s_scaleX, s_scaleY),
				L"en-US",
				&s_textFormats[index]);

			for (int c = 33; c < 256; c++)
			{
				char t[2];
				t[0] = (char)c;
				t[1] = 0;

				std::wstring wtext = string_towstring(t);

				if (wtext.empty())
				{
					continue;
				}

				int layoutIndex = index * 256 + c;
				this->_deviceResources->_dwriteFactory->CreateTextLayout(
					wtext.c_str(),
					wtext.length(),
					s_textFormats[index],
					(FLOAT)this->_deviceResources->_backbufferWidth,
					(FLOAT)this->_deviceResources->_backbufferHeight,
					&s_textLayouts[layoutIndex]);
			}
		}

		this->_deviceResources->_d2d1RenderTarget->CreateSolidColorBrush(D2D1::ColorF(0), &s_brush);
	}

	this->_deviceResources->_d2d1RenderTarget->SaveDrawingState(this->_deviceResources->_d2d1DrawingStateBlock);
	this->_deviceResources->_d2d1RenderTarget->BeginDraw();

	unsigned int brushColor = 0;
	s_brush->SetColor(D2D1::ColorF(brushColor));

	for (const auto& xwaText : g_xwa_text)
	{
		if (xwaText.textChar == ' ')
		{
			continue;
		}

		IDWriteTextLayout* textLayout = nullptr;

		for (int index = 0; index < 3; index++)
		{
			if (xwaText.fontSize == s_fontSizes[index])
			{
				int layoutIndex = index * 256 + (int)(unsigned char)xwaText.textChar;
				textLayout = s_textLayouts[layoutIndex];
				break;
			}
		}

		if (!textLayout)
		{
			continue;
		}

		if (xwaText.color != brushColor)
		{
			brushColor = xwaText.color;
			s_brush->SetColor(D2D1::ColorF(brushColor));
		}

		float x = (float)s_left + (float)xwaText.positionX * s_scaleX;
		float y = (float)s_top + (float)xwaText.positionY * s_scaleY;

		this->_deviceResources->_d2d1RenderTarget->DrawTextLayout(
			D2D1_POINT_2F{ x, y },
			textLayout,
			s_brush);
	}

	this->_deviceResources->_d2d1RenderTarget->EndDraw();
	this->_deviceResources->_d2d1RenderTarget->RestoreDrawingState(this->_deviceResources->_d2d1DrawingStateBlock);

	g_xwa_text.clear();
	g_xwa_text.reserve(4096);

	this->_deviceResources->EndAnnotatedEvent();
}

void PrimarySurface::RenderRadar()
{
	static ID2D1RenderTarget* s_d2d1RenderTarget = nullptr;
	static DWORD s_displayWidth = 0;
	static DWORD s_displayHeight = 0;
	static ComPtr<ID2D1SolidColorBrush> s_brush;
	static UINT s_left;
	static UINT s_top;
	static float s_scaleX;
	static float s_scaleY;

	if (!g_PrimarySurfaceInitialized)
	{
		s_d2d1RenderTarget = nullptr;
		s_displayWidth = 0;
		s_displayHeight = 0;

		s_brush.Release();
		return;
	}

	this->_deviceResources->BeginAnnotatedEvent(L"RenderRadar");

	if (this->_deviceResources->_d2d1RenderTarget != s_d2d1RenderTarget || this->_deviceResources->_displayWidth != s_displayWidth || this->_deviceResources->_displayHeight != s_displayHeight)
	{
		s_d2d1RenderTarget = this->_deviceResources->_d2d1RenderTarget;
		s_displayWidth = this->_deviceResources->_displayWidth;
		s_displayHeight = this->_deviceResources->_displayHeight;

		//UINT w;
		//UINT h;

		//if (g_config.AspectRatioPreserved)
		//{
		//	if (this->_deviceResources->_backbufferHeight * this->_deviceResources->_displayWidth <= this->_deviceResources->_backbufferWidth * this->_deviceResources->_displayHeight)
		//	{
		//		w = this->_deviceResources->_backbufferHeight * this->_deviceResources->_displayWidth / this->_deviceResources->_displayHeight;
		//		h = this->_deviceResources->_backbufferHeight;
		//	}
		//	else
		//	{
		//		w = this->_deviceResources->_backbufferWidth;
		//		h = this->_deviceResources->_backbufferWidth * this->_deviceResources->_displayHeight / this->_deviceResources->_displayWidth;
		//	}
		//}
		//else
		//{
		//	w = this->_deviceResources->_backbufferWidth;
		//	h = this->_deviceResources->_backbufferHeight;
		//}

		//s_left = (this->_deviceResources->_backbufferWidth - w) / 2;
		//s_top = (this->_deviceResources->_backbufferHeight - h) / 2;

		//s_scaleX = (float)w / (float)this->_deviceResources->_displayWidth;
		//s_scaleY = (float)h / (float)this->_deviceResources->_displayHeight;

		//w = this->_deviceResources->_displayWidth;
		//h = this->_deviceResources->_displayHeight;

		s_left = 0;
		s_top = 0;

		s_scaleX = 1.0f;
		s_scaleY = 1.0f;

		this->_deviceResources->_d2d1RenderTarget->CreateSolidColorBrush(D2D1::ColorF(0), &s_brush);
	}

	this->_deviceResources->_d2d1RenderTarget->SaveDrawingState(this->_deviceResources->_d2d1DrawingStateBlock);
	this->_deviceResources->_d2d1RenderTarget->BeginDraw();

	unsigned int brushColor = 0;
	s_brush->SetColor(D2D1::ColorF(brushColor));

	for (const auto& xwaRadar : g_xwa_radar)
	{
		unsigned short si = ((unsigned short*)0x08D9420)[xwaRadar.colorIndex];
		unsigned int esi;

		if (((bool(*)())0x0050DC50)() != 0)
		{
			unsigned short eax = si & 0x001F;
			unsigned short ecx = si & 0x7C00;
			unsigned short edx = si & 0x03E0;

			esi = (eax << 3) | (edx << 6) | (ecx << 9);
		}
		else
		{
			unsigned short eax = si & 0x001F;
			unsigned short edx = si & 0xF800;
			unsigned short ecx = si & 0x07E0;

			esi = (eax << 3) | (ecx << 5) | (edx << 8);
		}

		if (esi != brushColor)
		{
			brushColor = esi;
			s_brush->SetColor(D2D1::ColorF(brushColor));
		}

		float x = s_left + (float)xwaRadar.positionX * s_scaleX;
		float y = s_top + (float)xwaRadar.positionY * s_scaleY;

		float deltaX = 2.0f * s_scaleX;
		float deltaY = 2.0f * s_scaleY;

		this->_deviceResources->_d2d1RenderTarget->FillEllipse(D2D1::Ellipse(D2D1::Point2F(x, y), deltaX, deltaY), s_brush);
	}

	if (g_xwa_radar_selected_positionX != -1 && g_xwa_radar_selected_positionY != -1)
	{
		s_brush->SetColor(D2D1::ColorF(D2D1::ColorF::Yellow));

		float x = s_left + (float)g_xwa_radar_selected_positionX * s_scaleX;
		float y = s_top + (float)g_xwa_radar_selected_positionY * s_scaleY;

		float deltaX = 4.0f * s_scaleX;
		float deltaY = 4.0f * s_scaleY;

		this->_deviceResources->_d2d1RenderTarget->FillEllipse(D2D1::Ellipse(D2D1::Point2F(x, y), deltaX, deltaY), s_brush);
	}

	this->_deviceResources->_d2d1RenderTarget->EndDraw();
	this->_deviceResources->_d2d1RenderTarget->RestoreDrawingState(this->_deviceResources->_d2d1DrawingStateBlock);

	g_xwa_radar.clear();
	g_xwa_radar_selected_positionX = -1;
	g_xwa_radar_selected_positionY = -1;

	this->_deviceResources->EndAnnotatedEvent();
}

void PrimarySurface::RenderBracket()
{
	static ID2D1RenderTarget* s_d2d1RenderTarget = nullptr;
	static DWORD s_displayWidth = 0;
	static DWORD s_displayHeight = 0;
	static ComPtr<ID2D1SolidColorBrush> s_brush;
	static UINT s_left;
	static UINT s_top;
	static float s_scaleX;
	static float s_scaleY;

	if (!g_PrimarySurfaceInitialized)
	{
		s_d2d1RenderTarget = nullptr;
		s_displayWidth = 0;
		s_displayHeight = 0;

		s_brush.Release();
		return;
	}

	this->_deviceResources->BeginAnnotatedEvent(L"RenderBracket");

	if (this->_deviceResources->_d2d1RenderTarget != s_d2d1RenderTarget || this->_deviceResources->_displayWidth != s_displayWidth || this->_deviceResources->_displayHeight != s_displayHeight)
	{
		s_d2d1RenderTarget = this->_deviceResources->_d2d1RenderTarget;
		s_displayWidth = this->_deviceResources->_displayWidth;
		s_displayHeight = this->_deviceResources->_displayHeight;

		//UINT w;
		//UINT h;

		//if (g_config.AspectRatioPreserved)
		//{
		//	if (this->_deviceResources->_backbufferHeight * this->_deviceResources->_displayWidth <= this->_deviceResources->_backbufferWidth * this->_deviceResources->_displayHeight)
		//	{
		//		w = this->_deviceResources->_backbufferHeight * this->_deviceResources->_displayWidth / this->_deviceResources->_displayHeight;
		//		h = this->_deviceResources->_backbufferHeight;
		//	}
		//	else
		//	{
		//		w = this->_deviceResources->_backbufferWidth;
		//		h = this->_deviceResources->_backbufferWidth * this->_deviceResources->_displayHeight / this->_deviceResources->_displayWidth;
		//	}
		//}
		//else
		//{
		//	w = this->_deviceResources->_backbufferWidth;
		//	h = this->_deviceResources->_backbufferHeight;
		//}

		//s_left = (this->_deviceResources->_backbufferWidth - w) / 2;
		//s_top = (this->_deviceResources->_backbufferHeight - h) / 2;

		//s_scaleX = (float)w / (float)this->_deviceResources->_displayWidth;
		//s_scaleY = (float)h / (float)this->_deviceResources->_displayHeight;

		//w = this->_deviceResources->_displayWidth;
		//h = this->_deviceResources->_displayHeight;

		s_left = 0;
		s_top = 0;

		s_scaleX = 1.0f;
		s_scaleY = 1.0f;

		this->_deviceResources->_d2d1RenderTarget->CreateSolidColorBrush(D2D1::ColorF(0), &s_brush);
	}

	this->_deviceResources->_d2d1RenderTarget->SaveDrawingState(this->_deviceResources->_d2d1DrawingStateBlock);
	this->_deviceResources->_d2d1RenderTarget->BeginDraw();

	unsigned int brushColor = 0;
	s_brush->SetColor(D2D1::ColorF(brushColor));

	for (const auto& xwaBracket : g_xwa_bracket)
	{
		unsigned short si = ((unsigned short*)0x08D9420)[xwaBracket.colorIndex];
		unsigned int esi;

		if (((bool(*)())0x0050DC50)() != 0)
		{
			unsigned short eax = si & 0x001F;
			unsigned short ecx = si & 0x7C00;
			unsigned short edx = si & 0x03E0;

			esi = (eax << 3) | (edx << 6) | (ecx << 9);
		}
		else
		{
			unsigned short eax = si & 0x001F;
			unsigned short edx = si & 0xF800;
			unsigned short ecx = si & 0x07E0;

			esi = (eax << 3) | (ecx << 5) | (edx << 8);
		}

		if (esi != brushColor)
		{
			brushColor = esi;
			s_brush->SetColor(D2D1::ColorF(brushColor));
		}

		float posX = s_left + (float)xwaBracket.positionX * s_scaleX;
		float posY = s_top + (float)xwaBracket.positionY * s_scaleY;
		float posW = (float)xwaBracket.width * s_scaleX;
		float posH = (float)xwaBracket.height * s_scaleY;
		float posSide = 0.125;

		float strokeWidth = 2.0f * min(s_scaleX, s_scaleY);

		bool fill = xwaBracket.width <= 4 || xwaBracket.height <= 4;

		if (fill)
		{
			this->_deviceResources->_d2d1RenderTarget->FillRectangle(D2D1::RectF(posX, posY, posX + posW, posY + posH), s_brush);
		}
		else
		{
			// top left
			this->_deviceResources->_d2d1RenderTarget->DrawLine(D2D1::Point2F(posX, posY), D2D1::Point2F(posX + posW * posSide, posY), s_brush, strokeWidth);
			this->_deviceResources->_d2d1RenderTarget->DrawLine(D2D1::Point2F(posX, posY), D2D1::Point2F(posX, posY + posH * posSide), s_brush, strokeWidth);

			// top right
			this->_deviceResources->_d2d1RenderTarget->DrawLine(D2D1::Point2F(posX + posW - posW * posSide, posY), D2D1::Point2F(posX + posW, posY), s_brush, strokeWidth);
			this->_deviceResources->_d2d1RenderTarget->DrawLine(D2D1::Point2F(posX + posW, posY), D2D1::Point2F(posX + posW, posY + posH * posSide), s_brush, strokeWidth);

			// bottom left
			this->_deviceResources->_d2d1RenderTarget->DrawLine(D2D1::Point2F(posX, posY + posH - posH * posSide), D2D1::Point2F(posX, posY + posH), s_brush, strokeWidth);
			this->_deviceResources->_d2d1RenderTarget->DrawLine(D2D1::Point2F(posX, posY + posH), D2D1::Point2F(posX + posW * posSide, posY + posH), s_brush, strokeWidth);

			// bottom right
			this->_deviceResources->_d2d1RenderTarget->DrawLine(D2D1::Point2F(posX + posW - posW * posSide, posY + posH), D2D1::Point2F(posX + posW, posY + posH), s_brush, strokeWidth);
			this->_deviceResources->_d2d1RenderTarget->DrawLine(D2D1::Point2F(posX + posW, posY + posH - posH * posSide), D2D1::Point2F(posX + posW, posY + posH), s_brush, strokeWidth);
		}
	}

	this->_deviceResources->_d2d1RenderTarget->EndDraw();
	this->_deviceResources->_d2d1RenderTarget->RestoreDrawingState(this->_deviceResources->_d2d1DrawingStateBlock);

	g_xwa_bracket.clear();

	this->_deviceResources->EndAnnotatedEvent();
}

void PrimarySurface::ResizeForPresent()
{
	HRESULT hr = S_OK;
	const char* step = "";
	auto& resources = this->_deviceResources;
	
	resources->InitInputLayout(resources->_mainInputLayout);
	resources->InitTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	resources->InitVertexShader(resources->_mainVertexShader);
	resources->InitPixelShader(resources->_pixelShaderResize);

	UINT w;
	UINT h;

	if (g_config.AspectRatioPreserved)
	{
		if (resources->_backbufferHeight * resources->_displayWidth <= resources->_backbufferWidth * resources->_displayHeight)
		{
			w = resources->_backbufferHeight * resources->_displayWidth / resources->_displayHeight;
			h = resources->_backbufferHeight;
		}
		else
		{
			w = resources->_backbufferWidth;
			h = resources->_backbufferWidth * resources->_displayHeight / resources->_displayWidth;
		}
	}
	else
	{
		w = resources->_backbufferWidth;
		h = resources->_backbufferHeight;
	}

	UINT left = (resources->_backbufferWidth - w) / 2;
	UINT top = (resources->_backbufferHeight - h) / 2;

	D3D11_VIEWPORT viewport;
	viewport.TopLeftX = (float)left;
	viewport.TopLeftY = (float)top;
	viewport.Width = (float)w;
	viewport.Height = (float)h;
	viewport.MinDepth = D3D11_MIN_DEPTH;
	viewport.MaxDepth = D3D11_MAX_DEPTH;

	resources->InitViewport(&viewport);

	if (SUCCEEDED(hr))
	{
		step = "States";
		resources->InitRasterizerState(resources->_mainRasterizerState);
		resources->InitSamplerState(resources->_mainSamplerState.GetAddressOf(), nullptr);
		resources->InitBlendState(resources->_mainBlendState, nullptr);
		resources->InitDepthStencilState(resources->_mainDepthState, nullptr);
	}

	if (SUCCEEDED(hr))
	{
		step = "offscreenBufferResolved ShaderResourceView";

		resources->InitPSShaderResourceView(resources->_offscreenBufferResolvedView);
	}

	resources->_d3dDeviceContext->OMSetRenderTargets(1, resources->_backBufferRenderTargetView.GetAddressOf(), NULL);

	if (SUCCEEDED(hr))
	{
		step = "Draw";

		UINT stride = sizeof(MainVertex);
		UINT offset = 0;

		resources->InitVertexBuffer(resources->_mainVertexBuffer.GetAddressOf(), &stride, &offset);
		resources->InitIndexBuffer(resources->_mainIndexBuffer, false);
		resources->_d3dDeviceContext->DrawIndexed(6, 0, 0);
	}

	//Restore required render state

	resources->_d3dDeviceContext->OMSetRenderTargets(1, resources->_renderTargetView.GetAddressOf(), resources->_depthStencilView.Get());

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
}