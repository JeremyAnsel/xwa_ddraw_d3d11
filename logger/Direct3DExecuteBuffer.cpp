#include "common.h"
#include "Direct3DExecuteBuffer.h"

Direct3DExecuteBuffer::Direct3DExecuteBuffer(IDirect3DExecuteBuffer* original)
{
	this->_original = original;
}

Direct3DExecuteBuffer::~Direct3DExecuteBuffer()
{
}

HRESULT Direct3DExecuteBuffer::QueryInterface(
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

ULONG Direct3DExecuteBuffer::AddRef()
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->AddRef();
}

ULONG Direct3DExecuteBuffer::Release()
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

HRESULT Direct3DExecuteBuffer::Initialize(
	LPDIRECT3DDEVICE lpDirect3DDevice,
	LPD3DEXECUTEBUFFERDESC lpDesc
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->Initialize(lpDirect3DDevice, lpDesc);
}

HRESULT Direct3DExecuteBuffer::Lock(
	LPD3DEXECUTEBUFFERDESC lpDesc
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->Lock(lpDesc);
}

HRESULT Direct3DExecuteBuffer::Unlock()
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->Unlock();
}

HRESULT Direct3DExecuteBuffer::SetExecuteData(
	LPD3DEXECUTEDATA lpData
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->SetExecuteData(lpData);
}

HRESULT Direct3DExecuteBuffer::GetExecuteData(
	LPD3DEXECUTEDATA lpData
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->GetExecuteData(lpData);
}

HRESULT Direct3DExecuteBuffer::Validate(
	LPDWORD lpdwOffset,
	LPD3DVALIDATECALLBACK lpFunc,
	LPVOID lpUserArg,
	DWORD dwReserved
	)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->Validate(lpdwOffset, lpFunc, lpUserArg, dwReserved);
}

HRESULT Direct3DExecuteBuffer::Optimize(DWORD dwFlags)
{
	std::ostringstream str;
	str << this << " " << __FUNCTION__;

	LogText(str.str());

	return this->_original->Optimize(dwFlags);
}
