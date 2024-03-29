// Copyright (c) 2014 J�r�my Ansel
// Licensed under the MIT license. See LICENSE.txt

#include "common.h"
#include "utils.h"

#include <sstream>
#include <iomanip>
#include <memory>
#include <gdiplus.h>
#include <shellapi.h>

#pragma comment(lib, "Gdiplus")

using namespace Gdiplus;

std::string int_to_hex(int i)
{
	std::stringstream stream;
	stream << std::setfill('0') << std::setw(8) << std::hex << i;
	return stream.str();
}

std::string wchar_tostring(LPCWSTR text)
{
	std::wstring wstr(text);
	return std::string(wstr.begin(), wstr.end());
}

std::wstring string_towstring(const std::string& text)
{
	std::wstringstream path;
	path << text.c_str();
	return path.str();
}

std::wstring string_towstring(const char* text)
{
	std::wstringstream path;
	path << text;
	return path.str();
}

#if LOGGER

std::string tostr_IID(REFIID iid)
{
	LPOLESTR lpsz;

	if (FAILED(StringFromIID(iid, &lpsz)))
	{
		return std::string();
	}

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

std::string tostr_RECT(LPRECT lpRect)
{
	if (lpRect == nullptr)
	{
		return " NULL";
	}

	std::stringstream str;

	str << " " << lpRect->left;
	str << " " << lpRect->top;
	str << " " << lpRect->right;
	str << " " << lpRect->bottom;

	return str.str();
}

#endif

void copySurface(char* dest, DWORD destWidth, DWORD destHeight, DWORD destBpp, char* src, DWORD srcWidth, DWORD srcHeight, DWORD srcBpp, DWORD dwX, DWORD dwY, LPRECT lpSrcRect, bool useColorKey)
{
	RECT rc;

	if (lpSrcRect != nullptr)
	{
		rc = *lpSrcRect;
	}
	else
	{
		rc = { 0, 0, static_cast<LONG>(srcWidth), static_cast<LONG>(srcHeight) };
	}

	int h = rc.bottom - rc.top;
	int w = rc.right - rc.left;

	if (destBpp == 2)
	{
		if (srcBpp == 2)
		{
			unsigned short* srcBuffer = (unsigned short*)src + rc.top * srcWidth + rc.left;
			unsigned short* destBuffer = (unsigned short*)dest + dwY * destWidth + dwX;

			if (useColorKey)
			{
				for (int y = 0; y < h; ++y)
				{
					for (int x = 0; x < w; ++x)
					{
						unsigned short color16 = srcBuffer[y * srcWidth + x];

						if (color16 == 0x2000)
							continue;

						destBuffer[y * destWidth + x] = color16;
					}
				}
			}
			else
			{
				for (int y = 0; y < h; ++y)
				{
					for (int x = 0; x < w; ++x)
					{
						unsigned short color16 = srcBuffer[y * srcWidth + x];

						destBuffer[y * destWidth + x] = color16;
					}
				}
			}
		}
		else
		{
			unsigned int* srcBuffer = (unsigned int*)src + rc.top * srcWidth + rc.left;
			unsigned short* destBuffer = (unsigned short*)dest + dwY * destWidth + dwX;

			if (useColorKey)
			{
				for (int y = 0; y < h; ++y)
				{
					for (int x = 0; x < w; ++x)
					{
						unsigned int color32 = srcBuffer[y * srcWidth + x];

						if (color32 == 0x200000)
							continue;

						destBuffer[y * destWidth + x] = convertColorB8G8R8X8toB5G6R5(color32);
					}
				}
			}
			else
			{
				for (int y = 0; y < h; ++y)
				{
					for (int x = 0; x < w; ++x)
					{
						unsigned int color32 = srcBuffer[y * srcWidth + x];

						destBuffer[y * destWidth + x] = convertColorB8G8R8X8toB5G6R5(color32);
					}
				}
			}
		}
	}
	else
	{
		if (srcBpp == 2)
		{
			unsigned short* srcBuffer = (unsigned short*)src + rc.top * srcWidth + rc.left;
			unsigned int* destBuffer = (unsigned int*)dest + dwY * destWidth + dwX;

			if (useColorKey)
			{
				for (int y = 0; y < h; ++y)
				{
					for (int x = 0; x < w; ++x)
					{
						unsigned short color16 = srcBuffer[y * srcWidth + x];

						if (color16 == 0x2000)
							continue;

						destBuffer[y * destWidth + x] = convertColorB5G6R5toB8G8R8X8(color16);
					}
				}
			}
			else
			{
				for (int y = 0; y < h; ++y)
				{
					for (int x = 0; x < w; ++x)
					{
						unsigned short color16 = srcBuffer[y * srcWidth + x];

						destBuffer[y * destWidth + x] = convertColorB5G6R5toB8G8R8X8(color16);
					}
				}
			}
		}
		else
		{
			unsigned int* srcBuffer = (unsigned int*)src + rc.top * srcWidth + rc.left;
			unsigned int* destBuffer = (unsigned int*)dest + dwY * destWidth + dwX;

			if (useColorKey)
			{
				for (int y = 0; y < h; ++y)
				{
					for (int x = 0; x < w; ++x)
					{
						unsigned int color32 = srcBuffer[y * srcWidth + x];

						if (color32 == 0x200000)
							continue;

						destBuffer[y * destWidth + x] = color32 & 0xffffff;
					}
				}
			}
			else
			{
				for (int y = 0; y < h; ++y)
				{
					for (int x = 0; x < w; ++x)
					{
						unsigned int color32 = srcBuffer[y * srcWidth + x];

						destBuffer[y * destWidth + x] = color32 & 0xffffff;
					}
				}
			}
		}
	}
}

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT num = 0;
	UINT size = 0;

	ImageCodecInfo* pImageCodecInfo = nullptr;

	GetImageEncodersSize(&num, &size);
	if (size == 0)
		return -1;  // Failure

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == nullptr)
		return -1;  // Failure

	GetImageEncoders(num, size, pImageCodecInfo);

	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}
	}

	free(pImageCodecInfo);
	return -1;  // Failure
}

class GdiInitializer
{
public:
	GdiInitializer()
	{
		this->status = GdiplusStartup(&token, &gdiplusStartupInput, nullptr);

		GetEncoderClsid(L"image/png", &this->pngClsid);
		GetEncoderClsid(L"image/jpeg", &this->jpgClsid);
	}

	~GdiInitializer()
	{
		if (this->status == 0)
		{
			GdiplusShutdown(token);
		}
	}

	bool hasError()
	{
		return this->status != 0;
	}

	ULONG_PTR token;
	GdiplusStartupInput gdiplusStartupInput;

	Status status;
	CLSID pngClsid;
	CLSID jpgClsid;
};

static GdiInitializer g_gdiInitializer;

void scaleSurface(char* dest, DWORD destWidth, DWORD destHeight, DWORD destBpp, char* src, DWORD srcWidth, DWORD srcHeight, DWORD srcBpp)
{
	if (g_gdiInitializer.hasError())
		return;

	std::unique_ptr<Bitmap> bitmap(new Bitmap(destWidth, destHeight, destBpp == 2 ? PixelFormat16bppRGB565 : PixelFormat32bppRGB));
	std::unique_ptr<Bitmap> bitmapSrc(new Bitmap(srcWidth, srcHeight, srcWidth * srcBpp, srcBpp == 2 ? PixelFormat16bppRGB565 : PixelFormat32bppRGB, (BYTE*)src));

	{
		std::unique_ptr<Graphics> graphics(new Graphics(bitmap.get()));
		graphics->SetInterpolationMode(InterpolationModeNearestNeighbor);

		Rect rc(0, 0, destWidth, destHeight);

		Rect srcRc;

		if (g_config.AspectRatioPreserved)
		{
			if (srcHeight * destWidth <= srcWidth * destHeight)
			{
				srcRc.Width = srcHeight * destWidth / destHeight;
				srcRc.Height = srcHeight;
			}
			else
			{
				srcRc.Width = srcWidth;
				srcRc.Height = srcWidth * destHeight / destWidth;
			}
		}
		else
		{
			srcRc.Width = srcWidth;
			srcRc.Height = srcHeight;
		}

		srcRc.X = (srcWidth - srcRc.Width) / 2;
		srcRc.Y = (srcHeight - srcRc.Height) / 2;

		if (graphics->DrawImage(bitmapSrc.get(), rc, srcRc.X, srcRc.Y, srcRc.Width, srcRc.Height, UnitPixel) == 0)
		{
			BitmapData data;

			if (bitmap->LockBits(&rc, ImageLockModeRead, bitmap->GetPixelFormat(), &data) == 0)
			{
				int rowLength = destWidth * destBpp;

				if (rowLength == data.Stride)
				{
					memcpy(dest, data.Scan0, destHeight * rowLength);
				}
				else
				{
					char* srcBuffer = (char*)data.Scan0;
					char* destBuffer = dest;

					for (DWORD y = 0; y < destHeight; y++)
					{
						memcpy(destBuffer, srcBuffer, rowLength);

						srcBuffer += data.Stride;
						destBuffer += rowLength;
					}
				}

				bitmap->UnlockBits(&data);
			}
		}
	}
}

ColorConverterTables::ColorConverterTables()
{
	// X8toX5, X8toX6
	for (unsigned int c = 0; c < 0x100; c++)
	{
		this->X8toX5[c] = (c * (0x1F * 2) + 0xFF) / (0xFF * 2);
		this->X8toX6[c] = (c * (0x3F * 2) + 0xFF) / (0xFF * 2);
	}

	// B5G6R5toB8G8R8X8
	for (unsigned int color16 = 0; color16 < 0x10000; color16++)
	{
		unsigned int red = (color16 & 0xF800) >> 11;
		unsigned int green = (color16 & 0x7E0) >> 5;
		unsigned int blue = color16 & 0x1F;

		red = (red * (0xFF * 2) + 0x1F) / (0x1F * 2);
		green = (green * (0xFF * 2) + 0x3F) / (0x3F * 2);
		blue = (blue * (0xFF * 2) + 0x1F) / (0x1F * 2);

		unsigned int color32 = (red << 16) | (green << 8) | blue;

		this->B5G6R5toB8G8R8X8[color16] = color32;
	}

	// B4G4R4A4toB8G8R8A8
	for (unsigned int color16 = 0; color16 < 0x10000; color16++)
	{
		unsigned int red = (color16 & 0xF00) >> 8;
		unsigned int green = (color16 & 0xF0) >> 4;
		unsigned int blue = color16 & 0x0F;
		unsigned int alpha = (color16 & 0xF000) >> 12;

		red = (red * (0xFF * 2) + 0x0F) / (0x0F * 2);
		green = (green * (0xFF * 2) + 0x0F) / (0x0F * 2);
		blue = (blue * (0xFF * 2) + 0x0F) / (0x0F * 2);
		alpha = (alpha * (0xFF * 2) + 0x0F) / (0x0F * 2);

		unsigned int color32 = (alpha << 24) | (red << 16) | (green << 8) | blue;

		this->B4G4R4A4toB8G8R8A8[color16] = color32;
	}

	// B5G5R5A1toB8G8R8A8
	for (unsigned int color16 = 0; color16 < 0x10000; color16++)
	{
		unsigned int red = (color16 & 0x7C00) >> 10;
		unsigned int green = (color16 & 0x3E0) >> 5;
		unsigned int blue = color16 & 0x1F;
		unsigned int alpha = (color16 & 0x8000) ? 0xFF : 0;

		red = (red * (0xFF * 2) + 0x1F) / (0x1F * 2);
		green = (green * (0xFF * 2) + 0x1F) / (0x1F * 2);
		blue = (blue * (0xFF * 2) + 0x1F) / (0x1F * 2);

		unsigned int color32 = (alpha << 24) | (red << 16) | (green << 8) | blue;

		this->B5G5R5A1toB8G8R8A8[color16] = color32;
	}

	// B5G6R5toB8G8R8A8
	for (unsigned int color16 = 0; color16 < 0x10000; color16++)
	{
		unsigned int red = (color16 & 0xF800) >> 11;
		unsigned int green = (color16 & 0x7E0) >> 5;
		unsigned int blue = color16 & 0x1F;
		unsigned int alpha = 0xFF;

		red = (red * (0xFF * 2) + 0x1F) / (0x1F * 2);
		green = (green * (0xFF * 2) + 0x3F) / (0x3F * 2);
		blue = (blue * (0xFF * 2) + 0x1F) / (0x1F * 2);

		unsigned int color32 = (alpha << 24) | (red << 16) | (green << 8) | blue;

		this->B5G6R5toB8G8R8A8[color16] = color32;
	}
}

ColorConverterTables g_colorConverterTables;

void saveScreenshot(const std::wstring& filename, char* buffer, DWORD width, DWORD height, DWORD bpp)
{
	if (g_gdiInitializer.hasError())
		return;

	char* image;

	if (bpp == 2)
	{
		image = new char[width * height * 4];
		copySurface(image, width, height, 4, buffer, width, height, bpp, 0, 0, nullptr, false);
	}
	else
	{
		image = buffer;
	}

	EncoderParameters encoderParameters;
	ULONG quality = 100;
	encoderParameters.Count = 1;
	encoderParameters.Parameter[0].Guid = EncoderQuality;
	encoderParameters.Parameter[0].Type = EncoderParameterValueTypeLong;
	encoderParameters.Parameter[0].NumberOfValues = 1;
	encoderParameters.Parameter[0].Value = &quality;

	std::unique_ptr<Bitmap> bitmap(new Bitmap(width, height, width * 4, PixelFormat32bppRGB, (BYTE*)image));

	bitmap->Save(filename.c_str(), &g_gdiInitializer.jpgClsid, &encoderParameters);

	if (bpp == 2)
	{
		delete[] image;
	}
}

void saveSurface(const std::wstring& name, char* buffer, DWORD width, DWORD height, DWORD bpp)
{
	if (g_gdiInitializer.hasError())
		return;

	static int index = 0;

	if (index == 0)
	{
		SHFILEOPSTRUCT file_op = {
			NULL,
			FO_DELETE,
			"_screenshots",
			"",
			FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT,
			false,
			0,
			"" };

		SHFileOperation(&file_op);

		CreateDirectory("_screenshots", nullptr);
	}

	//std::wstring filename = L"_screenshots\\" + std::to_wstring(index) + L"_" + name + L".png";
	std::wstring filename = L"_screenshots\\" + std::to_wstring(index) + L"_" + name + L"_" + std::to_wstring(width) + L"x" + std::to_wstring(height) + L"x" + std::to_wstring(bpp) + L".png";
	index++;

	char* image;

	if (bpp == 2)
	{
		image = new char[width * height * 4];
		copySurface(image, width, height, 4, buffer, width, height, bpp, 0, 0, nullptr, false);
	}
	else
	{
		image = buffer;
	}

	std::unique_ptr<Bitmap> bitmap(new Bitmap(width, height, width * 4, PixelFormat32bppRGB, (BYTE*)image));

	bitmap->Save(filename.c_str(), &g_gdiInitializer.pngClsid, nullptr);

	if (bpp == 2)
	{
		delete[] image;
	}
}
