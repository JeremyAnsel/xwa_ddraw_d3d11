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
#include <WICTextureLoader.h>
#include <wincodec.h>
#include <vector>

const char *TRIANGLE_PTR_RESNAME = "dat,13000,100,";
const char *TARGETING_COMP_RESNAME = "dat,12000,1100,";

std::vector<char *> HUD_ResNames = {
	"dat,12000,1000,", // 0x19f6f5a2, // Next laser available to fire. (master branch)
	"dat,12000,900,",  // 0x6acc3e3a, // Green dot for next laser available. (master branch)
	"dat,12000,500,",  // 0xdcb8e4f4, // Main Laser HUD (master branch).
	"dat,12000,1500,", // 0x1c5e0b86, // HUD warning indicator, left. (master branch)
	"dat,12000,1600,", // 0xc54d8171, // HUD warning indicator, mid-left. (master branch)
	"dat,12000,1700,", // 0xf4388255, // HUD warning indicator, mid-right. (master branch)
	"dat,12000,1800,", // 0xee802582, // HUD warning indicator, right. (master branch)
	"dat,12000,700,",  // 0xa4870ab3, // Main Warhead HUD. (master branch)
	"dat,12000,1900,", // 0x671e8041, // Warhead HUD, left. (master branch)
	"dat,12000,2000,", // 0x6cd5d81f, // Warhead HUD, mid-left,right (master branch) CRC collision!
	"dat,12000,2100,", // 0x6cd5d81f, // Warhead HUD, mid-left,right (master branch) CRC collision!
	"dat,12000,2200,", // 0xc33a94b3, // Warhead HUD, right. (master branch)
	"dat,12000,600,",  // 0x0793c7d6, // Semi circles that indicate target is ready to be fired upon. (master branch)
	"dat,12000,800,",  // 0x756c8f81, // Warhead semi-circles that indicate lock is being acquired. (master branch)
};

std::vector<char *> Text_ResNames = {
	"dat,16000,"
};

std::vector<char *> Floating_GUI_ResNames = {
	"dat,12000,2400,", // 0xd08b4437, (16x16) Laser charge. (master branch)
	"dat,12000,2300,", // 0xd0168df9, (64x64) Laser charge boxes.
	"dat,12000,2500,", // 0xe321d785, (64x64) Laser and ion charge boxes on B - Wing. (master branch)
	"dat,12000,2600,", // 0xca2a5c48, (8x8) Laser and ion charge on B - Wing. (master branch)
	"dat,12000,1100,", // 0x3b9a3741, (256x128) Full targetting computer, solid. (master branch)
	"dat,12000,100,",  // 0x7e1b021d, (128x128) Left targetting computer, solid. (master branch)
	"dat,12000,200,",  // 0x771a714,  (256x256) Left targetting computer, frame only. (master branch)
};

// List of regular GUI elements (this is not an exhaustive list). It's mostly used to detect when
// the game has started rendering the GUI
std::vector<char *> GUI_ResNames = {
	"dat,12000,2700,", // 0xc2416bf9, // (256x32) Top-left bracket (master branch)
	"dat,12000,2800,", // 0x71ce88f1, // (256x32) Top-right bracket (master branch)
	"dat,12000,4500,", // 0x75b9e062, // (128x128) Left radar (master branch)
	"dat,12000,4600,", // 0x1ec963a9, // (128x128) Right radar (master branch)
	"dat,12000,400,",  // 0xbe6846fb, // Right radar when no tractor beam is present
	"dat,12000,4300,", // 0x3188119f, // (128x128) Left Shield Display (master branch)
	"dat,12000,4400,", // 0x75082e5e, // (128x128) Right Tractor Beam Display (master branch)
};

/*
  Target comp tex:		'opt,FlightModels\XwingCockpit.opt,TEX00097,color,0' img-270, size: 256x128
  In low-res mode, this texture is:
	size: 128, 64, name: opt,FlightModels\XwingCockpit.opt,TEX00097,color,0  <-- This is half the size; but still Mipmap 0
  
  Target comp tex:		'opt,FlightModels\XwingCockpit.opt,TEX00097,color,0' img-270, size: 256x128
  Left radar panel:		'opt,FlightModels\XwingCockpit.opt,TEX00096,color,0'
  Right radar panel:		'opt,FlightModels\XwingCockpit.opt,TEX00095,color,0'
  Front panel:			'opt,FlightModels\XwingCockpit.opt,TEX00076,color,0'
  Shields panel:			'opt,FlightModels\XwingCockpit.opt,TEX00098,color,0'
  Lasers panel:			'opt,FlightModels\XwingCockpit.opt,TEX00094,color,0'
*/
// g_DCElements is used when loading textures to load the cover texture.
extern std::vector<dc_element> g_DCElements;
extern bool g_bDynCockpitEnabled;
extern char g_sCurrentCockpit[128];

bool LoadIndividualDCParams(char *sFileName);
void CockpitNameToDCParamsFile(char *CockpitName, char *sFileName, int iFileNameSize);

bool isInVector(uint32_t crc, std::vector<uint32_t> &vector) {
	for (uint32_t x : vector)
		if (x == crc)
			return true;
	return false;
}

bool isInVector(char *name, std::vector<char *> &vector) {
	for (char *x : vector)
		if (strstr(name, x) != NULL)
			return true;
	return false;
}

int isInVector(char *name, std::vector<dc_element> dc_elements) {
	int size = (int)dc_elements.size();
	for (int i = 0; i < size; i++) {
		if (strstr(name, dc_elements[i].name) != NULL)
			return i;
	}
	return -1;
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
	// TODO: Replace with a resname-based reloader... if such a thing makes sense
	bool result = true;
	/*result &= Reload_CRC_vector(HUD_CRCs, "./HUD_CRCs.txt");
	result &= Reload_CRC_vector(Text_CRCs, "./Text_CRCs.txt");
	result &= Reload_CRC_vector(GUI_CRCs, "./GUI_CRCs.txt");
	result &= Reload_CRC_vector(Floating_GUI_CRCs, "./Floating_GUI_CRCs.txt");*/
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

	//this->crc = 0;
	this->is_HUD = false;
	this->is_TrianglePointer = false;
	this->is_Text = false;
	this->is_Floating_GUI = false;
	this->is_GUI = false;
	this->is_TargetingComp = false;
	this->is_Laser = false;
	this->is_LightTexture = false;
	this->is_EngineGlow = false;
	// Dynamic cockpit data
	this->DCElementIndex = -1;
	this->is_DynCockpitDst = false;
	this->is_DynCockpitAlphaOverlay = false;
	this->is_DC_HUDSource = false;
	this->is_DC_TargetCompSrc = false;
	this->is_DC_LeftSensorSrc = false;
	this->is_DC_RightSensorSrc = false;
	this->is_DC_RightSensor2Src = false;
	this->is_DC_ShieldsSrc = false;
	this->is_DC_SolidMsgSrc = false;
	this->is_DC_BorderMsgSrc = false;
	this->is_DC_LaserBoxSrc = false;
	this->is_DC_IonBoxSrc = false;
	this->is_DC_BeamBoxSrc = false;
	this->is_DC_TopLeftSrc = false;
	this->is_DC_TopRightSrc = false;
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
	//log_debug("[DBG] Loading %s", surface->name);
	// The changes from Jeremy's commit fe50cc59e03225bb7e39ae2852e87d305e7c7891 to reduce
	// memory usage cause mipmapped textures to call Load() again. So we must copy all the
	// settings from the input texture to this level.
	this->is_HUD = d3dTexture->is_HUD;
	this->is_TrianglePointer = d3dTexture->is_TrianglePointer;
	this->is_Text = d3dTexture->is_Text;
	this->is_Floating_GUI = d3dTexture->is_Floating_GUI;
	this->is_GUI = d3dTexture->is_GUI;
	this->is_TargetingComp = d3dTexture->is_TargetingComp;
	this->is_Laser = d3dTexture->is_Laser;
	this->is_LightTexture = d3dTexture->is_LightTexture;
	this->is_EngineGlow = d3dTexture->is_EngineGlow;
	// TODO: Remove later:
	// TODO: We don't need to copy texture names around!
	//strncpy_s(this->_surface->_name, d3dTexture->_surface->_name, MAX_TEXTURE_NAME);
	// Dynamic Cockpit data
	this->is_DynCockpitDst = d3dTexture->is_DynCockpitDst;
	this->is_DynCockpitAlphaOverlay = d3dTexture->is_DynCockpitAlphaOverlay;
	this->DCElementIndex = d3dTexture->DCElementIndex;
	this->is_DC_HUDSource = d3dTexture->is_DC_HUDSource;
	this->is_DC_TargetCompSrc = d3dTexture->is_DC_TargetCompSrc;
	this->is_DC_LeftSensorSrc = d3dTexture->is_DC_LeftSensorSrc;
	this->is_DC_RightSensorSrc = d3dTexture->is_DC_RightSensorSrc;
	this->is_DC_RightSensor2Src = d3dTexture->is_DC_RightSensor2Src;
	this->is_DC_ShieldsSrc = d3dTexture->is_DC_ShieldsSrc;
	this->is_DC_SolidMsgSrc = d3dTexture->is_DC_SolidMsgSrc;
	this->is_DC_BorderMsgSrc = d3dTexture->is_DC_BorderMsgSrc;
	this->is_DC_LaserBoxSrc = d3dTexture->is_DC_LaserBoxSrc;
	this->is_DC_IonBoxSrc = d3dTexture->is_DC_IonBoxSrc;
	this->is_DC_BeamBoxSrc = d3dTexture->is_DC_BeamBoxSrc;
	this->is_DC_TopLeftSrc = d3dTexture->is_DC_TopLeftSrc;
	this->is_DC_TopRightSrc = d3dTexture->is_DC_TopRightSrc;

	if (d3dTexture->_textureView)
	{
#if LOGGER
		str.str("\tretrieve existing texture");
		LogText(str.str());
#endif

		d3dTexture->_textureView->AddRef();
		*&this->_textureView = d3dTexture->_textureView.Get();

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
	if (FAILED(hr)) {
		log_debug("[DBG] Failed when calling CreateTexture2D, reason: 0x%x",
			this->_deviceResources->_d3dDevice->GetDeviceRemovedReason());
		goto out;
	}

	if (surface->_mipmapCount == 1) {
		if (surface->_width == 8 || surface->_width == 16 || surface->_width == 32 || surface->_width == 64 ||
			surface->_width == 128 || surface->_width == 256 || surface->_width == 512) 
		{

			// Capture the textures
			#ifdef DBG_VR
			
			{
				static int TexIndex = 0;
				wchar_t filename[300];
				swprintf_s(filename, 300, L"c:\\XWA-Tex-w-names-3\\img-%d.png", TexIndex);
				saveSurface(filename, (char *)textureData[0].pSysMem, surface->_width, surface->_height, bpp);

				char buf[300];
				sprintf_s(buf, 300, "c:\\XWA-Tex-w-names-3\\data-%d.txt", TexIndex);
				FILE *file;
				fopen_s(&file, buf, "wt");
				fprintf(file, "0x%x, size: %d, %d, name: '%s'\n", crc, surface->_width, surface->_height, surface->_name);
				fclose(file);

				TexIndex++;
			}
			#endif

			if (strstr(surface->_name, TRIANGLE_PTR_RESNAME) != NULL)
				this->is_TrianglePointer = true;
			else if (strstr(surface->_name, TARGETING_COMP_RESNAME) != NULL)
				this->is_TargetingComp = true;
			else if (isInVector(surface->_name, HUD_ResNames))
				this->is_HUD = true;
			else if (isInVector(surface->_name, Floating_GUI_ResNames))
				this->is_Floating_GUI = true;
			else if (isInVector(surface->_name, Text_ResNames))
				this->is_Text = true;
			else if (isInVector(surface->_name, GUI_ResNames))
				this->is_GUI = true;

			// Catch the engine glow and mark it
			if (strstr(surface->_name, "dat,1000,1,") != NULL)
				this->is_EngineGlow = true;

			/* Special handling for Dynamic Cockpit source HUD textures */
			if (g_bDynCockpitEnabled) {
				if (strstr(surface->_name, DC_TARGET_COMP_SRC_RESNAME) != NULL) {
					this->is_DC_TargetCompSrc = true;
					this->is_DC_HUDSource = true;
				}
				if (strstr(surface->_name, DC_LEFT_SENSOR_SRC_RESNAME) != NULL) {
					this->is_DC_LeftSensorSrc = true;
					this->is_DC_HUDSource = true;
				}
				if (strstr(surface->_name, DC_RIGHT_SENSOR_SRC_RESNAME) != NULL) {
					this->is_DC_RightSensorSrc = true;
					this->is_DC_HUDSource = true;
				}
				if (strstr(surface->_name, DC_RIGHT_SENSOR_2_SRC_RESNAME) != NULL) {
					this->is_DC_RightSensor2Src = true;
					this->is_DC_HUDSource = true;
				}
				if (strstr(surface->_name, DC_SHIELDS_SRC_RESNAME) != NULL) {
					this->is_DC_ShieldsSrc = true;
					this->is_DC_HUDSource = true;
				}
				if (strstr(surface->_name, DC_SOLID_MSG_SRC_RESNAME) != NULL) {
					this->is_DC_SolidMsgSrc = true;
					this->is_DC_HUDSource = true;
				}
				if (strstr(surface->_name, DC_BORDER_MSG_SRC_RESNAME) != NULL) {
					this->is_DC_BorderMsgSrc = true;
					this->is_DC_HUDSource = true;
				}
				if (strstr(surface->_name, DC_LASER_BOX_SRC_RESNAME) != NULL) {
					this->is_DC_LaserBoxSrc = true;
					this->is_DC_HUDSource = true;
				}
				if (strstr(surface->_name, DC_ION_BOX_SRC_RESNAME) != NULL) {
					this->is_DC_IonBoxSrc = true;
					this->is_DC_HUDSource = true;
				}
				if (strstr(surface->_name, DC_BEAM_BOX_SRC_RESNAME) != NULL) {
					this->is_DC_BeamBoxSrc = true;
					this->is_DC_HUDSource = true;
				}
				if (strstr(surface->_name, DC_TOP_LEFT_SRC_RESNAME) != NULL) {
					this->is_DC_TopLeftSrc = true;
					this->is_DC_HUDSource = true;
				}
				if (strstr(surface->_name, DC_TOP_RIGHT_SRC_RESNAME) != NULL) {
					this->is_DC_TopRightSrc = true;
					this->is_DC_HUDSource = true;
				}
			}
		}
	}
	else if (surface->_mipmapCount > 1) {
		// Catch the laser-related textures
		if (strstr(surface->_name, "Laser") != NULL) {
			// Ignore "LaserBat.OPT"
			if (strstr(surface->_name, "LaserBat") == NULL) {
				this->is_Laser = true;
				// Dump the texture
				/*
				// Dump the texture, let's see it's contents...
				{
					char name[120];
					char *start = strstr(surface->_name, "Laser");
					char *end = strstr(start, ".opt");
					int size = end - start;
					strncpy_s(name, 120, start, size);
					log_debug("[DBG] Laser name: [%s], file: [%s]", surface->_name, name);
					snprintf(name, 120, "c:\\temp\\%s", surface->_name);
					//log_debug("[DBG] Dumping Laser texture for [%s]", surface->_name);
					//saveSurface(name, (char *)textureData[0].pSysMem, surface->_width, surface->_height, bpp);
				}
				*/
			}
		}

		// Catch light textures and mark them appropriately
		if (strstr(surface->_name, ",light,") != NULL)
			this->is_LightTexture = true;

		if (g_bDynCockpitEnabled) {
			// Capture and store the name of the cockpit
			if (g_sCurrentCockpit[0] == 0) {
				if (strstr(surface->_name, "Cockpit") != NULL) {
					//strstr(surface->_name, "Gunner")  != NULL)  {
					//log_debug("[DBG] [DC] Cockpit found");
					char *start = strstr(surface->_name, "\\");
					char *end = strstr(surface->_name, ".opt");
					if (start != NULL && end != NULL) {
						start += 1; // Skip the backslash
						int size = end - start;
						strncpy_s(g_sCurrentCockpit, 128, start, size);
						log_debug("[DBG] [DC] Cockpit Name: '%s'", g_sCurrentCockpit);

						// Load the relevant DC file for the current cockpit
						char sFileName[80];
						CockpitNameToDCParamsFile(g_sCurrentCockpit, sFileName, 80);
						if (!LoadIndividualDCParams(sFileName))
							log_debug("[DBG] [DC] ERROR: Could not load DC params");
					}
				}
			}

			/* Process Dynamic Cockpit destination textures: */
			int idx = isInVector(surface->_name, g_DCElements);
			if (idx > -1) {
				// "light" and "color" textures are processed differently
				if (strstr(surface->_name, ",color") != NULL) {
					// This texture is a Dynamic Cockpit destination texture
					this->is_DynCockpitDst = true;
					// Make this texture "point back" to the right dc_element
					this->DCElementIndex = idx;
					// Activate this dc_element
					g_DCElements[idx].bActive = true;
					// Load the cover texture if necessary
					if (g_DCElements[idx].coverTexture == NULL && g_DCElements[idx].coverTextureName[0] != 0) {
						wchar_t wTexName[MAX_TEXTURE_NAME];
						size_t len = 0;
						mbstowcs_s(&len, wTexName, MAX_TEXTURE_NAME, g_DCElements[idx].coverTextureName, MAX_TEXTURE_NAME);
						HRESULT res = DirectX::CreateWICTextureFromFile(surface->_deviceResources->_d3dDevice,
							wTexName, NULL, &g_DCElements[idx].coverTexture);
						if (FAILED(res)) {
							//log_debug("[DBG] [DC] ***** Could not load cover texture [%s]: 0x%x",
							//	g_DCElements[idx].coverTextureName, res);
							g_DCElements[idx].coverTexture = NULL;
						}
						//else {
						//	log_debug("[DBG] [DC] ***** Loaded cover texture [%s]", g_DCElements[idx].coverTextureName);
						//}
					}
				} else if (strstr(surface->_name, ",light") != NULL) {
					this->is_DynCockpitAlphaOverlay = true;
					//log_debug("[DBG] [DC] Alpha Overlay: [%s]", surface->_name);
					/*if (_stricmp(surface->_name, "TEX00036") == 0) {
						log_debug("[DBG] Dumping light texture for TEX00036...");
						saveSurface(L"c:\\temp\\TEX00036-light", (char *)textureData[0].pSysMem, surface->_width, surface->_height, bpp);
					}*/
				}
			}
		} // if (g_bDynCockpitEnabled)
	}

out:
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

	d3dTexture->_textureView->AddRef();
	*&this->_textureView = d3dTexture->_textureView.Get();

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
