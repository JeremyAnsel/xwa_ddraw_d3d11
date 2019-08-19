// Copyright (c) 2014 Jérémy Ansel
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

	unsigned int B5G6R5toB8G8R8X8[0x10000];

	unsigned int B4G4R4A4toB8G8R8A8[0x10000];

	unsigned int B5G5R5A1toB8G8R8A8[0x10000];

	unsigned int B5G6R5toB8G8R8A8[0x10000];
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
	return g_colorConverterTables.B5G6R5toB8G8R8X8[color16];
}

inline unsigned int convertColorB4G4R4A4toB8G8R8A8(unsigned short color16)
{
	return g_colorConverterTables.B4G4R4A4toB8G8R8A8[color16];
}

inline unsigned int convertColorB5G5R5A1toB8G8R8A8(unsigned short color16)
{
	return g_colorConverterTables.B5G5R5A1toB8G8R8A8[color16];
}

inline unsigned int convertColorB5G6R5toB8G8R8A8(unsigned short color16)
{
	return g_colorConverterTables.B5G6R5toB8G8R8A8[color16];
}

//#if LOGGER
void saveSurface(std::wstring name, char* buffer, DWORD width, DWORD height, DWORD bpp);
//#endif

void log_debug(const char *format, ...);
