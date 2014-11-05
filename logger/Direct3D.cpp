#include "common.h"
#include "Direct3D.h"
#include "Direct3DViewport.h"

Direct3D::Direct3D(IDirect3D* original)
{
	this->_original = original;
}

Direct3D::~Direct3D()
{
}

HRESULT Direct3D::QueryInterface(
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

ULONG Direct3D::AddRef()
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->AddRef();
}

ULONG Direct3D::Release()
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

HRESULT Direct3D::Initialize(REFCLSID riid)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->Initialize(riid);
}

static LPD3DENUMDEVICESCALLBACK s_lpEnumDevicesCallback;

static HRESULT CALLBACK EnumDevicesCallback(LPGUID lpGuid, LPSTR lpDeviceDescription, LPSTR lpDeviceName, LPD3DDEVICEDESC lpD3DHWDeviceDesc, LPD3DDEVICEDESC lpD3DHELDeviceDesc, LPVOID lpContext)
{
	std::ostringstream str;
	str << "\t" << lpDeviceName << "\t" << lpDeviceDescription;

	LogText(str.str());

	return s_lpEnumDevicesCallback(lpGuid, lpDeviceDescription, lpDeviceName, lpD3DHWDeviceDesc, lpD3DHELDeviceDesc, lpContext);
}

HRESULT Direct3D::EnumDevices(
	LPD3DENUMDEVICESCALLBACK lpEnumDevicesCallback,
	LPVOID lpUserArg
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	s_lpEnumDevicesCallback = lpEnumDevicesCallback;

	return this->_original->EnumDevices(EnumDevicesCallback, lpUserArg);
}

HRESULT Direct3D::CreateLight(
	LPDIRECT3DLIGHT* lplpDirect3DLight,
	IUnknown* pUnkOuter
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->CreateLight(lplpDirect3DLight, pUnkOuter);
}

HRESULT Direct3D::CreateMaterial(
	LPDIRECT3DMATERIAL* lplpDirect3DMaterial,
	IUnknown* pUnkOuter
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->CreateMaterial(lplpDirect3DMaterial, pUnkOuter);
}

HRESULT Direct3D::CreateViewport(
	LPDIRECT3DVIEWPORT* lplpD3DViewport,
	IUnknown* pUnkOuter
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	HRESULT hr = this->_original->CreateViewport(lplpD3DViewport, pUnkOuter);

	if (SUCCEEDED(hr))
	{
		*lplpD3DViewport = GetOrCreateWrapperT(Direct3DViewport, *lplpD3DViewport);
	}

	return hr;
}

HRESULT Direct3D::FindDevice(
	LPD3DFINDDEVICESEARCH lpD3DFDS,
	LPD3DFINDDEVICERESULT lpD3DFDR
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->FindDevice(lpD3DFDS, lpD3DFDR);
}
