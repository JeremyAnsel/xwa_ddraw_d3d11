// Copyright (c) 2014 Jérémy Ansel
// Licensed under the MIT license. See LICENSE.txt

#include "common.h"
#include "DeviceResources.h"
#include "Direct3DMaterial.h"
#include "Direct3DViewport.h"

Direct3DViewport::Direct3DViewport(DeviceResources* deviceResources)
{
	this->_refCount = 1;
	this->_deviceResources = deviceResources;
}

Direct3DViewport::~Direct3DViewport()
{
}

HRESULT Direct3DViewport::QueryInterface(
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

ULONG Direct3DViewport::AddRef()
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

ULONG Direct3DViewport::Release()
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

HRESULT Direct3DViewport::Initialize(
	LPDIRECT3D lpDirect3D
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

HRESULT Direct3DViewport::GetViewport(
	LPD3DVIEWPORT lpData
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

HRESULT Direct3DViewport::SetViewport(
	LPD3DVIEWPORT lpData
	)
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	LogText(str.str());
#endif

	if (lpData == nullptr)
	{
		return DDERR_INVALIDPARAMS;
	}

	return D3D_OK;
}

HRESULT Direct3DViewport::TransformVertices(
	DWORD dwVertexCount,
	LPD3DTRANSFORMDATA lpData,
	DWORD dwFlags,
	LPDWORD lpOffscreen
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

HRESULT Direct3DViewport::LightElements(
	DWORD dwElementCount,
	LPD3DLIGHTDATA lpData
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

HRESULT Direct3DViewport::SetBackground(
	D3DMATERIALHANDLE hMat
	)
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	LogText(str.str());
#endif

	// Not 100% sure this is correct, but seems to avoid crashes in XvT.
	((Direct3DMaterial *)hMat)->AddRef();

	clearColor[0] = ((Direct3DMaterial *)hMat)->material.diffuse.r;
	clearColor[1] = ((Direct3DMaterial *)hMat)->material.diffuse.g;
	clearColor[2] = ((Direct3DMaterial *)hMat)->material.diffuse.b;
	clearColor[3] = ((Direct3DMaterial *)hMat)->material.diffuse.a;

	return DD_OK;
}

HRESULT Direct3DViewport::GetBackground(
	LPD3DMATERIALHANDLE lphMat,
	LPBOOL lpValid
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

HRESULT Direct3DViewport::SetBackgroundDepth(
	LPDIRECTDRAWSURFACE lpDDSurface
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

HRESULT Direct3DViewport::GetBackgroundDepth(
	LPDIRECTDRAWSURFACE* lplpDDSurface,
	LPBOOL lpValid
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

HRESULT Direct3DViewport::Clear(
	DWORD dwCount,
	LPD3DRECT lpRects,
	DWORD dwFlags
	)
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	LogText(str.str());
#endif

	std::copy(clearColor, clearColor + 4, this->_deviceResources->clearColor);
	this->_deviceResources->clearColorSet = true;

	return DD_OK;
}

HRESULT Direct3DViewport::AddLight(
	LPDIRECT3DLIGHT lpDirect3DLight
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

HRESULT Direct3DViewport::DeleteLight(
	LPDIRECT3DLIGHT lpDirect3DLight
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

HRESULT Direct3DViewport::NextLight(
	LPDIRECT3DLIGHT lpDirect3DLight,
	LPDIRECT3DLIGHT* lplpDirect3DLight,
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

HRESULT Direct3DViewport::GetViewport2(
	LPD3DVIEWPORT2 lpData
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

HRESULT Direct3DViewport::SetViewport2(
	LPD3DVIEWPORT2 lpData
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

HRESULT Direct3DViewport::SetBackgroundDepth2(
	LPDIRECTDRAWSURFACE4 lpDDS
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

HRESULT Direct3DViewport::GetBackgroundDepth2(
	LPDIRECTDRAWSURFACE4* lplpDDS,
	LPBOOL lpValid
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

HRESULT Direct3DViewport::Clear2(
	DWORD     dwCount,
	LPD3DRECT lpRects,
	DWORD     dwFlags,
	DWORD     dwColor,
	D3DVALUE  dvZ,
	DWORD     dwStencil
	)
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	str << " " << (void*)dwColor << " " << dvZ;
	LogText(str.str());
#endif

	this->_deviceResources->clearDepth = dvZ;
	this->_deviceResources->clearDepthSet = true;

	return D3D_OK;
}
