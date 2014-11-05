#include "common.h"
#include "DirectDrawSurface.h"
#include "Direct3DTexture.h"
#include "Direct3DDevice.h"
#include "DirectDrawPalette.h"

DirectDrawSurface::DirectDrawSurface(IDirectDrawSurface* original)
{
	this->_original = original;
}

DirectDrawSurface::~DirectDrawSurface()
{
}

HRESULT DirectDrawSurface::QueryInterface(
	REFIID riid,
	LPVOID* obp
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	str << " " << tostr_IID(riid);

	if (riid == IID_IDirect3DTexture)
	{
		str << " IDirect3DTexture";
	}
	else if (riid == IID_IDirect3DDevice)
	{
		str << " IDirect3DDevice";
	}
	else if (riid == IID_IDirect3DHALDevice)
	{
		str << " IDirect3DHALDevice";
	}

	LogText(str.str());

	*obp = nullptr;

	HRESULT hr = this->_original->QueryInterface(riid, obp);

	if (SUCCEEDED(hr))
	{
		if (riid == IID_IDirect3DTexture)
		{
			*obp = GetOrCreateWrapperT(Direct3DTexture, (LPDIRECT3DTEXTURE)*obp);
		}
		else if (riid == IID_IDirect3DDevice || riid == IID_IDirect3DHALDevice)
		{
			*obp = GetOrCreateWrapperT(Direct3DDevice, (LPDIRECT3DDEVICE)*obp);
		}

		str.str("");
		str << "\t" << *obp;

		LogText(str.str());
	}

	return hr;
}

ULONG DirectDrawSurface::AddRef()
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->AddRef();
}

ULONG DirectDrawSurface::Release()
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	ULONG count = this->_original->Release();

	if (count == 0)
	{
		RemoveWrapper(this->_original);
		delete this;
	}

	return count;
}

HRESULT DirectDrawSurface::AddAttachedSurface(
	LPDIRECTDRAWSURFACE lpDDSAttachedSurface
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	str << " " << lpDDSAttachedSurface;

	LogText(str.str());

	if (lpDDSAttachedSurface)
	{
		lpDDSAttachedSurface = ((DirectDrawSurface*)lpDDSAttachedSurface)->_original;
	}

	return this->_original->AddAttachedSurface(lpDDSAttachedSurface);
}

HRESULT DirectDrawSurface::AddOverlayDirtyRect(
	LPRECT lpRect
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->AddOverlayDirtyRect(lpRect);
}

HRESULT DirectDrawSurface::Blt(
	LPRECT lpDestRect,
	LPDIRECTDRAWSURFACE lpDDSrcSurface,
	LPRECT lpSrcRect,
	DWORD dwFlags,
	LPDDBLTFX lpDDBltFx
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	if (lpDestRect)
	{
		str << " " << lpDestRect->left << "," << lpDestRect->top << "," << lpDestRect->right << "," << lpDestRect->bottom;
	}

	str << " " << lpDDSrcSurface;

	if (lpSrcRect)
	{
		str << " " << lpSrcRect->left << "," << lpSrcRect->top << "," << lpSrcRect->right << "," << lpSrcRect->bottom;
	}

	if (dwFlags & DDBLT_ALPHADEST)
	{
		str << " ALPHADEST";
	}

	if (dwFlags & DDBLT_ALPHADESTCONSTOVERRIDE)
	{
		str << " ALPHADESTCONSTOVERRIDE";
	}

	if (dwFlags & DDBLT_ALPHADESTNEG)
	{
		str << " ALPHADESTNEG";
	}

	if (dwFlags & DDBLT_ALPHADESTSURFACEOVERRIDE)
	{
		str << " ALPHADESTSURFACEOVERRIDE";
	}

	if (dwFlags & DDBLT_ALPHAEDGEBLEND)
	{
		str << " ALPHAEDGEBLEND";
	}

	if (dwFlags & DDBLT_ALPHASRC)
	{
		str << " ALPHASRC";
	}

	if (dwFlags & DDBLT_ALPHASRCCONSTOVERRIDE)
	{
		str << " ALPHASRCCONSTOVERRIDE";
	}

	if (dwFlags & DDBLT_ALPHASRCNEG)
	{
		str << " ALPHASRCNEG";
	}

	if (dwFlags & DDBLT_ALPHASRCSURFACEOVERRIDE)
	{
		str << " ALPHASRCSURFACEOVERRIDE";
	}

	if (dwFlags & DDBLT_ASYNC)
	{
		str << " ASYNC";
	}

	if (dwFlags & DDBLT_COLORFILL)
	{
		str << " COLORFILL";
	}

	if (dwFlags & DDBLT_DDFX)
	{
		str << " DDFX";
	}

	if (dwFlags & DDBLT_DDROPS)
	{
		str << " DDROPS";
	}

	if (dwFlags & DDBLT_KEYDEST)
	{
		str << " KEYDEST";
	}

	if (dwFlags & DDBLT_KEYDESTOVERRIDE)
	{
		str << " KEYDESTOVERRIDE";
	}

	if (dwFlags & DDBLT_KEYSRC)
	{
		str << " KEYSRC";
	}

	if (dwFlags & DDBLT_KEYSRCOVERRIDE)
	{
		str << " KEYSRCOVERRIDE";
	}

	if (dwFlags & DDBLT_ROP)
	{
		str << " ROP";
	}

	if (dwFlags & DDBLT_ROTATIONANGLE)
	{
		str << " ROTATIONANGLE";
	}

	if (dwFlags & DDBLT_ZBUFFER)
	{
		str << " ZBUFFER";
	}

	if (dwFlags & DDBLT_ZBUFFERDESTCONSTOVERRIDE)
	{
		str << " ZBUFFERDESTCONSTOVERRIDE";
	}

	if (dwFlags & DDBLT_ZBUFFERDESTOVERRIDE)
	{
		str << " ZBUFFERDESTOVERRIDE";
	}

	if (dwFlags & DDBLT_ZBUFFERSRCCONSTOVERRIDE)
	{
		str << " ZBUFFERSRCCONSTOVERRIDE";
	}

	if (dwFlags & DDBLT_ZBUFFERSRCOVERRIDE)
	{
		str << " ZBUFFERSRCOVERRIDE";
	}

	if (dwFlags & DDBLT_WAIT)
	{
		str << " WAIT";
	}

	if (dwFlags & DDBLT_DEPTHFILL)
	{
		str << " DEPTHFILL";
	}

	if (dwFlags & DDBLT_DONOTWAIT)
	{
		str << " DONOTWAIT";
	}

	LogText(str.str());

	if (lpDDSrcSurface)
	{
		lpDDSrcSurface = ((DirectDrawSurface*)lpDDSrcSurface)->_original;
	}

	return this->_original->Blt(lpDestRect, lpDDSrcSurface, lpSrcRect, dwFlags, lpDDBltFx);
}

HRESULT DirectDrawSurface::BltBatch(
	LPDDBLTBATCH lpDDBltBatch,
	DWORD dwCount,
	DWORD dwFlags
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->BltBatch(lpDDBltBatch, dwCount, dwFlags);
}

HRESULT DirectDrawSurface::BltFast(
	DWORD dwX,
	DWORD dwY,
	LPDIRECTDRAWSURFACE lpDDSrcSurface,
	LPRECT lpSrcRect,
	DWORD dwTrans
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	str << " " << dwX << "," << dwY;
	str << " " << lpDDSrcSurface;

	if (lpSrcRect)
	{
		str << " " << lpSrcRect->left << "," << lpSrcRect->top << "," << lpSrcRect->right << "," << lpSrcRect->bottom;
	}

	if (dwTrans & DDBLTFAST_NOCOLORKEY)
	{
		str << " NOCOLORKEY";
	}

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

	if (lpDDSrcSurface)
	{
		lpDDSrcSurface = ((DirectDrawSurface*)lpDDSrcSurface)->_original;
	}

	return this->_original->BltFast(dwX, dwY, lpDDSrcSurface, lpSrcRect, dwTrans);
}

HRESULT DirectDrawSurface::DeleteAttachedSurface(
	DWORD dwFlags,
	LPDIRECTDRAWSURFACE lpDDSAttachedSurface
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	str << " " << lpDDSAttachedSurface;

	LogText(str.str());

	if (lpDDSAttachedSurface)
	{
		lpDDSAttachedSurface = ((DirectDrawSurface*)lpDDSAttachedSurface)->_original;
	}

	return this->_original->DeleteAttachedSurface(dwFlags, lpDDSAttachedSurface);
}

HRESULT DirectDrawSurface::EnumAttachedSurfaces(
	LPVOID lpContext,
	LPDDENUMSURFACESCALLBACK lpEnumSurfacesCallback
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->EnumAttachedSurfaces(lpContext, lpEnumSurfacesCallback);
}

HRESULT DirectDrawSurface::EnumOverlayZOrders(
	DWORD dwFlags,
	LPVOID lpContext,
	LPDDENUMSURFACESCALLBACK lpfnCallback
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->EnumOverlayZOrders(dwFlags, lpContext, lpfnCallback);
}

HRESULT DirectDrawSurface::Flip(
	LPDIRECTDRAWSURFACE lpDDSurfaceTargetOverride,
	DWORD dwFlags
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	str << " " << lpDDSurfaceTargetOverride;

	LogText(str.str());

	if (lpDDSurfaceTargetOverride)
	{
		lpDDSurfaceTargetOverride = ((DirectDrawSurface*)lpDDSurfaceTargetOverride)->_original;
	}

	return this->_original->Flip(lpDDSurfaceTargetOverride, dwFlags);
}

HRESULT DirectDrawSurface::GetAttachedSurface(
	LPDDSCAPS lpDDSCaps,
	LPDIRECTDRAWSURFACE FAR *lplpDDAttachedSurface
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	if (lpDDSCaps)
	{
		if (lpDDSCaps->dwCaps & DDSCAPS_ALPHA)
		{
			str << " ALPHA";
		}

		if (lpDDSCaps->dwCaps & DDSCAPS_BACKBUFFER)
		{
			str << " BACKBUFFER";
		}

		if (lpDDSCaps->dwCaps & DDSCAPS_COMPLEX)
		{
			str << " COMPLEX";
		}

		if (lpDDSCaps->dwCaps & DDSCAPS_FLIP)
		{
			str << " FLIP";
		}

		if (lpDDSCaps->dwCaps & DDSCAPS_FRONTBUFFER)
		{
			str << " FRONTBUFFER";
		}

		if (lpDDSCaps->dwCaps & DDSCAPS_OFFSCREENPLAIN)
		{
			str << " OFFSCREENPLAIN";
		}

		if (lpDDSCaps->dwCaps & DDSCAPS_OVERLAY)
		{
			str << " OVERLAY";
		}

		if (lpDDSCaps->dwCaps & DDSCAPS_PALETTE)
		{
			str << " PALETTE";
		}

		if (lpDDSCaps->dwCaps & DDSCAPS_PRIMARYSURFACE)
		{
			str << " PRIMARYSURFACE";
		}

		if (lpDDSCaps->dwCaps & DDSCAPS_PRIMARYSURFACELEFT)
		{
			str << " PRIMARYSURFACELEFT";
		}

		if (lpDDSCaps->dwCaps & DDSCAPS_SYSTEMMEMORY)
		{
			str << " SYSTEMMEMORY";
		}

		if (lpDDSCaps->dwCaps & DDSCAPS_TEXTURE)
		{
			str << " TEXTURE";
		}

		if (lpDDSCaps->dwCaps & DDSCAPS_3DDEVICE)
		{
			str << " 3DDEVICE";
		}

		if (lpDDSCaps->dwCaps & DDSCAPS_VIDEOMEMORY)
		{
			str << " VIDEOMEMORY";
		}

		if (lpDDSCaps->dwCaps & DDSCAPS_VISIBLE)
		{
			str << " VISIBLE";
		}

		if (lpDDSCaps->dwCaps & DDSCAPS_WRITEONLY)
		{
			str << " WRITEONLY";
		}

		if (lpDDSCaps->dwCaps & DDSCAPS_ZBUFFER)
		{
			str << " ZBUFFER";
		}

		if (lpDDSCaps->dwCaps & DDSCAPS_OWNDC)
		{
			str << " OWNDC";
		}

		if (lpDDSCaps->dwCaps & DDSCAPS_LIVEVIDEO)
		{
			str << " LIVEVIDEO";
		}

		if (lpDDSCaps->dwCaps & DDSCAPS_HWCODEC)
		{
			str << " HWCODEC";
		}

		if (lpDDSCaps->dwCaps & DDSCAPS_MODEX)
		{
			str << " MODEX";
		}

		if (lpDDSCaps->dwCaps & DDSCAPS_MIPMAP)
		{
			str << " MIPMAP";
		}

		if (lpDDSCaps->dwCaps & DDSCAPS_ALLOCONLOAD)
		{
			str << " ALLOCONLOAD";
		}

		if (lpDDSCaps->dwCaps & DDSCAPS_VIDEOPORT)
		{
			str << " VIDEOPORT";
		}

		if (lpDDSCaps->dwCaps & DDSCAPS_LOCALVIDMEM)
		{
			str << " LOCALVIDMEM";
		}

		if (lpDDSCaps->dwCaps & DDSCAPS_NONLOCALVIDMEM)
		{
			str << " NONLOCALVIDMEM";
		}

		if (lpDDSCaps->dwCaps & DDSCAPS_STANDARDVGAMODE)
		{
			str << " STANDARDVGAMODE";
		}

		if (lpDDSCaps->dwCaps & DDSCAPS_OPTIMIZED)
		{
			str << " OPTIMIZED";
		}
	}

	LogText(str.str());

	HRESULT hr = this->_original->GetAttachedSurface(lpDDSCaps, lplpDDAttachedSurface);

	if (SUCCEEDED(hr))
	{
		*lplpDDAttachedSurface = GetOrCreateWrapperT(DirectDrawSurface, *lplpDDAttachedSurface);
	}

	str.str("");
	str << "\t" << *lplpDDAttachedSurface;

	LogText(str.str());

	return hr;
}

HRESULT DirectDrawSurface::GetBltStatus(
	DWORD dwFlags
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->GetBltStatus(dwFlags);
}

HRESULT DirectDrawSurface::GetCaps(
	LPDDSCAPS lpDDSCaps
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->GetCaps(lpDDSCaps);
}

HRESULT DirectDrawSurface::GetClipper(
	LPDIRECTDRAWCLIPPER FAR *lplpDDClipper
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->GetClipper(lplpDDClipper);
}

HRESULT DirectDrawSurface::GetColorKey(
	DWORD dwFlags,
	LPDDCOLORKEY lpDDColorKey
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->GetColorKey(dwFlags, lpDDColorKey);
}

HRESULT DirectDrawSurface::GetDC(
	HDC FAR *lphDC
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->GetDC(lphDC);
}

HRESULT DirectDrawSurface::GetFlipStatus(
	DWORD dwFlags
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->GetFlipStatus(dwFlags);
}

HRESULT DirectDrawSurface::GetOverlayPosition(
	LPLONG lplX,
	LPLONG lplY
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->GetOverlayPosition(lplX, lplY);
}

HRESULT DirectDrawSurface::GetPalette(
	LPDIRECTDRAWPALETTE FAR *lplpDDPalette
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	HRESULT hr = this->_original->GetPalette(lplpDDPalette);

	if (SUCCEEDED(hr))
	{
		*lplpDDPalette = GetOrCreateWrapperT(DirectDrawPalette, *lplpDDPalette);
	}

	return hr;
}

HRESULT DirectDrawSurface::GetPixelFormat(
	LPDDPIXELFORMAT lpDDPixelFormat
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->GetPixelFormat(lpDDPixelFormat);
}

HRESULT DirectDrawSurface::GetSurfaceDesc(
	LPDDSURFACEDESC lpDDSurfaceDesc
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	HRESULT hr = this->_original->GetSurfaceDesc(lpDDSurfaceDesc);

	if (SUCCEEDED(hr))
	{
		str.str("");
		str << "\t" << tostr_DDSURFACEDESC(lpDDSurfaceDesc);

		LogText(str.str());
	}

	return hr;
}

HRESULT DirectDrawSurface::Initialize(
	LPDIRECTDRAW lpDD,
	LPDDSURFACEDESC lpDDSurfaceDesc
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->Initialize(lpDD, lpDDSurfaceDesc);
}

HRESULT DirectDrawSurface::IsLost()
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->IsLost();
}

HRESULT DirectDrawSurface::Lock(
	LPRECT lpDestRect,
	LPDDSURFACEDESC lpDDSurfaceDesc,
	DWORD dwFlags,
	HANDLE hEvent
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->Lock(lpDestRect, lpDDSurfaceDesc, dwFlags, hEvent);
}

HRESULT DirectDrawSurface::ReleaseDC(
	HDC hDC
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->ReleaseDC(hDC);
}

HRESULT DirectDrawSurface::Restore()
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->Restore();
}

HRESULT DirectDrawSurface::SetClipper(
	LPDIRECTDRAWCLIPPER lpDDClipper
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->SetClipper(lpDDClipper);
}

HRESULT DirectDrawSurface::SetColorKey(
	DWORD dwFlags,
	LPDDCOLORKEY lpDDColorKey
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->SetColorKey(dwFlags, lpDDColorKey);
}

HRESULT DirectDrawSurface::SetOverlayPosition(
	LONG lX,
	LONG lY
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->SetOverlayPosition(lX, lY);
}

HRESULT DirectDrawSurface::SetPalette(
	LPDIRECTDRAWPALETTE lpDDPalette
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	if (lpDDPalette)
	{
		lpDDPalette = ((DirectDrawPalette*)lpDDPalette)->_original;
	}

	return this->_original->SetPalette(lpDDPalette);
}

HRESULT DirectDrawSurface::Unlock(
	LPVOID lpSurfaceData
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->Unlock(lpSurfaceData);
}

HRESULT DirectDrawSurface::UpdateOverlay(
	LPRECT lpSrcRect,
	LPDIRECTDRAWSURFACE lpDDDestSurface,
	LPRECT lpDestRect,
	DWORD dwFlags,
	LPDDOVERLAYFX lpDDOverlayFx
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	if (lpDDDestSurface)
	{
		lpDDDestSurface = ((DirectDrawSurface*)lpDDDestSurface)->_original;
	}

	return this->_original->UpdateOverlay(lpSrcRect, lpDDDestSurface, lpDestRect, dwFlags, lpDDOverlayFx);
}

HRESULT DirectDrawSurface::UpdateOverlayDisplay(
	DWORD dwFlags
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->UpdateOverlayDisplay(dwFlags);
}

HRESULT DirectDrawSurface::UpdateOverlayZOrder(
	DWORD dwFlags,
	LPDIRECTDRAWSURFACE lpDDSReference
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	if (lpDDSReference)
	{
		lpDDSReference = ((DirectDrawSurface*)lpDDSReference)->_original;
	}

	return this->_original->UpdateOverlayZOrder(dwFlags, lpDDSReference);
}
