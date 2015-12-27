#include "common.h"
#include "DirectDrawPalette.h"

DirectDrawPalette::DirectDrawPalette()
{
	this->_refCount = 1;
}

DirectDrawPalette::~DirectDrawPalette()
{
}

HRESULT DirectDrawPalette::QueryInterface(
	REFIID riid,
	LPVOID* obp
	)
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	if (riid == IID_IDirectDrawPalette)
	{
		str << " IDirectDrawPalette";
	}
	else
	{
		str << " " << tostr_IID(riid);
	}

	LogText(str.str());
#endif

	if (obp == nullptr)
	{
#if LOGGER
		str.str("\tDDERR_INVALIDPARAMS");
		LogText(str.str());
#endif

		return DDERR_INVALIDPARAMS;
	}

#if LOGGER
	str.str("\tE_NOINTERFACE");
	LogText(str.str());
#endif

	return E_NOINTERFACE;
}

ULONG DirectDrawPalette::AddRef()
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

ULONG DirectDrawPalette::Release()
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

HRESULT DirectDrawPalette::GetCaps(LPDWORD)
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

HRESULT DirectDrawPalette::Initialize(LPDIRECTDRAW, DWORD, LPPALETTEENTRY)
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	LogText(str.str());
#endif

#if LOGGER
	str.str("\tDDERR_ALREADYINITIALIZED");
	LogText(str.str());
#endif

	return DDERR_ALREADYINITIALIZED;
}

HRESULT DirectDrawPalette::GetEntries(DWORD, DWORD, DWORD, LPPALETTEENTRY)
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	LogText(str.str());
#endif

	return DDERR_UNSUPPORTED;
}

HRESULT DirectDrawPalette::SetEntries(DWORD, DWORD, DWORD, LPPALETTEENTRY)
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	LogText(str.str());
#endif

	return DDERR_UNSUPPORTED;
}