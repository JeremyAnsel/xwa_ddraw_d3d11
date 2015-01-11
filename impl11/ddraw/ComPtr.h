// Copyright (c) 2014 Jérémy Ansel
// Licensed under the MIT license. See LICENSE.txt

#pragma once

template<class T>
class ComPtr
{
private:
	T* _ptr;

public:
	ComPtr()
	{
		this->_ptr = nullptr;
	}

	ComPtr(T* ptr)
	{
		this->_ptr = ptr;
	}

	~ComPtr()
	{
		this->Release();
	}

	operator T*()
	{
		return this->_ptr;
	}

	operator bool()
	{
		return this->_ptr != nullptr;
	}

	T* operator ->()
	{
		return this->_ptr;
	}

	T** operator &()
	{
		this->Release();
		return &this->_ptr;
	}

	void Release()
	{
		if (this->_ptr != nullptr)
		{
			this->_ptr->Release();
			this->_ptr = nullptr;
		}
	}

	T* Get()
	{
		return this->_ptr;
	}

	T** GetAddressOf()
	{
		return &this->_ptr;
	}

	template<class U>
	HRESULT As(U** ptr)
	{
		return this->_ptr->QueryInterface(IID_PPV_ARGS(ptr));
	}
};
