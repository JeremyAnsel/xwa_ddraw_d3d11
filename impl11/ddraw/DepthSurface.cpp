#include "common.h"
#include "DeviceResources.h"
#include "DepthSurface.h"

DepthSurface::DepthSurface(DeviceResources* deviceResources)
{
	this->_refCount = 1;
	this->_deviceResources = deviceResources;
}

DepthSurface::~DepthSurface()
{
	if (this->_deviceResources->_depthSurface == this)
	{
		this->_deviceResources->_depthSurface = nullptr;
	}
}

HRESULT DepthSurface::QueryInterface(
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

ULONG DepthSurface::AddRef()
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

ULONG DepthSurface::Release()
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

HRESULT DepthSurface::AddAttachedSurface(
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

HRESULT DepthSurface::AddOverlayDirtyRect(
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

HRESULT DepthSurface::Blt(
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

	if ((dwFlags & DDBLT_DEPTHFILL) != 0 && lpDDBltFx != nullptr)
	{
		str << " " << (void*)lpDDBltFx->dwFillDepth;
	}

	LogText(str.str());
#endif

	if ((dwFlags & DDBLT_DEPTHFILL) != 0 && lpDDBltFx != nullptr)
	{
		this->_deviceResources->clearDepth = *(D3DVALUE*)&lpDDBltFx->dwFillDepth;
	}

	return DD_OK;
}

HRESULT DepthSurface::BltBatch(
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

HRESULT DepthSurface::BltFast(
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
	LogText(str.str());
#endif

#if LOGGER
	str.str("\tDDERR_UNSUPPORTED");
	LogText(str.str());
#endif

	return DDERR_UNSUPPORTED;
}

HRESULT DepthSurface::DeleteAttachedSurface(
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

HRESULT DepthSurface::EnumAttachedSurfaces(
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

HRESULT DepthSurface::EnumOverlayZOrders(
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

HRESULT DepthSurface::Flip(
	LPDIRECTDRAWSURFACE lpDDSurfaceTargetOverride,
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

HRESULT DepthSurface::GetAttachedSurface(
	LPDDSCAPS lpDDSCaps,
	LPDIRECTDRAWSURFACE FAR *lplpDDAttachedSurface
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

HRESULT DepthSurface::GetBltStatus(
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

HRESULT DepthSurface::GetCaps(
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

HRESULT DepthSurface::GetClipper(
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

HRESULT DepthSurface::GetColorKey(
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

HRESULT DepthSurface::GetDC(
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

HRESULT DepthSurface::GetFlipStatus(
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

HRESULT DepthSurface::GetOverlayPosition(
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

HRESULT DepthSurface::GetPalette(
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

HRESULT DepthSurface::GetPixelFormat(
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

HRESULT DepthSurface::GetSurfaceDesc(
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

	memset(lpDDSurfaceDesc, 0, sizeof(DDSURFACEDESC));
	lpDDSurfaceDesc->dwSize = sizeof(DDSURFACEDESC);
	lpDDSurfaceDesc->dwFlags = DDSD_CAPS;
	lpDDSurfaceDesc->ddsCaps.dwCaps = DDSCAPS_ZBUFFER | DDSCAPS_VIDEOMEMORY;

#if LOGGER
	str.str("");
	str << "\t" << tostr_DDSURFACEDESC(lpDDSurfaceDesc);
	LogText(str.str());
#endif

	return DD_OK;
}

HRESULT DepthSurface::Initialize(
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

HRESULT DepthSurface::IsLost()
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

HRESULT DepthSurface::Lock(
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

HRESULT DepthSurface::ReleaseDC(
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

HRESULT DepthSurface::Restore()
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

HRESULT DepthSurface::SetClipper(
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

HRESULT DepthSurface::SetColorKey(
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

HRESULT DepthSurface::SetOverlayPosition(
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

HRESULT DepthSurface::SetPalette(
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

HRESULT DepthSurface::Unlock(
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

HRESULT DepthSurface::UpdateOverlay(
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

HRESULT DepthSurface::UpdateOverlayDisplay(
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

HRESULT DepthSurface::UpdateOverlayZOrder(
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
