// Copyright (c) 2014 Jérémy Ansel
// Licensed under the MIT license. See LICENSE.txt
// Extended for VR by Leo Reyes, 2019

#include "common.h"
#include "DeviceResources.h"
#include "Direct3DTexture.h"
#include "TextureSurface.h"
#include "MipmapSurface.h"
#include <comdef.h>

#include <ScreenGrab.h>
#include <wincodec.h>
#include <vector>

std::vector<uint32_t> HUD_CRCs = {
	0x19f6f5a2, // Next laser available to fire. (master branch)
	0x6acc3e3a, // Green dot for next laser available. (master branch)
	0xdcb8e4f4, // Main Laser HUD (master branch).
	0x1c5e0b86, // HUD warning indicator, left. (master branch)
	0xc54d8171, // HUD warning indicator, mid-left. (master branch)
	0xf4388255, // HUD warning indicator, mid-right. (master branch)
	0xee802582, // HUD warning indicator, right. (master branch)
	0xa4870ab3, // Main Warhead HUD. (master branch)
	0x671e8041, // Warhead HUD, left. (master branch)
	0x6cd5d81f, // Warhead HUD, mid-left,right (master branch)
	0xc33a94b3, // Warhead HUD, right. (master branch)
	0x0793c7d6, // Semi circles that indicate target is ready to be fired upon. (master branch)
	0x756c8f81, // Warhead semi-circles that indicate lock is being acquired. (master branch)
};

std::vector<uint32_t> Text_CRCs = {
	0x201b794e, // 128x128 (master branch)
	0xfcf50e34, // 256x256 (master branch)
	0x42654667  // 256x256 (master branch)
};

std::vector<uint32_t> Floating_GUI_CRCs = {
	0xd08b4437, // (16x16) Laser charge. (master branch)
	0xd0168df9, // (64x64) Laser charge boxes. (master branch)
	0xe321d785, // (64x64) Laser and ion charge boxes on B - Wing. (master branch)
	0xca2a5c48, // (8x8) Laser and ion charge on B - Wing. (master branch)
	0x3b9a3741, // (256x128) Full targetting computer, solid. (master branch)
	0x7e1b021d, // (128x128) Left targetting computer, solid. (master branch)
	0x771a714c  // (256x256) Left targetting computer, frame only. (master branch)
};

// List of regular GUI elements (this is not an exhaustive list). It's mostly used to detect when
// the game has started rendering the GUI
std::vector<uint32_t> GUI_CRCs = {
	0xc2416bf9, // (256x32) Top-left bracket (master branch)
	0x71ce88f1, // (256x32) Top-right bracket (master branch)
	0x75b9e062, // (128x128) Left radar (?) (master branch)
	0x1ec963a9, // (128x128) Right radar (?) (master branch)
	0x3188119f, // (128x128) Left Shield Display (master branch)
	0x75082e5e, // (128x128) Right Tractor Beam Display (master branch)
};

bool isInVector(uint32_t crc, std::vector<uint32_t> &vector) {
	for (uint32_t x : vector)
		if (x == crc)
			return true;
	return false;
}

bool Reload_CRC_vector(std::vector<uint32_t> &data, char *filename) {
	FILE *file;
	int error = 0;

	//log_debug("[DBG] Loading file %s...", filename);
	try {
		error = fopen_s(&file, filename, "rt");
	} catch (...) {
		log_debug("[DBG] Error: %d when loading file: %s", filename);
	}

	if (error != 0)
		return false;

	data.clear();
	char buf[120];
	uint32_t crc;
	while (fgets(buf, 120, file) != NULL) {
		if (strlen(buf) > 0 && buf[0] != ';' && buf[0] != '#') {
			// Read a hex value and add it to the vector
			if (sscanf_s(buf, "0x%x", &crc) > 0) {
				data.push_back(crc);
			}
		}
	}
	//log_debug("[DBG] Read %d CRCs from %s", data.size(), filename);

	fclose(file);
	return true;
}

bool ReloadCRCs() {
	bool result = true;
	result &= Reload_CRC_vector(HUD_CRCs, "./HUD_CRCs.txt");
	result &= Reload_CRC_vector(HUD_CRCs, "./Text_CRCs.txt");
	result &= Reload_CRC_vector(GUI_CRCs, "./GUI_CRCs.txt");
	result &= Reload_CRC_vector(Floating_GUI_CRCs, "./Floating_GUI_CRCs.txt");
	return result;
}

#ifdef DBG_VR
/*
void DumpTexture(ID3D11DeviceContext *context, ID3D11Resource *texture, int index) {
	// DBG: Hack: Save the texture and its CRC
	wchar_t filename[80];
	swprintf_s(filename, 80, L"c:\\temp\\Load-img-%d.png", index);
	DirectX::SaveWICTextureToFile(context, texture, GUID_ContainerFormatPng, filename);
	log_debug("[DBG] Saved Texture: %d", index);
}
*/
#endif

char* convertFormat(char* src, DWORD width, DWORD height, DXGI_FORMAT format)
{
	int length = width * height;
	char* buffer = new char[length * 4];

	if (format == DXGI_FORMAT_B8G8R8A8_UNORM)
	{
		memcpy(buffer, src, length * 4);
	}
	else if (format == DXGI_FORMAT_B4G4R4A4_UNORM)
	{
		unsigned short* srcBuffer = (unsigned short*)src;
		unsigned int* destBuffer = (unsigned int*)buffer;

		for (int i = 0; i < length; ++i)
		{
			unsigned short color16 = srcBuffer[i];

			destBuffer[i] = convertColorB4G4R4A4toB8G8R8A8(color16);
		}
	}
	else if (format == DXGI_FORMAT_B5G5R5A1_UNORM)
	{
		unsigned short* srcBuffer = (unsigned short*)src;
		unsigned int* destBuffer = (unsigned int*)buffer;

		for (int i = 0; i < length; ++i)
		{
			unsigned short color16 = srcBuffer[i];

			destBuffer[i] = convertColorB5G5R5A1toB8G8R8A8(color16);
		}
	}
	else if (format == DXGI_FORMAT_B5G6R5_UNORM)
	{
		unsigned short* srcBuffer = (unsigned short*)src;
		unsigned int* destBuffer = (unsigned int*)buffer;

		for (int i = 0; i < length; ++i)
		{
			unsigned short color16 = srcBuffer[i];

			destBuffer[i] = convertColorB5G6R5toB8G8R8A8(color16);
		}
	}
	else
	{
		memset(buffer, 0, length * 4);
	}

	return buffer;
}

Direct3DTexture::Direct3DTexture(DeviceResources* deviceResources, TextureSurface* surface)
{
	this->_refCount = 1;
	this->_deviceResources = deviceResources;

	this->_surface = surface;
	this->crc = 0;
	this->is_HUD = false;
	this->is_TrianglePointer = false;
	this->is_Text = false;
	this->is_Floating_GUI = false;
	this->is_GUI = false;
}

int Direct3DTexture::GetWidth() {
	return this->_surface->_width;
}

int Direct3DTexture::GetHeight() {
	return this->_surface->_height;
}

Direct3DTexture::~Direct3DTexture()
{
}

HRESULT Direct3DTexture::QueryInterface(
	REFIID riid,
	LPVOID* obp
	)
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	LogText(str.str());
#endif

#if LOGGER
	str.str("\tE_NOINTERFACE");
	LogText(str.str());
#endif

	return E_NOINTERFACE;
}

ULONG Direct3DTexture::AddRef()
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

ULONG Direct3DTexture::Release()
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

HRESULT Direct3DTexture::Initialize(
	LPDIRECT3DDEVICE lpD3DDevice,
	LPDIRECTDRAWSURFACE lpDDSurface)
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

HRESULT Direct3DTexture::GetHandle(
	LPDIRECT3DDEVICE lpDirect3DDevice,
	LPD3DTEXTUREHANDLE lpHandle)
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	LogText(str.str());
#endif

	if (lpHandle == nullptr)
	{
#if LOGGER
		str.str("\tDDERR_INVALIDPARAMS");
		LogText(str.str());
#endif

		return DDERR_INVALIDPARAMS;
	}

	*lpHandle = (D3DTEXTUREHANDLE)this;

	return D3D_OK;
}

HRESULT Direct3DTexture::PaletteChanged(
	DWORD dwStart,
	DWORD dwCount)
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

HRESULT Direct3DTexture::Load(
	LPDIRECT3DTEXTURE lpD3DTexture)
{
#if LOGGER
	std::ostringstream str;
	str << this << " " << __FUNCTION__;
	str << " " << lpD3DTexture;
	LogText(str.str());
#endif

	if (lpD3DTexture == nullptr)
	{
#if LOGGER
		str.str("\tDDERR_INVALIDPARAMS");
		LogText(str.str());
#endif

		return DDERR_INVALIDPARAMS;
	}

	Direct3DTexture* d3dTexture = (Direct3DTexture*)lpD3DTexture;
	TextureSurface* surface = d3dTexture->_surface;

	if (d3dTexture->_textureView)
	{
#if LOGGER
		str.str("\tretrieve existing texture");
		LogText(str.str());
#endif

		*&this->_textureView = d3dTexture->_textureView.Get();
		this->_textureView->AddRef();
		return D3D_OK;
	}

#if LOGGER
	str.str("\tcreate new texture");
	LogText(str.str());
#endif

#if LOGGER
	str.str("");
	str << "\t" << surface->_pixelFormat.dwRGBBitCount;
	str << " " << (void*)surface->_pixelFormat.dwRBitMask;
	str << " " << (void*)surface->_pixelFormat.dwGBitMask;
	str << " " << (void*)surface->_pixelFormat.dwBBitMask;
	str << " " << (void*)surface->_pixelFormat.dwRGBAlphaBitMask;
	LogText(str.str());
#endif

	DWORD bpp = surface->_pixelFormat.dwRGBBitCount == 32 ? 4 : 2;

	DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;

	if (bpp == 4)
	{
		format = DXGI_FORMAT_B8G8R8A8_UNORM;
	}
	else
	{
		DWORD alpha = surface->_pixelFormat.dwRGBAlphaBitMask;

		if (alpha == 0x0000F000)
		{
			format = DXGI_FORMAT_B4G4R4A4_UNORM;
		}
		else if (alpha == 0x00008000)
		{
			format = DXGI_FORMAT_B5G5R5A1_UNORM;
		}
		else
		{
			format = DXGI_FORMAT_B5G6R5_UNORM;
		}
	}

	D3D11_TEXTURE2D_DESC textureDesc;
	textureDesc.Width = surface->_width;
	textureDesc.Height = surface->_height;
	textureDesc.Format = this->_deviceResources->_are16BppTexturesSupported || format == DXGI_FORMAT_B8G8R8A8_UNORM ? format : DXGI_FORMAT_B8G8R8A8_UNORM;
	textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
	textureDesc.MipLevels = surface->_mipmapCount;
	textureDesc.ArraySize = 1;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	D3D11_SUBRESOURCE_DATA* textureData = new D3D11_SUBRESOURCE_DATA[textureDesc.MipLevels];

	bool useBuffers = !this->_deviceResources->_are16BppTexturesSupported && format != DXGI_FORMAT_B8G8R8A8_UNORM;
	char** buffers = nullptr;

	if (useBuffers)
	{
		buffers = new char*[textureDesc.MipLevels];
		buffers[0] = convertFormat(surface->_buffer, surface->_width, surface->_height, format);
	}

	textureData[0].pSysMem = useBuffers ? buffers[0] : surface->_buffer;
	textureData[0].SysMemPitch = surface->_width * (useBuffers ? 4 : bpp);
	textureData[0].SysMemSlicePitch = 0;

	MipmapSurface* mipmap = surface->_mipmap;
	for (DWORD i = 1; i < textureDesc.MipLevels; i++)
	{
		if (useBuffers)
		{
			buffers[i] = convertFormat(mipmap->_buffer, mipmap->_width, mipmap->_height, format);
		}

		textureData[i].pSysMem = useBuffers ? buffers[i] : mipmap->_buffer;
		textureData[i].SysMemPitch = mipmap->_width * (useBuffers ? 4 : bpp);
		textureData[i].SysMemSlicePitch = 0;

		mipmap = mipmap->_mipmap;
	}

	// This is where the various textures are created from data already loaded into RAM
	ComPtr<ID3D11Texture2D> texture;
	HRESULT hr = this->_deviceResources->_d3dDevice->CreateTexture2D(&textureDesc, textureData, &texture);

	if (surface->_mipmapCount == 1) {
		if (surface->_width == 8 || surface->_width == 16 || surface->_width == 32 || surface->_width == 64 ||
			surface->_width == 128 || surface->_width == 256) {
			unsigned int size = surface->_width * surface->_height * (useBuffers ? 4 : bpp);

			// Compute the CRC
			this->crc = crc32c(0, (const unsigned char *)textureData[0].pSysMem, size);

			#ifdef DBG_VR
			// Capture the textures
			{
				static int TexIndex = 0;
				wchar_t filename[80];
				swprintf_s(filename, 80, L"c:\\temp\\master-img-%d.png", TexIndex);

				saveSurface(filename, (char *)textureData[0].pSysMem, surface->_width, surface->_height, bpp);
				log_debug("[DBG] Master Tex: %d, 0x%x, size: %d, %d",
					TexIndex, crc, surface->_width, surface->_height);

				char buf[80];
				sprintf_s(buf, 80, "c:\\temp\\master-crc-%d.txt", TexIndex);
				FILE *file;
				fopen_s(&file, buf, "wt");
				fprintf(file, "0x%x\n", crc);
				fclose(file);

				TexIndex++;
			}
			#endif

			// Check this CRC to see if it's interesting
			if (this->crc == TRIANGLE_PTR_CRC)
				this->is_TrianglePointer = true;
			else if (isInVector(this->crc, HUD_CRCs)) {
				this->is_HUD = true;
			}
			else if (isInVector(this->crc, Floating_GUI_CRCs)) {
				this->is_Floating_GUI = true;
			}
			else if (isInVector(this->crc, Text_CRCs)) {
				this->is_Text = true;
			}
			else if (isInVector(this->crc, GUI_CRCs)) {
				this->is_GUI = true;
			}
		}
	}

	if (useBuffers)
	{
		for (DWORD i = 0; i < textureDesc.MipLevels; i++)
		{
			delete[] buffers[i];
		}

		delete[] buffers;
	}

	delete[] textureData;

	if (FAILED(hr))
	{
		static bool messageShown = false;

		if (!messageShown)
		{
			MessageBox(nullptr, _com_error(hr).ErrorMessage(), __FUNCTION__, MB_ICONERROR);
		}

		messageShown = true;

#if LOGGER
		str.str("\tD3DERR_TEXTURE_LOAD_FAILED");
		LogText(str.str());
#endif

		return D3DERR_TEXTURE_LOAD_FAILED;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC textureViewDesc{};
	textureViewDesc.Format = textureDesc.Format;
	textureViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	textureViewDesc.Texture2D.MipLevels = textureDesc.MipLevels;
	textureViewDesc.Texture2D.MostDetailedMip = 0;

	if (FAILED(this->_deviceResources->_d3dDevice->CreateShaderResourceView(texture, &textureViewDesc, &d3dTexture->_textureView)))
	{
#if LOGGER
		str.str("\tD3DERR_TEXTURE_LOAD_FAILED");
		LogText(str.str());
#endif

		return D3DERR_TEXTURE_LOAD_FAILED;
	}

	*&this->_textureView = d3dTexture->_textureView.Get();
	this->_textureView->AddRef();

	return D3D_OK;
}

HRESULT Direct3DTexture::Unload()
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
