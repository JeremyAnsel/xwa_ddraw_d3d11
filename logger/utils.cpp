#include "common.h"
#include "utils.h"

#include <unordered_map>

std::unordered_map<void*, void*> g_map;

void* GetWrapper(void* original)
{
	return g_map[original];
}

void AddWrapper(void* wrapper, void* original)
{
	g_map[original] = wrapper;
}

void RemoveWrapper(void* original)
{
	g_map.erase(original);
}

std::string tostr_IID(REFIID iid)
{
	LPOLESTR lpsz;
	StringFromIID(iid, &lpsz);

	std::wstring wstr(lpsz);
	std::string str(wstr.begin(), wstr.end());

	CoTaskMemFree(lpsz);

	return str;
}

std::string tostr_DDSURFACEDESC(LPDDSURFACEDESC lpDDSurfaceDesc)
{
	if (lpDDSurfaceDesc == nullptr)
	{
		return " NULL";
	}

	std::stringstream str;

	DWORD dwFlags = lpDDSurfaceDesc->dwFlags;

	if (dwFlags & DDSD_CAPS)
	{
		DWORD dwCaps = lpDDSurfaceDesc->ddsCaps.dwCaps;

		if (dwCaps & DDSCAPS_ALPHA)
		{
			str << " ALPHA";
		}

		if (dwCaps & DDSCAPS_BACKBUFFER)
		{
			str << " BACKBUFFER";
		}

		if (dwCaps & DDSCAPS_COMPLEX)
		{
			str << " COMPLEX";
		}

		if (dwCaps & DDSCAPS_FLIP)
		{
			str << " FLIP";
		}

		if (dwCaps & DDSCAPS_FRONTBUFFER)
		{
			str << " FRONTBUFFER";
		}

		if (dwCaps & DDSCAPS_OFFSCREENPLAIN)
		{
			str << " OFFSCREENPLAIN";
		}

		if (dwCaps & DDSCAPS_OVERLAY)
		{
			str << " OVERLAY";
		}

		if (dwCaps & DDSCAPS_PALETTE)
		{
			str << " PALETTE";
		}

		if (dwCaps & DDSCAPS_PRIMARYSURFACE)
		{
			str << " PRIMARYSURFACE";
		}

		if (dwCaps & DDSCAPS_SYSTEMMEMORY)
		{
			str << " SYSTEMMEMORY";
		}

		if (dwCaps & DDSCAPS_TEXTURE)
		{
			str << " TEXTURE";
		}

		if (dwCaps & DDSCAPS_3DDEVICE)
		{
			str << " 3DDEVICE";
		}

		if (dwCaps & DDSCAPS_VIDEOMEMORY)
		{
			str << " VIDEOMEMORY";
		}

		if (dwCaps & DDSCAPS_VISIBLE)
		{
			str << " VISIBLE";
		}

		if (dwCaps & DDSCAPS_WRITEONLY)
		{
			str << " WRITEONLY";
		}

		if (dwCaps & DDSCAPS_ZBUFFER)
		{
			str << " ZBUFFER";
		}

		if (dwCaps & DDSCAPS_OWNDC)
		{
			str << " OWNDC";
		}

		if (dwCaps & DDSCAPS_LIVEVIDEO)
		{
			str << " LIVEVIDEO";
		}

		if (dwCaps & DDSCAPS_HWCODEC)
		{
			str << " HWCODEC";
		}

		if (dwCaps & DDSCAPS_MODEX)
		{
			str << " MODEX";
		}

		if (dwCaps & DDSCAPS_MIPMAP)
		{
			str << " MIPMAP";
		}

		if (dwCaps & DDSCAPS_ALLOCONLOAD)
		{
			str << " ALLOCONLOAD";
		}

		if (dwCaps & DDSCAPS_VIDEOPORT)
		{
			str << " VIDEOPORT";
		}

		if (dwCaps & DDSCAPS_LOCALVIDMEM)
		{
			str << " LOCALVIDMEM";
		}

		if (dwCaps & DDSCAPS_NONLOCALVIDMEM)
		{
			str << " NONLOCALVIDMEM";
		}

		if (dwCaps & DDSCAPS_STANDARDVGAMODE)
		{
			str << " STANDARDVGAMODE";
		}

		if (dwCaps & DDSCAPS_OPTIMIZED)
		{
			str << " OPTIMIZED";
		}
	}

	if (dwFlags & (DDSD_HEIGHT | DDSD_WIDTH))
	{
		str << " " << lpDDSurfaceDesc->dwWidth << "x" << lpDDSurfaceDesc->dwHeight;
	}

	if (dwFlags & DDSD_PITCH)
	{

	}

	if (dwFlags & DDSD_BACKBUFFERCOUNT)
	{
		str << " " << lpDDSurfaceDesc->dwBackBufferCount << " back buffers";
	}

	if (dwFlags & DDSD_ZBUFFERBITDEPTH)
	{

	}

	if (dwFlags & DDSD_ALPHABITDEPTH)
	{

	}

	if (dwFlags & DDSD_LPSURFACE)
	{

	}

	if (dwFlags & DDSD_PIXELFORMAT)
	{
		DDPIXELFORMAT& ddpf = lpDDSurfaceDesc->ddpfPixelFormat;

		if (ddpf.dwFlags & DDPF_RGB)
		{
			str << " " << ddpf.dwRGBBitCount << " bpp";
		}
	}

	if (dwFlags & DDSD_CKDESTOVERLAY)
	{

	}

	if (dwFlags & DDSD_CKDESTBLT)
	{

	}

	if (dwFlags & DDSD_CKSRCOVERLAY)
	{

	}

	if (dwFlags & DDSD_CKSRCBLT)
	{

	}

	if (dwFlags & DDSD_MIPMAPCOUNT)
	{
		str << " " << lpDDSurfaceDesc->dwMipMapCount << " mipmaps";
	}

	if (dwFlags & DDSD_REFRESHRATE)
	{
		if (lpDDSurfaceDesc->dwRefreshRate == 0)
		{
			str << " default Hz";
		}
		else
		{
			str << " " << lpDDSurfaceDesc->dwRefreshRate << " Hz";
		}
	}

	if (dwFlags & DDSD_LINEARSIZE)
	{

	}

	return str.str();
}
