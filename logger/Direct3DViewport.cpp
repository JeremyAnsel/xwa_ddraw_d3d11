#include "common.h"
#include "Direct3DViewport.h"
#include "DirectDrawSurface.h"

Direct3DViewport::Direct3DViewport(IDirect3DViewport* original)
{
	this->_original = original;
}

Direct3DViewport::~Direct3DViewport()
{
}

HRESULT Direct3DViewport::QueryInterface(
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

ULONG Direct3DViewport::AddRef()
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->AddRef();
}

ULONG Direct3DViewport::Release()
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

HRESULT Direct3DViewport::Initialize(
	LPDIRECT3D lpDirect3D
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->Initialize(lpDirect3D);
}

HRESULT Direct3DViewport::GetViewport(
	LPD3DVIEWPORT lpData
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->GetViewport(lpData);
}

HRESULT Direct3DViewport::SetViewport(
	LPD3DVIEWPORT lpData
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->SetViewport(lpData);
}

HRESULT Direct3DViewport::TransformVertices(
	DWORD dwVertexCount,
	LPD3DTRANSFORMDATA lpData,
	DWORD dwFlags,
	LPDWORD lpOffscreen
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->TransformVertices(dwVertexCount, lpData, dwFlags, lpOffscreen);
}

HRESULT Direct3DViewport::LightElements(
	DWORD dwElementCount,
	LPD3DLIGHTDATA lpData
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->LightElements(dwElementCount, lpData);
}

HRESULT Direct3DViewport::SetBackground(
	D3DMATERIALHANDLE hMat
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->SetBackground(hMat);
}

HRESULT Direct3DViewport::GetBackground(
	LPD3DMATERIALHANDLE lphMat,
	LPBOOL lpValid
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->GetBackground(lphMat, lpValid);
}

HRESULT Direct3DViewport::SetBackgroundDepth(
	LPDIRECTDRAWSURFACE lpDDSurface
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	if (lpDDSurface)
	{
		lpDDSurface = ((DirectDrawSurface*)lpDDSurface)->_original;
	}

	return this->_original->SetBackgroundDepth(lpDDSurface);
}

HRESULT Direct3DViewport::GetBackgroundDepth(
	LPDIRECTDRAWSURFACE* lplpDDSurface,
	LPBOOL lpValid
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	HRESULT hr = this->_original->GetBackgroundDepth(lplpDDSurface, lpValid);

	if (SUCCEEDED(hr))
	{
		*lplpDDSurface = GetOrCreateWrapperT(DirectDrawSurface, *lplpDDSurface);
	}

	return hr;
}

HRESULT Direct3DViewport::Clear(
	DWORD dwCount,
	LPD3DRECT lpRects,
	DWORD dwFlags
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->Clear(dwCount, lpRects, dwFlags);
}

HRESULT Direct3DViewport::AddLight(
	LPDIRECT3DLIGHT lpDirect3DLight
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->AddLight(lpDirect3DLight);
}

HRESULT Direct3DViewport::DeleteLight(
	LPDIRECT3DLIGHT lpDirect3DLight
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->DeleteLight(lpDirect3DLight);
}

HRESULT Direct3DViewport::NextLight(
	LPDIRECT3DLIGHT lpDirect3DLight,
	LPDIRECT3DLIGHT* lplpDirect3DLight,
	DWORD dwFlags
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->NextLight(lpDirect3DLight, lplpDirect3DLight, dwFlags);
}

HRESULT Direct3DViewport::GetViewport2(
	LPD3DVIEWPORT2 lpData
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return DDERR_UNSUPPORTED;
}

HRESULT Direct3DViewport::SetViewport2(
	LPD3DVIEWPORT2 lpData
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return DDERR_UNSUPPORTED;
}

HRESULT Direct3DViewport::SetBackgroundDepth2(
	LPDIRECTDRAWSURFACE4 lpDDS
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return DDERR_UNSUPPORTED;
}

HRESULT Direct3DViewport::GetBackgroundDepth2(
	LPDIRECTDRAWSURFACE4* lplpDDS,
	LPBOOL lpValid
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return DDERR_UNSUPPORTED;
}

HRESULT Direct3DViewport::Clear2(
	DWORD     dwCount,
	LPD3DRECT lpRects,
	DWORD     dwFlags,
	DWORD     dwColor,
	D3DVALUE  dvZ,
	DWORD     dwStencil
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return ((LPDIRECT3DVIEWPORT3)this->_original)->Clear2(dwCount, lpRects, dwFlags, dwColor, dvZ, dwStencil);
}
