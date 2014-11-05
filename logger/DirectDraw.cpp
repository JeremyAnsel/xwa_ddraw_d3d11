#include "common.h"
#include "DirectDraw.h"
#include "DirectDraw2.h"
#include "DirectDraw4.h"
#include "Direct3D.h"
#include "DirectDrawSurface.h"
#include "DirectDrawPalette.h"

DirectDraw::DirectDraw(IDirectDraw* original)
{
	this->_original = original;
}

DirectDraw::~DirectDraw()
{
}

HRESULT DirectDraw::QueryInterface(
	REFIID riid,
	LPVOID* obp
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	str << " " << tostr_IID(riid);

	if (riid == IID_IDirectDraw2)
	{
		str << " IDirectDraw2";
	}
	else if (riid == IID_IDirectDraw4)
	{
		str << " IDirectDraw4";
	}
	else if (riid == IID_IDirect3D)
	{
		str << " IDirect3D";
	}

	LogText(str.str());

	*obp = nullptr;

	HRESULT hr = this->_original->QueryInterface(riid, obp);

	if (SUCCEEDED(hr))
	{
		if (riid == IID_IDirectDraw2)
		{
			*obp = GetOrCreateWrapperT(DirectDraw2, *obp);
		}
		else if (riid == IID_IDirectDraw4)
		{
			*obp = GetOrCreateWrapperT(DirectDraw4, *obp);
		}
		else if (riid == IID_IDirect3D)
		{
			*obp = GetOrCreateWrapperT(Direct3D, *obp);
		}
	}

	return hr;
}

ULONG DirectDraw::AddRef()
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->AddRef();
}

ULONG DirectDraw::Release()
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

HRESULT DirectDraw::Compact()
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->Compact();
}

HRESULT DirectDraw::CreateClipper(
	DWORD dwFlags,
	LPDIRECTDRAWCLIPPER FAR *lplpDDClipper,
	IUnknown FAR *pUnkOuter
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->CreateClipper(dwFlags, lplpDDClipper, pUnkOuter);
}

HRESULT DirectDraw::CreatePalette(
	DWORD dwFlags,
	LPPALETTEENTRY lpColorTable,
	LPDIRECTDRAWPALETTE FAR *lplpDDPalette,
	IUnknown FAR *pUnkOuter
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	if (dwFlags & DDPCAPS_1BIT)
	{
		str << " 1BIT";
	}

	if (dwFlags & DDPCAPS_2BIT)
	{
		str << " 2BIT";
	}

	if (dwFlags & DDPCAPS_4BIT)
	{
		str << " 4BIT";
	}

	if (dwFlags & DDPCAPS_8BITENTRIES)
	{
		str << " 8BITENTRIES";
	}

	if (dwFlags & DDPCAPS_8BIT)
	{
		str << " 8BIT";
	}

	if (dwFlags & DDPCAPS_ALLOW256)
	{
		str << " ALLOW256";
	}

	LogText(str.str());

	HRESULT hr = this->_original->CreatePalette(dwFlags, lpColorTable, lplpDDPalette, pUnkOuter);

	if (SUCCEEDED(hr))
	{
		*lplpDDPalette = GetOrCreateWrapperT(DirectDrawPalette, *lplpDDPalette);
	}

	return hr;
}

HRESULT DirectDraw::CreateSurface(
	LPDDSURFACEDESC lpDDSurfaceDesc,
	LPDIRECTDRAWSURFACE FAR *lplpDDSurface,
	IUnknown FAR *pUnkOuter
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	str << tostr_DDSURFACEDESC(lpDDSurfaceDesc);

	LogText(str.str());

	HRESULT hr = this->_original->CreateSurface(lpDDSurfaceDesc, lplpDDSurface, pUnkOuter);

	if (SUCCEEDED(hr))
	{
		*lplpDDSurface = GetOrCreateWrapperT(DirectDrawSurface, *lplpDDSurface);
	}

	str.str("");
	str << "\t" << *lplpDDSurface;

	LogText(str.str());

	return hr;
}

HRESULT DirectDraw::DuplicateSurface(
	LPDIRECTDRAWSURFACE lpDDSurface,
	LPDIRECTDRAWSURFACE FAR *lplpDupDDSurface
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	str << " " << lpDDSurface;

	LogText(str.str());

	if (lpDDSurface)
	{
		lpDDSurface = ((DirectDrawSurface*)lpDDSurface)->_original;
	}

	HRESULT hr = this->_original->DuplicateSurface(lpDDSurface, lplpDupDDSurface);

	if (SUCCEEDED(hr))
	{
		*lplpDupDDSurface = GetOrCreateWrapperT(DirectDrawSurface, *lplpDupDDSurface);
	}

	return hr;
}

HRESULT DirectDraw::EnumDisplayModes(
	DWORD dwFlags,
	LPDDSURFACEDESC lpDDSurfaceDesc,
	LPVOID lpContext,
	LPDDENUMMODESCALLBACK lpEnumModesCallback
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->EnumDisplayModes(dwFlags, lpDDSurfaceDesc, lpContext, lpEnumModesCallback);
}

HRESULT DirectDraw::EnumSurfaces(
	DWORD dwFlags,
	LPDDSURFACEDESC lpDDSD,
	LPVOID lpContext,
	LPDDENUMSURFACESCALLBACK lpEnumSurfacesCallback
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->EnumSurfaces(dwFlags, lpDDSD, lpContext, lpEnumSurfacesCallback);
}

HRESULT DirectDraw::FlipToGDISurface()
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->FlipToGDISurface();
}

HRESULT DirectDraw::GetCaps(
	LPDDCAPS lpDDDriverCaps,
	LPDDCAPS lpDDHELCaps
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->GetCaps(lpDDDriverCaps, lpDDHELCaps);
}

HRESULT DirectDraw::GetDisplayMode(
	LPDDSURFACEDESC lpDDSurfaceDesc
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->GetDisplayMode(lpDDSurfaceDesc);
}

HRESULT DirectDraw::GetFourCCCodes(
	LPDWORD lpNumCodes,
	LPDWORD lpCodes
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->GetFourCCCodes(lpNumCodes, lpCodes);
}

HRESULT DirectDraw::GetGDISurface(
	LPDIRECTDRAWSURFACE FAR *lplpGDIDDSSurface
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	HRESULT hr = this->_original->GetGDISurface(lplpGDIDDSSurface);

	if (SUCCEEDED(hr))
	{
		*lplpGDIDDSSurface = GetOrCreateWrapperT(DirectDrawSurface, *lplpGDIDDSSurface);
	}

	return hr;
}

HRESULT DirectDraw::GetMonitorFrequency(
	LPDWORD lpdwFrequency
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->GetMonitorFrequency(lpdwFrequency);
}

HRESULT DirectDraw::GetScanLine(
	LPDWORD lpdwScanLine
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->GetScanLine(lpdwScanLine);
}

HRESULT DirectDraw::GetVerticalBlankStatus(
	LPBOOL lpbIsInVB
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->GetVerticalBlankStatus(lpbIsInVB);
}

HRESULT DirectDraw::Initialize(
	GUID FAR *lpGUID
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->Initialize(lpGUID);
}

HRESULT DirectDraw::RestoreDisplayMode()
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->RestoreDisplayMode();
}

HRESULT DirectDraw::SetCooperativeLevel(
	HWND hWnd,
	DWORD dwFlags
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	str << " " << hWnd;

	if (dwFlags & DDSCL_FULLSCREEN)
	{
		str << " FULLSCREEN";
	}

	if (dwFlags & DDSCL_ALLOWREBOOT)
	{
		str << " ALLOWREBOOT";
	}

	if (dwFlags & DDSCL_NOWINDOWCHANGES)
	{
		str << " NOWINDOWCHANGES";
	}

	if (dwFlags & DDSCL_NORMAL)
	{
		str << " NORMAL";
	}

	if (dwFlags & DDSCL_EXCLUSIVE)
	{
		str << " EXCLUSIVE";
	}

	if (dwFlags & DDSCL_ALLOWMODEX)
	{
		str << " ALLOWMODEX";
	}

	if (dwFlags & DDSCL_SETFOCUSWINDOW)
	{
		str << " SETFOCUSWINDOW";
	}

	if (dwFlags & DDSCL_SETDEVICEWINDOW)
	{
		str << " SETDEVICEWINDOW";
	}

	if (dwFlags & DDSCL_CREATEDEVICEWINDOW)
	{
		str << " CREATEDEVICEWINDOW";
	}

	LogText(str.str());

	return this->_original->SetCooperativeLevel(hWnd, dwFlags);
}

HRESULT DirectDraw::SetDisplayMode(
	DWORD dwWidth,
	DWORD dwHeight,
	DWORD dwBPP
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	str << " " << dwWidth << "x" << dwHeight << " " << dwBPP;

	LogText(str.str());

	return this->_original->SetDisplayMode(dwWidth, dwHeight, dwBPP);
}

HRESULT DirectDraw::WaitForVerticalBlank(
	DWORD dwFlags,
	HANDLE hEvent
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	if (dwFlags & DDWAITVB_BLOCKBEGIN)
	{
		str << " BLOCKBEGIN";
	}

	if (dwFlags & DDWAITVB_BLOCKBEGINEVENT)
	{
		str << " BLOCKBEGINEVENT";
	}

	if (dwFlags & DDWAITVB_BLOCKEND)
	{
		str << " BLOCKEND";
	}

	LogText(str.str());

	return this->_original->WaitForVerticalBlank(dwFlags, hEvent);
}
