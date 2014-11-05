#pragma once

#include <string>

void* GetWrapper(void* original);
void AddWrapper(void* wrapper, void* original);
void RemoveWrapper(void* original);

template<class Wrap,class Orig>
Wrap* GetOrCreateWrapper(void* original)
{
	void* wrapper = GetWrapper(original);

	if (!wrapper)
	{
		wrapper = new Wrap((Orig*)original);
		AddWrapper(wrapper, original);
	}

	return (Wrap*)wrapper;
}

#define GetOrCreateWrapperT(T, original) GetOrCreateWrapper<T,I ## T>((original))

std::string tostr_IID(REFIID iid);

std::string tostr_DDSURFACEDESC(LPDDSURFACEDESC lpDDSurfaceDesc);
