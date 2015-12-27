#pragma once

class DirectDrawPalette : public IDirectDrawPalette
{
public:
	DirectDrawPalette();

	virtual ~DirectDrawPalette();

	/*** IUnknown methods ***/

	STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID FAR * ppvObj);

	STDMETHOD_(ULONG, AddRef) (THIS);

	STDMETHOD_(ULONG, Release) (THIS);

	/*** IDirectDrawPalette methods ***/

	STDMETHOD(GetCaps)(THIS_ LPDWORD);

	STDMETHOD(Initialize)(THIS_ LPDIRECTDRAW, DWORD, LPPALETTEENTRY);

	STDMETHOD(GetEntries)(THIS_ DWORD, DWORD, DWORD, LPPALETTEENTRY);

	STDMETHOD(SetEntries)(THIS_ DWORD, DWORD, DWORD, LPPALETTEENTRY);

	ULONG _refCount;
};
