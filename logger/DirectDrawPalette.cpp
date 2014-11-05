#include "common.h"
#include "DirectDrawPalette.h"

DirectDrawPalette::DirectDrawPalette(IDirectDrawPalette* original)
{
	this->_original = original;
}

DirectDrawPalette::~DirectDrawPalette()
{
}

HRESULT DirectDrawPalette::QueryInterface(
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

ULONG DirectDrawPalette::AddRef()
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->AddRef();
}

ULONG DirectDrawPalette::Release()
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

HRESULT DirectDrawPalette::GetCaps(
	LPDWORD lpdwCaps
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->GetCaps(lpdwCaps);
}

HRESULT DirectDrawPalette::GetEntries(
	DWORD dwFlags,
	DWORD dwBase,
	DWORD dwNumEntries,
	LPPALETTEENTRY lpEntries
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->GetEntries(dwFlags, dwBase, dwNumEntries, lpEntries);
}

HRESULT DirectDrawPalette::Initialize(
	LPDIRECTDRAW lpDD,
	DWORD dwFlags,
	LPPALETTEENTRY lpDDColorTable
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->Initialize(lpDD, dwFlags, lpDDColorTable);
}

HRESULT DirectDrawPalette::SetEntries(
	DWORD dwFlags,
	DWORD dwStartingEntry,
	DWORD dwCount,
	LPPALETTEENTRY lpEntries
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->SetEntries(dwFlags, dwStartingEntry, dwCount, lpEntries);
}
