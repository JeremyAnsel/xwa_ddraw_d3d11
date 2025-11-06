#include "framework.h"
#include "ddraw.h"
#include <WICTextureLoader.h>

#pragma comment(lib, "dxguid")
#pragma comment(lib, "dxgi")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d2d1")

extern "C"
{
	// http://developer.download.nvidia.com/devzone/devcenter/gamegraphics/files/OptimusRenderingPolicies.pdf
	__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;

	// https://gpuopen.com/amdpowerxpressrequesthighperformance/
	__declspec(dllexport) DWORD AmdPowerXpressRequestHighPerformance = 0x00000001;
}

enum ExeDataType
{
	ExeDataType_None,
	ExeDataType_ShowMessage,
	ExeDataType_LoadTexture_Result,
	ExeDataType_LoadTexture_Handle,
	ExeDataType_LoadTexture_Release,
};

void ExeShowMessage(DWORD cbData, PVOID lpData)
{
	std::string message = (const char*)lpData;
	MessageBoxA(NULL, message.c_str(), "ddraw", MB_OK);
}

std::wstring StringToWString(const std::string& str)
{
	wchar_t wTexName[256];
	size_t len = 0;
	mbstowcs_s(&len, wTexName, 256, str.c_str(), 256);
	std::wstring wstr(wTexName);
	return wstr;
}

void GetD3dDevice(ID3D11Device** pDevice, ID3D11DeviceContext** pDeviceContext)
{
	static ComPtr<ID3D11Device> _d3dDevice;
	static ComPtr<ID3D11DeviceContext> _d3dDeviceContext;

	if (!_d3dDevice.Get())
	{
		D3D_FEATURE_LEVEL featureLevels[] =
		{
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0
		};

		UINT numFeatureLevels = ARRAYSIZE(featureLevels);

		D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT, featureLevels, numFeatureLevels, D3D11_SDK_VERSION, &_d3dDevice, nullptr, &_d3dDeviceContext);
	}

	if (pDevice)
	{
		*pDevice = _d3dDevice.Get();
	}

	if (pDeviceContext)
	{
		*pDeviceContext = _d3dDeviceContext.Get();
	}
}

static unsigned int _ddrawLoadTexture_Handle;
static ID3D11Texture2D* _ddrawLoadTexture_Texture;

int ExeDDrawLoadTexture_Result(DWORD cbData, PVOID lpData)
{
	std::wstring name = (const wchar_t*)lpData;
	_ddrawLoadTexture_Handle = 0;
	_ddrawLoadTexture_Texture = nullptr;

	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;
	GetD3dDevice(&device, &deviceContext);

	ComPtr<ID3D11ShaderResourceView> textureSRV;

	HRESULT hr = DirectX::CreateWICTextureFromFileEx(
		device, deviceContext,
		name.c_str(),
		0,
		D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_SHARED,
		DirectX::DX11::WIC_LOADER_DEFAULT,
		(ID3D11Resource**)&_ddrawLoadTexture_Texture, &textureSRV);

	deviceContext->Flush();

	if (SUCCEEDED(hr))
	{
		ComPtr<IDXGIResource> resource;
		_ddrawLoadTexture_Texture->QueryInterface(&resource);
		HANDLE handle = NULL;
		resource->GetSharedHandle(&handle);
		_ddrawLoadTexture_Handle = (unsigned int)handle;
	}

	return hr;
}

int ExeDDrawLoadTexture_Handle(DWORD cbData, PVOID lpData)
{
	return _ddrawLoadTexture_Handle;
}

void ExeDDrawLoadTexture_Release(DWORD cbData, PVOID lpData)
{
	if (_ddrawLoadTexture_Texture)
	{
		_ddrawLoadTexture_Texture->Release();
		_ddrawLoadTexture_Texture = nullptr;
	}
}

LRESULT HandleMessage(ULONG_PTR dwData, DWORD cbData, PVOID lpData)
{
	switch (dwData)
	{
	case ExeDataType_ShowMessage:
		ExeShowMessage(cbData, lpData);
		return 0;

	case ExeDataType_LoadTexture_Result:
		return ExeDDrawLoadTexture_Result(cbData, lpData);

	case ExeDataType_LoadTexture_Handle:
		return ExeDDrawLoadTexture_Handle(cbData, lpData);

	case ExeDataType_LoadTexture_Release:
		ExeDDrawLoadTexture_Release(cbData, lpData);
		return 0;
	}

	return 0;
}
