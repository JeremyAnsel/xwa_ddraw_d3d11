// Copyright (c) 2014 Jérémy Ansel
// Licensed under the MIT license. See LICENSE.txt

#pragma once

class Direct3D : public IDirect3D
{
public:
	Direct3D(DeviceResources* deviceResources);

	virtual ~Direct3D();

	/*** IUnknown methods ***/

	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj);

	STDMETHOD_(ULONG, AddRef)(THIS);

	STDMETHOD_(ULONG, Release)(THIS);

	/*** IDirect3D methods ***/

	STDMETHOD(Initialize)(THIS_ REFCLSID);

	STDMETHOD(EnumDevices)(THIS_ LPD3DENUMDEVICESCALLBACK, LPVOID);

	STDMETHOD(CreateLight)(THIS_ LPDIRECT3DLIGHT*, IUnknown*);

	STDMETHOD(CreateMaterial)(THIS_ LPDIRECT3DMATERIAL*, IUnknown*);

	STDMETHOD(CreateViewport)(THIS_ LPDIRECT3DVIEWPORT*, IUnknown*);

	STDMETHOD(FindDevice)(THIS_ LPD3DFINDDEVICESEARCH, LPD3DFINDDEVICERESULT);

	ULONG _refCount;

	DeviceResources* _deviceResources;
};
