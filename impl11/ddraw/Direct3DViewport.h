#pragma once

class Direct3DViewport : public IDirect3DViewport3
{
public:
	Direct3DViewport(DeviceResources* deviceResources);

	virtual ~Direct3DViewport();

	/*** IUnknown methods ***/

	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID * ppvObj);

	STDMETHOD_(ULONG, AddRef)(THIS);

	STDMETHOD_(ULONG, Release)(THIS);

	/*** IDirect3DViewport methods ***/

	STDMETHOD(Initialize)(THIS_ LPDIRECT3D);

	STDMETHOD(GetViewport)(THIS_ LPD3DVIEWPORT);

	STDMETHOD(SetViewport)(THIS_ LPD3DVIEWPORT);

	STDMETHOD(TransformVertices)(THIS_ DWORD, LPD3DTRANSFORMDATA, DWORD, LPDWORD);

	STDMETHOD(LightElements)(THIS_ DWORD, LPD3DLIGHTDATA);

	STDMETHOD(SetBackground)(THIS_ D3DMATERIALHANDLE);

	STDMETHOD(GetBackground)(THIS_ LPD3DMATERIALHANDLE, LPBOOL);

	STDMETHOD(SetBackgroundDepth)(THIS_ LPDIRECTDRAWSURFACE);

	STDMETHOD(GetBackgroundDepth)(THIS_ LPDIRECTDRAWSURFACE*, LPBOOL);

	STDMETHOD(Clear)(THIS_ DWORD, LPD3DRECT, DWORD);

	STDMETHOD(AddLight)(THIS_ LPDIRECT3DLIGHT);

	STDMETHOD(DeleteLight)(THIS_ LPDIRECT3DLIGHT);

	STDMETHOD(NextLight)(THIS_ LPDIRECT3DLIGHT, LPDIRECT3DLIGHT*, DWORD);

	/*** IDirect3DViewport2 methods ***/
	STDMETHOD(GetViewport2)(THIS_ LPD3DVIEWPORT2);
	STDMETHOD(SetViewport2)(THIS_ LPD3DVIEWPORT2);
	STDMETHOD(SetBackgroundDepth2)(THIS_ LPDIRECTDRAWSURFACE4);
	STDMETHOD(GetBackgroundDepth2)(THIS_ LPDIRECTDRAWSURFACE4*, LPBOOL);

	/*** IDirect3DViewport3 methods ***/
	STDMETHOD(Clear2)(THIS_ DWORD, LPD3DRECT, DWORD, D3DCOLOR, D3DVALUE, DWORD);

	ULONG _refCount;

	DeviceResources* _deviceResources;
};
