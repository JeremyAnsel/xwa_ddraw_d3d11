#include "common.h"
#include "Direct3DDevice.h"
#include "Direct3D.h"
#include "Direct3DTexture.h"
#include "Direct3DExecuteBuffer.h"
#include "ExecuteBufferDumper.h"
#include "Direct3DViewport.h"

Direct3DDevice::Direct3DDevice(IDirect3DDevice* original)
{
	this->_original = original;
}

Direct3DDevice::~Direct3DDevice()
{
}

HRESULT Direct3DDevice::QueryInterface(
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

ULONG Direct3DDevice::AddRef()
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->AddRef();
}

ULONG Direct3DDevice::Release()
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

HRESULT Direct3DDevice::Initialize(
	LPDIRECT3D lpd3d,
	LPGUID lpGUID,
	LPD3DDEVICEDESC lpd3ddvdesc
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->Initialize(lpd3d, lpGUID, lpd3ddvdesc);
}

HRESULT Direct3DDevice::GetCaps(
	LPD3DDEVICEDESC lpD3DHWDevDesc,
	LPD3DDEVICEDESC lpD3DHELDevDesc
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->GetCaps(lpD3DHWDevDesc, lpD3DHELDevDesc);
}

HRESULT Direct3DDevice::SwapTextureHandles(
	LPDIRECT3DTEXTURE lpD3DTex1,
	LPDIRECT3DTEXTURE lpD3DTex2
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	if (lpD3DTex1)
	{
		lpD3DTex1 = ((Direct3DTexture*)lpD3DTex1)->_original;
	}

	if (lpD3DTex2)
	{
		lpD3DTex2 = ((Direct3DTexture*)lpD3DTex2)->_original;
	}

	return this->_original->SwapTextureHandles(lpD3DTex1, lpD3DTex2);
}

HRESULT Direct3DDevice::CreateExecuteBuffer(
	LPD3DEXECUTEBUFFERDESC lpDesc,
	LPDIRECT3DEXECUTEBUFFER *lplpDirect3DExecuteBuffer,
	IUnknown *pUnkOuter
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	HRESULT hr = this->_original->CreateExecuteBuffer(lpDesc, lplpDirect3DExecuteBuffer, pUnkOuter);

	if (SUCCEEDED(hr))
	{
		*lplpDirect3DExecuteBuffer = GetOrCreateWrapperT(Direct3DExecuteBuffer, *lplpDirect3DExecuteBuffer);
	}

	return hr;
}

HRESULT Direct3DDevice::GetStats(
	LPD3DSTATS lpD3DStats
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->GetStats(lpD3DStats);
}

HRESULT Direct3DDevice::Execute(
	LPDIRECT3DEXECUTEBUFFER lpDirect3DExecuteBuffer,
	LPDIRECT3DVIEWPORT lpDirect3DViewport,
	DWORD dwFlags
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	if (lpDirect3DExecuteBuffer)
	{
		lpDirect3DExecuteBuffer = ((Direct3DExecuteBuffer*)lpDirect3DExecuteBuffer)->_original;
	}

	if (lpDirect3DViewport)
	{
		lpDirect3DViewport = ((Direct3DViewport*)lpDirect3DViewport)->_original;
	}

	if (lpDirect3DExecuteBuffer)
	{
		DumpExecuteBuffer(lpDirect3DExecuteBuffer);
	}

	return this->_original->Execute(lpDirect3DExecuteBuffer, lpDirect3DViewport, dwFlags);
}

HRESULT Direct3DDevice::AddViewport(
	LPDIRECT3DVIEWPORT lpDirect3DViewport
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	if (lpDirect3DViewport)
	{
		lpDirect3DViewport = ((Direct3DViewport*)lpDirect3DViewport)->_original;
	}

	return this->_original->AddViewport(lpDirect3DViewport);
}

HRESULT Direct3DDevice::DeleteViewport(
	LPDIRECT3DVIEWPORT lpDirect3DViewport
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	if (lpDirect3DViewport)
	{
		lpDirect3DViewport = ((Direct3DViewport*)lpDirect3DViewport)->_original;
	}

	return this->_original->DeleteViewport(lpDirect3DViewport);
}

HRESULT Direct3DDevice::NextViewport(
	LPDIRECT3DVIEWPORT lpDirect3DViewport,
	LPDIRECT3DVIEWPORT *lplpDirect3DViewport,
	DWORD dwFlags
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	if (lpDirect3DViewport)
	{
		lpDirect3DViewport = ((Direct3DViewport*)lpDirect3DViewport)->_original;
	}

	HRESULT hr = this->_original->NextViewport(lpDirect3DViewport, lplpDirect3DViewport, dwFlags);

	if (SUCCEEDED(hr))
	{
		*lplpDirect3DViewport = GetOrCreateWrapperT(Direct3DViewport, *lplpDirect3DViewport);
	}

	return hr;
}

HRESULT Direct3DDevice::Pick(
	LPDIRECT3DEXECUTEBUFFER lpDirect3DExecuteBuffer,
	LPDIRECT3DVIEWPORT lpDirect3DViewport,
	DWORD dwFlags,
	LPD3DRECT lpRect
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	if (lpDirect3DExecuteBuffer)
	{
		lpDirect3DExecuteBuffer = ((Direct3DExecuteBuffer*)lpDirect3DExecuteBuffer)->_original;
	}

	if (lpDirect3DViewport)
	{
		lpDirect3DViewport = ((Direct3DViewport*)lpDirect3DViewport)->_original;
	}

	return this->_original->Pick(lpDirect3DExecuteBuffer, lpDirect3DViewport, dwFlags, lpRect);
}

HRESULT Direct3DDevice::GetPickRecords(
	LPDWORD lpCount,
	LPD3DPICKRECORD lpD3DPickRec
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->GetPickRecords(lpCount, lpD3DPickRec);
}

HRESULT Direct3DDevice::EnumTextureFormats(
	LPD3DENUMTEXTUREFORMATSCALLBACK lpd3dEnumTextureProc,
	LPVOID lpArg
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->EnumTextureFormats(lpd3dEnumTextureProc, lpArg);
}

HRESULT Direct3DDevice::CreateMatrix(
	LPD3DMATRIXHANDLE lpD3DMatHandle
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->CreateMatrix(lpD3DMatHandle);
}

HRESULT Direct3DDevice::SetMatrix(
	D3DMATRIXHANDLE d3dMatHandle,
	LPD3DMATRIX lpD3DMatrix
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->SetMatrix(d3dMatHandle, lpD3DMatrix);
}

HRESULT Direct3DDevice::GetMatrix(
	D3DMATRIXHANDLE d3dMatHandle,
	LPD3DMATRIX lpD3DMatrix
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->GetMatrix(d3dMatHandle, lpD3DMatrix);
}

HRESULT Direct3DDevice::DeleteMatrix(
	D3DMATRIXHANDLE d3dMatHandle
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->DeleteMatrix(d3dMatHandle);
}

HRESULT Direct3DDevice::BeginScene()
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	LogObjFrame();

	return this->_original->BeginScene();
}

HRESULT Direct3DDevice::EndScene()
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->EndScene();
}

HRESULT Direct3DDevice::GetDirect3D(
	LPDIRECT3D *lplpD3D
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	HRESULT hr = this->_original->GetDirect3D(lplpD3D);

	if (SUCCEEDED(hr))
	{
		*lplpD3D = GetOrCreateWrapperT(Direct3D, *lplpD3D);
	}

	return hr;
}
