// Copyright (c) 2014 J�r�my Ansel
// Licensed under the MIT license. See LICENSE.txt

#pragma once

std::string wchar_tostring(LPCWSTR text);

#if LOGGER

std::string tostr_IID(REFIID iid);

std::string tostr_DDSURFACEDESC(LPDDSURFACEDESC lpDDSurfaceDesc);

std::string tostr_RECT(LPRECT lpRect);

#endif

void copySurface(char* dest, DWORD destWidth, DWORD destHeight, DWORD destBpp, char* src, DWORD srcWidth, DWORD srcHeight, DWORD srcBpp, DWORD dwX, DWORD dwY, LPRECT lpSrcRect, bool useColorKey);

void scaleSurface(char* dest, DWORD destWidth, DWORD destHeight, DWORD destBpp, char* src, DWORD srcWidth, DWORD srcHeight, DWORD srcBpp);

class ColorConverterTables
{
public:
	ColorConverterTables();

	unsigned char X8toX5[0x100];
	unsigned char X8toX6[0x100];
};

extern ColorConverterTables g_colorConverterTables;

inline unsigned short convertColorB8G8R8X8toB5G6R5(unsigned int color32)
{
	unsigned char red = (unsigned char)((color32 & 0xFF0000) >> 16);
	unsigned char green = (unsigned char)((color32 & 0xFF00) >> 8);
	unsigned char blue = (unsigned char)(color32 & 0xFF);

	red = g_colorConverterTables.X8toX5[red];
	green = g_colorConverterTables.X8toX6[green];
	blue = g_colorConverterTables.X8toX5[blue];

	return (red << 11) | (green << 5) | blue;
}

inline unsigned int convertColorB5G6R5toB8G8R8X8(unsigned short color16)
{
	unsigned red = color16 >> 11;
	red |= red << 5;
	red >>= 2;
	unsigned green = (color16 >> 5) & 0x3f;
	green |= green << 6;
	green >>= 4;
	unsigned blue = color16 & 0x1f;
	blue |= blue << 5;
	blue >>= 2;
	return (red << 16) | (green << 8) | blue;
}

inline unsigned int convertColorB4G4R4A4toB8G8R8A8(unsigned short color16)
{
	unsigned alpha = color16 >> 12;
	alpha |= alpha << 4;
	unsigned red = (color16 >> 8) & 0xf;
	red |= red << 4;
	unsigned green = (color16 >> 4) & 0xf;
	green |= green << 4;
	unsigned blue = color16 & 0xf;
	blue |= blue << 4;
	return (alpha << 24) | (red << 16) | (green << 8) | blue;
}

inline unsigned int convertColorB5G5R5A1toB8G8R8A8(unsigned short color16)
{
	unsigned red = (color16 >> 10) & 0x1f;
	red |= red << 5;
	red >>= 2;
	unsigned green = (color16 >> 5) & 0x1f;
	green |= green << 5;
	green >>= 2;
	unsigned blue = color16 & 0x1f;
	blue |= blue << 5;
	blue >>= 2;
	unsigned alpha = (color16 & 0x8000u) ? 0xff000000u : 0;
	return alpha | (red << 16) | (green << 8) | blue;
}

inline unsigned int convertColorB5G6R5toB8G8R8A8(unsigned short color16)
{
	return convertColorB5G6R5toB8G8R8X8(color16) | 0xff000000u;
}

#if LOGGER

void saveSurface(std::wstring name, char* buffer, DWORD width, DWORD height, DWORD bpp);

#endif
