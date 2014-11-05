#include "common.h"
#include "Direct3DTexture.h"
#include "Direct3DDevice.h"

Direct3DTexture::Direct3DTexture(IDirect3DTexture* original)
{
	this->_original = original;
}

Direct3DTexture::~Direct3DTexture()
{
}

HRESULT Direct3DTexture::QueryInterface(
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

ULONG Direct3DTexture::AddRef()
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->AddRef();
}

ULONG Direct3DTexture::Release()
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

HRESULT Direct3DTexture::Initialize(LPDIRECT3DDEVICE lpD3DDevice, LPDIRECTDRAWSURFACE lpD3DSurface)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->Initialize(lpD3DDevice, lpD3DSurface);
}

HRESULT Direct3DTexture::GetHandle(
	LPDIRECT3DDEVICE lpDirect3DDevice,
	LPD3DTEXTUREHANDLE lpHandle
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	if (lpDirect3DDevice)
	{
		lpDirect3DDevice = ((Direct3DDevice*)lpDirect3DDevice)->_original;
	}

	return this->_original->GetHandle(lpDirect3DDevice, lpHandle);
}

HRESULT Direct3DTexture::PaletteChanged(
	DWORD dwStart,
	DWORD dwCount
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->PaletteChanged(dwStart, dwCount);
}

HRESULT Direct3DTexture::Load(
	LPDIRECT3DTEXTURE lpD3DTexture
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	str << " " << lpD3DTexture;

	LogText(str.str());

	if (lpD3DTexture)
	{
		lpD3DTexture = ((Direct3DTexture*)lpD3DTexture)->_original;
	}

	return this->_original->Load(lpD3DTexture);
}

HRESULT Direct3DTexture::Unload()
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->Unload();
}
