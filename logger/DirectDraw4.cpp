#include "common.h"
#include "DirectDraw4.h"
#include "DirectDrawPalette.h"

DirectDraw4::DirectDraw4(IDirectDraw4* original)
{
	this->_original = original;
}

DirectDraw4::~DirectDraw4()
{
}

HRESULT DirectDraw4::QueryInterface(
	REFIID riid,
	LPVOID* obp
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	str << " " << tostr_IID(riid);

	LogText(str.str());

	*obp = nullptr;

	HRESULT hr = this->_original->QueryInterface(riid, obp);

	if (SUCCEEDED(hr))
	{
	}

	return hr;
}

ULONG DirectDraw4::AddRef()
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->AddRef();
}

ULONG DirectDraw4::Release()
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

HRESULT DirectDraw4::Compact()
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->Compact();
}

HRESULT DirectDraw4::CreateClipper(
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

HRESULT DirectDraw4::CreatePalette(
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

HRESULT DirectDraw4::CreateSurface(
	LPDDSURFACEDESC2 lpDDSurfaceDesc,
	LPDIRECTDRAWSURFACE4 FAR *lplpDDSurface,
	IUnknown FAR *pUnkOuter
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->CreateSurface(lpDDSurfaceDesc, lplpDDSurface, pUnkOuter);
}

HRESULT DirectDraw4::DuplicateSurface(
	LPDIRECTDRAWSURFACE4 lpDDSurface,
	LPDIRECTDRAWSURFACE4 FAR *lplpDupDDSurface
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	str << " " << lpDDSurface;

	LogText(str.str());

	return this->_original->DuplicateSurface(lpDDSurface, lplpDupDDSurface);
}

HRESULT DirectDraw4::EnumDisplayModes(
	DWORD dwFlags,
	LPDDSURFACEDESC2 lpDDSurfaceDesc,
	LPVOID lpContext,
	LPDDENUMMODESCALLBACK2 lpEnumModesCallback
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->EnumDisplayModes(dwFlags, lpDDSurfaceDesc, lpContext, lpEnumModesCallback);
}

HRESULT DirectDraw4::EnumSurfaces(
	DWORD dwFlags,
	LPDDSURFACEDESC2 lpDDSD,
	LPVOID lpContext,
	LPDDENUMSURFACESCALLBACK2 lpEnumSurfacesCallback
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->EnumSurfaces(dwFlags, lpDDSD, lpContext, lpEnumSurfacesCallback);
}

HRESULT DirectDraw4::FlipToGDISurface()
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->FlipToGDISurface();
}

HRESULT DirectDraw4::GetCaps(
	LPDDCAPS lpDDDriverCaps,
	LPDDCAPS lpDDHELCaps
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->GetCaps(lpDDDriverCaps, lpDDHELCaps);
}

HRESULT DirectDraw4::GetDisplayMode(
	LPDDSURFACEDESC2 lpDDSurfaceDesc
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->GetDisplayMode(lpDDSurfaceDesc);
}

HRESULT DirectDraw4::GetFourCCCodes(
	LPDWORD lpNumCodes,
	LPDWORD lpCodes
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->GetFourCCCodes(lpNumCodes, lpCodes);
}

HRESULT DirectDraw4::GetGDISurface(
	LPDIRECTDRAWSURFACE4 FAR *lplpGDIDDSSurface
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->GetGDISurface(lplpGDIDDSSurface);
}

HRESULT DirectDraw4::GetMonitorFrequency(
	LPDWORD lpdwFrequency
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->GetMonitorFrequency(lpdwFrequency);
}

HRESULT DirectDraw4::GetScanLine(
	LPDWORD lpdwScanLine
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->GetScanLine(lpdwScanLine);
}

HRESULT DirectDraw4::GetVerticalBlankStatus(
	LPBOOL lpbIsInVB
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->GetVerticalBlankStatus(lpbIsInVB);
}

HRESULT DirectDraw4::Initialize(
	GUID FAR *lpGUID
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->Initialize(lpGUID);
}

HRESULT DirectDraw4::RestoreDisplayMode()
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->RestoreDisplayMode();
}

HRESULT DirectDraw4::SetCooperativeLevel(
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

HRESULT DirectDraw4::SetDisplayMode(
	DWORD dwWidth,
	DWORD dwHeight,
	DWORD dwBPP,
	DWORD dwRefreshRate,
	DWORD dwFlags
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	str << " " << dwWidth << "x" << dwHeight << " " << dwBPP << " " << dwRefreshRate;

	LogText(str.str());

	return this->_original->SetDisplayMode(dwWidth, dwHeight, dwBPP, dwRefreshRate, dwFlags);
}

HRESULT DirectDraw4::WaitForVerticalBlank(
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

HRESULT DirectDraw4::GetAvailableVidMem(
	LPDDSCAPS2 lpDDSCaps,
	LPDWORD lpdwTotal,
	LPDWORD lpdwFree
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->GetAvailableVidMem(lpDDSCaps, lpdwTotal, lpdwFree);
}

HRESULT DirectDraw4::GetSurfaceFromDC(
	HDC hdc,
	LPDIRECTDRAWSURFACE4 * lpDDS4
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->GetSurfaceFromDC(hdc, lpDDS4);
}

HRESULT DirectDraw4::RestoreAllSurfaces()
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->RestoreAllSurfaces();
}

HRESULT DirectDraw4::TestCooperativeLevel()
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->TestCooperativeLevel();
}

HRESULT DirectDraw4::GetDeviceIdentifier(
	LPDDDEVICEIDENTIFIER lpdddi,
	DWORD dwFlags
	)
{
	return this->_original->GetDeviceIdentifier(lpdddi, dwFlags);
}
