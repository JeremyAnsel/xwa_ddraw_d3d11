#include "CubeMaps.h"
#include "xwa_structures.h"
#include "hook_config.h"
#include "hookexe.h"
#include <map>
#include <filesystem>

#ifdef _DEBUG
#include "../Debug/CubeMapsVertexShader.h"
#include "../Debug/CubeMapsPixelShader.h"
#else
#include "../Release/CubeMapsVertexShader.h"
#include "../Release/CubeMapsPixelShader.h"
#endif

namespace fs = std::filesystem;

const int MAX_TEXTURE_NAME = 128;

// Backdrops
bool g_bBackdropsReset = true;
int g_iBackdropsToTag = -1, g_iBackdropsTagged = 0;
std::map<int, int> g_BackdropIdToGroupId;
std::map<int, bool> g_StarfieldGroupIdImageIdMap;
std::map<int, bool> g_DisabledGroupIdImageIdMap;
std::map<int, bool> g_EnabledOvrGroupIdImageIdMap;
std::map<int, void*> g_GroupIdImageIdToTextureMap;
Direct3DTexture* g_StarfieldSRVs[STARFIELD_TYPE::MAX] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

CubeMapData g_CubeMaps;
bool g_bDefaultStarfieldRendered = false;

const float RAD_TO_DEG = 180.0f / 3.141593f;
const float DEG_TO_RAD = 3.141593f / 180.0f;

static void ReloadCubeMapsResources(DeviceResources* deviceResources);
static void RenderCubeMaps(DeviceResources* deviceResources);

static bool RenderCubeMapInThisRegion(int* region_out)
{
	if (*g_playerInHangar)
		*region_out = 0;
	else
		*region_out = PlayerDataTable[*g_playerIndex].Region;
	const bool validRegion = (*region_out >= 0 && *region_out < MAX_MISSION_REGIONS);
	return (validRegion && g_CubeMaps.bRenderInThisRegion[*region_out]);
}

/// <summary>
/// Returns 0xFFFF when no cubemaps are enabled in the current region (including AllRegions).
/// Returns -1 if AllRegions has a cubemap enabled
/// Returns 0..MAX_MISSION_REGIONS - 1 if the current region has a cubemap.
/// This is useful to tell when a new cubemap has been activated after jumping between regions
/// or a mission is (re)loaded.
/// </summary>
/// <returns></returns>
static int GetCurrentCubeMapRegion()
{
	int region = -1;
	bool regionEnabled = RenderCubeMapInThisRegion(&region);
	return  regionEnabled ? region : -1;
}

static bool RenderIllumCubeMapInThisRegion()
{
	int region = 0;
	if (*g_playerInHangar)
		region = 0;
	else
		region = PlayerDataTable[*g_playerIndex].Region;
	const bool validRegion = (region >= 0 && region < MAX_MISSION_REGIONS);
	return (validRegion && g_CubeMaps.bRenderIllumInThisRegion[region]);
}

static bool RenderOverlayCubeMapInThisRegion()
{
	int region = 0;
	if (*g_playerInHangar)
		region = 0;
	else
		region = PlayerDataTable[*g_playerIndex].Region;
	const bool validRegion = (region >= 0 && region < MAX_MISSION_REGIONS);
	return (validRegion && g_CubeMaps.bRenderOvrInThisRegion[region]);
}

void RenderDefaultBackground(DeviceResources* deviceResources, bool* drawCubeMap, Matrix4* cubeMapRot, bool* drawOvrCubeMap, Matrix4* ovrCubeMapRot)
{
	//if (!g_bRenderDefaultStarfield || !g_bRendering3D || g_bDefaultStarfieldRendered)
	//	return;

	*drawCubeMap = false;
	*drawOvrCubeMap = false;

	int region = 0;
	const bool renderCubeMapInThisRegion = RenderCubeMapInThisRegion(&region);
	const bool renderIllumCubeMapInThisRegion = RenderIllumCubeMapInThisRegion();
	const bool renderOvrCubeMapInThisRegion = RenderOverlayCubeMapInThisRegion();
	const bool renderCubeMap = (renderCubeMapInThisRegion || g_CubeMaps.bRenderAllRegions);
	const bool renderOvrCubeMap = (renderOvrCubeMapInThisRegion || g_CubeMaps.bAllRegionsOvr);

	float angX = g_CubeMaps.allRegionsAngX;
	float angY = g_CubeMaps.allRegionsAngY;
	float angZ = g_CubeMaps.allRegionsAngZ;
	float ovrAngX = g_CubeMaps.allRegionsOvrAngX;
	float ovrAngY = g_CubeMaps.allRegionsOvrAngY;
	float ovrAngZ = g_CubeMaps.allRegionsOvrAngZ;
	if (renderCubeMapInThisRegion)
	{
		angX = g_CubeMaps.regionAngX[region];
		angY = g_CubeMaps.regionAngY[region];
		angZ = g_CubeMaps.regionAngZ[region];
		ovrAngX = g_CubeMaps.regionOvrAngX[region];
		ovrAngY = g_CubeMaps.regionOvrAngY[region];
		ovrAngZ = g_CubeMaps.regionOvrAngZ[region];
	}

	const Matrix4 Rx = Matrix4().rotateX(angX);
	const Matrix4 Ry = Matrix4().rotateY(angY);
	const Matrix4 Rz = Matrix4().rotateZ(angZ);
	const Matrix4 ovrRx = Matrix4().rotateX(ovrAngX);
	const Matrix4 ovrRy = Matrix4().rotateY(ovrAngY);
	const Matrix4 ovrRz = Matrix4().rotateZ(ovrAngZ);
	Matrix4 mapRot = Rz * Ry * Rx;
	Matrix4 ovrMapRot = ovrRx * ovrRy * ovrRz;

	if (*g_playerInHangar)
	{
		Matrix4 heading = Matrix4().rotateX(-90);
		mapRot *= heading;
		ovrMapRot *= heading;
	}

	*drawCubeMap = renderCubeMap;
	*cubeMapRot = mapRot;
	*drawOvrCubeMap = renderOvrCubeMap;
	*ovrCubeMapRot = ovrMapRot;
}

float FindMipLevel(float texResolution, const float mipResolution)
{
	float mipLevel = 0;
	if (texResolution <= 0) return 0;

	// Find the mip level that makes this texture small enough for
	// diffuse illumination
	while (texResolution > mipResolution)
	{
		texResolution /= 2.0f;
		mipLevel += 1.0f;
	}

	return mipLevel;
}

std::vector<std::wstring> ListFiles(std::string path)
{
	std::vector<std::wstring> result;
	std::vector<std::wstring> temp;
	std::vector<int> keys;

	OutputDebugString(("[DBG] [CUBE] Listing files under [" + path + "]").c_str());
	if (!fs::exists(path) || !fs::is_directory(path)) {
		OutputDebugString(("[DBG] [CUBE] ERROR: Directory " + path + " does not exist").c_str());
		return result;
	}

	for (const auto& entry : fs::directory_iterator(path)) {
		// Check if the entry is a regular file
		if (fs::is_regular_file(entry.status())) {
			std::string fullName = path + "\\" + entry.path().filename().string();
			uint32_t idx = fullName.find_last_of('.');
			int key = atoi(fullName.substr(idx - 1, 1).c_str());
			OutputDebugString(("[DBG] [CUBE]   File: [" + fullName + "]-[" + std::to_string(key) + "]").c_str());
			keys.push_back(key);

			wchar_t wTexName[MAX_TEXTURE_NAME];
			size_t len = 0;
			mbstowcs_s(&len, wTexName, MAX_TEXTURE_NAME, fullName.c_str(), MAX_TEXTURE_NAME);
			std::wstring wstr(wTexName);
			temp.push_back(wstr);
		}
	}

	// Sort the entries by their last digit before the extension:
	for (uint32_t i = 0; i < temp.size(); i++) {
		for (uint32_t j = 0; j < keys.size(); j++) {
			if (keys[j] == i) {
				result.push_back(temp[j]);
				break;
			}
		}
	}

	return result;
}

bool LoadCubeMap(DeviceResources* deviceResources,
	const std::string path,
	const float mipResolution, float* texResolution,
	ID3D11Texture2D** cubeTexture,
	ID3D11ShaderResourceView** cubeTextureSRV,
	float* diffuseMipLevel_out)
{
	HRESULT res = S_OK;
	auto& resources = deviceResources;
	auto& device = deviceResources->_d3dDevice;
	auto& context = deviceResources->_d3dDeviceContext;
	ID3D11Texture2D* cubeFace = nullptr;

	if (path.size() == 0)
		return false;

	std::vector<std::wstring> fileNames = ListFiles(path.c_str());
	const bool cubeMapComplete = (fileNames.size() == 6);
	if (!cubeMapComplete)
		return false;
	OutputDebugString(("[DBG] [CUBE] Loading files under [" + path + "]").c_str());

	D3D11_BOX box;
	box.left = 0;
	box.top = 0;
	box.front = 0;
	box.right = 1; // Will be defined later
	box.bottom = 1; // Will be defined later
	box.back = 1;

	// 0: Right
	// 1: Left
	// 2: Top
	// 3: Down
	// 4: Fwd
	// 5: Back
	int totalMipLevels = 0;
	UINT size = 0;
	*diffuseMipLevel_out = 0;
	for (uint32_t cubeFaceIdx = 0; cubeFaceIdx < fileNames.size(); cubeFaceIdx++)
	{
		HANDLE handle;
		HRESULT res = ExeLoadTexture(fileNames[cubeFaceIdx], &handle);

		if (SUCCEEDED(res))
		{
			res = device->OpenSharedResource(handle, IID_PPV_ARGS(&cubeFace));
			ExeLoadTextureRelease();
		}

		if (SUCCEEDED(res))
		{
			// When the first face of the cube is loaded, we get the size of the texture
			// and we use that to re-create the cubeTexture and cubeTextureSRV:
			if (cubeFaceIdx == 0)
			{
				D3D11_TEXTURE2D_DESC desc;
				cubeFace->GetDesc(&desc);
				size = desc.Width;
				*texResolution = (float)size;
				box.right = box.bottom = size;
				totalMipLevels = desc.MipLevels;

				*diffuseMipLevel_out = FindMipLevel(*texResolution, mipResolution);

				D3D11_TEXTURE2D_DESC cubeDesc = {};
				D3D11_SHADER_RESOURCE_VIEW_DESC cubeSRVDesc = {};
				cubeDesc.Width = size;
				cubeDesc.Height = size;
				// Use totalMipLevels here if you want to copy each level manually. See the loop below...
				cubeDesc.MipLevels = 0;
				cubeDesc.ArraySize = 6;
				cubeDesc.Format = desc.Format; // Use the texture's format
				cubeDesc.Usage = D3D11_USAGE_DEFAULT;
				cubeDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
				cubeDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE | D3D11_RESOURCE_MISC_GENERATE_MIPS;
				cubeDesc.CPUAccessFlags = 0;
				cubeDesc.SampleDesc.Count = 1;
				cubeDesc.SampleDesc.Quality = 0;

				cubeSRVDesc.Format = cubeDesc.Format;
				cubeSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
				// Use totalMipLevels here if you want to copy each level manually. See the loop below...
				cubeSRVDesc.TextureCube.MipLevels = -1;
				cubeSRVDesc.TextureCube.MostDetailedMip = 0;

				if (*cubeTexture != nullptr) (*cubeTexture)->Release();
				if (*cubeTextureSRV != nullptr) (*cubeTextureSRV)->Release();

				res = device->CreateTexture2D(&cubeDesc, nullptr, cubeTexture);
				if (FAILED(res))
				{
					OutputDebugString(("[DBG] [CUBE] FAILED when creating cubeTexture: 0x" + int_to_hex(res)).c_str());
					return false;
				}

				res = device->CreateShaderResourceView(*cubeTexture, &cubeSRVDesc, cubeTextureSRV);
				if (FAILED(res))
				{
					OutputDebugString(("[DBG] [CUBE] FAILED to create cubeTextureSRV: 0x" + int_to_hex(res)).c_str());
					return false;
				}
			}

			// The destination subresource is an array, so we need to use cubeFaceIdx...
			UINT dstSubResourceIdx = D3D11CalcSubresource(0, cubeFaceIdx, totalMipLevels);
			// ... but the source is not an array, so the array index is always 0:
			UINT srcSubResourceIdx = D3D11CalcSubresource(0, 0, totalMipLevels);
			context->CopySubresourceRegion(*cubeTexture, dstSubResourceIdx, 0, 0, 0,
				cubeFace, srcSubResourceIdx, nullptr);

			/*
			// This code works, but I think it's more work copying all the mip levels than
			// having the GPU generate the mips on the final cubemap.
			for (int mipLevel = 0; mipLevel < totalMipLevels; mipLevel++)
			{
				// The destination subresource is an array, so we need to use cubeFaceIdx...
				UINT dstSubResourceIdx = D3D11CalcSubresource(mipLevel, cubeFaceIdx, totalMipLevels);
				// ... but the source is not an array, so the array index is always 0:
				UINT srcSubResourceIdx = D3D11CalcSubresource(mipLevel, 0, totalMipLevels);
				context->CopySubresourceRegion(*cubeTexture, dstSubResourceIdx, 0, 0, 0,
					cubeFace, srcSubResourceIdx, nullptr);
			}
			*/
		}
		else
		{
			OutputDebugString(("[DBG] [CUBE] COULD NOT LOAD CUBEFACE [" + std::to_string(cubeFaceIdx) + "]. Error: 0x" + int_to_hex(res)).c_str());
			return false;
		}
	}

	context->GenerateMips(*cubeTextureSRV);
	//DirectX::SaveDDSTextureToFile(context, *cubeTexture, L"C:\\Temp\\_cubeTexture.dds");
	return true;
}

inline bool IsInMap(const std::map<int, bool>& map, int key)
{
	return (map.find(key) != map.end());
}

inline int MakeKeyFromGroupIdImageId(int groupId, int imageId)
{
	return (groupId << 16) | (imageId);
}

void PopulateStarfieldMap()
{
	// Populate the standard starfield map.
	// This code is not an exhaustive list of starfields. Custom DAT files can be
	// added (TFTC does this), but this is a good starting point for "standard" XWAU.
	g_StarfieldGroupIdImageIdMap.clear();
	g_StarfieldGroupIdImageIdMap[MakeKeyFromGroupIdImageId(6104, 0)] = true;

	g_StarfieldGroupIdImageIdMap[MakeKeyFromGroupIdImageId(6079, 2)] = true;
	g_StarfieldGroupIdImageIdMap[MakeKeyFromGroupIdImageId(6079, 3)] = true;
	g_StarfieldGroupIdImageIdMap[MakeKeyFromGroupIdImageId(6079, 4)] = true;
	g_StarfieldGroupIdImageIdMap[MakeKeyFromGroupIdImageId(6079, 5)] = true;
	g_StarfieldGroupIdImageIdMap[MakeKeyFromGroupIdImageId(6079, 6)] = true;

	g_StarfieldGroupIdImageIdMap[MakeKeyFromGroupIdImageId(6034, 3)] = true;
	g_StarfieldGroupIdImageIdMap[MakeKeyFromGroupIdImageId(6034, 4)] = true;
	g_StarfieldGroupIdImageIdMap[MakeKeyFromGroupIdImageId(6034, 5)] = true;
	g_StarfieldGroupIdImageIdMap[MakeKeyFromGroupIdImageId(6034, 6)] = true;

	g_StarfieldGroupIdImageIdMap[MakeKeyFromGroupIdImageId(6042, 1)] = true;
	g_StarfieldGroupIdImageIdMap[MakeKeyFromGroupIdImageId(6042, 2)] = true;
	g_StarfieldGroupIdImageIdMap[MakeKeyFromGroupIdImageId(6042, 3)] = true;
	g_StarfieldGroupIdImageIdMap[MakeKeyFromGroupIdImageId(6042, 4)] = true;
	g_StarfieldGroupIdImageIdMap[MakeKeyFromGroupIdImageId(6042, 5)] = true;
	g_StarfieldGroupIdImageIdMap[MakeKeyFromGroupIdImageId(6042, 6)] = true;

	g_StarfieldGroupIdImageIdMap[MakeKeyFromGroupIdImageId(6094, 1)] = true;
	g_StarfieldGroupIdImageIdMap[MakeKeyFromGroupIdImageId(6094, 2)] = true; // Cap
	g_StarfieldGroupIdImageIdMap[MakeKeyFromGroupIdImageId(6094, 3)] = true;
	g_StarfieldGroupIdImageIdMap[MakeKeyFromGroupIdImageId(6094, 4)] = true; // Cap
	g_StarfieldGroupIdImageIdMap[MakeKeyFromGroupIdImageId(6094, 5)] = true;
	g_StarfieldGroupIdImageIdMap[MakeKeyFromGroupIdImageId(6094, 6)] = true; // Cap

	g_StarfieldGroupIdImageIdMap[MakeKeyFromGroupIdImageId(6083, 2)] = true;
	g_StarfieldGroupIdImageIdMap[MakeKeyFromGroupIdImageId(6083, 3)] = true; // Cap
	g_StarfieldGroupIdImageIdMap[MakeKeyFromGroupIdImageId(6083, 5)] = true;

	g_StarfieldGroupIdImageIdMap[MakeKeyFromGroupIdImageId(6084, 1)] = true;
	g_StarfieldGroupIdImageIdMap[MakeKeyFromGroupIdImageId(6084, 2)] = true; // Cap
	g_StarfieldGroupIdImageIdMap[MakeKeyFromGroupIdImageId(6084, 4)] = true;
	g_StarfieldGroupIdImageIdMap[MakeKeyFromGroupIdImageId(6084, 6)] = true; // Cap

	g_StarfieldGroupIdImageIdMap[MakeKeyFromGroupIdImageId(6094, 2)] = true; // Cap

	g_StarfieldGroupIdImageIdMap[MakeKeyFromGroupIdImageId(6104, 5)] = true; // Cap
}

void PopulateBackdropsMap(std::string& list, std::map<int, bool>& map)
{
	std::string token;
	size_t pos, idx;

	OutputDebugString(("[DBG] [CUBE] Parsing Backdrops: [" + list + "]").c_str());

	do
	{
		pos = list.find(';');
		// Get the next token:
		token = list.substr(0, pos);

		// Process the token
		idx = token.find('-');
		if (idx != std::string::npos)
		{
			const int groupId = atoi(token.substr(0, idx).c_str());
			const int imageId = atoi(token.substr(idx + 1, token.size() - (idx + 1)).c_str());
			const int key = MakeKeyFromGroupIdImageId(groupId, imageId);
			map[key] = true;
			OutputDebugString(("[DBG] [CUBE] -- Disabled: " + std::to_string(groupId) + "-" + std::to_string(imageId)).c_str());
		}
		else if (token == "ALL" || token == "all")
		{
			OutputDebugString("[DBG] [CUBE] Disabling ALL backdrops!");
			map[-1] = true;
			return;
		}

		// If we haven't reached the end of the string, then erase
		// the current token and repeat
		if (pos != std::string::npos)
			list.erase(0, pos + 1);
	} while (pos != std::string::npos);
}

void ParseCubeMapMissionIni(const std::vector<std::string>& lines)
{
	g_DisabledGroupIdImageIdMap.clear();
	g_EnabledOvrGroupIdImageIdMap.clear();

	std::string DisabledBackdropList = GetFileKeyValue(lines, "DisabledBackdrops");
	PopulateBackdropsMap(DisabledBackdropList, g_DisabledGroupIdImageIdMap);

	std::string EnabledBackdropList = GetFileKeyValue(lines, "EnabledBackdrops");
	PopulateBackdropsMap(EnabledBackdropList, g_EnabledOvrGroupIdImageIdMap);

	g_CubeMaps.allRegionsSpecular = GetFileKeyValueFloat(lines, "AllRegionsSpecular", 0.55f);
	g_CubeMaps.allRegionsAmbientInt = GetFileKeyValueFloat(lines, "AllRegionsAmbientInt", 0.50f);
	g_CubeMaps.allRegionsAmbientMin = GetFileKeyValueFloat(lines, "AllRegionsAmbientMin", 0.01f);
	g_CubeMaps.allRegionsAngX = GetFileKeyValueFloat(lines, "AllRegionsRotationX", 0.0f);
	g_CubeMaps.allRegionsAngY = GetFileKeyValueFloat(lines, "AllRegionsRotationY", 0.0f);
	g_CubeMaps.allRegionsAngZ = GetFileKeyValueFloat(lines, "AllRegionsRotationZ", 0.0f);

	g_CubeMaps.allRegionsOvrAngX = GetFileKeyValueFloat(lines, "AllRegionsOverlayRotationX", g_CubeMaps.allRegionsAngX);
	g_CubeMaps.allRegionsOvrAngY = GetFileKeyValueFloat(lines, "AllRegionsOverlayRotationY", g_CubeMaps.allRegionsAngY);
	g_CubeMaps.allRegionsOvrAngZ = GetFileKeyValueFloat(lines, "AllRegionsOverlayRotationZ", g_CubeMaps.allRegionsAngZ);

	g_CubeMaps.allRegionsMipRes = GetFileKeyValueFloat(lines, "AllRegionsReflectionRes", 8.0f);
	g_CubeMaps.allRegionsDiffuseMipLevel = FindMipLevel(g_CubeMaps.allRegionsTexRes, g_CubeMaps.allRegionsMipRes);

	const char* regionSpecNames[MAX_MISSION_REGIONS] = { "Region0Specular",   "Region1Specular",   "Region2Specular",   "Region3Specular" };
	const char* regionAmbientIntNames[MAX_MISSION_REGIONS] = { "Region0AmbientInt", "Region1AmbientInt", "Region2AmbientInt", "Region3AmbientInt" };
	const char* regionAmbientMinNames[MAX_MISSION_REGIONS] = { "Region0AmbientMin", "Region1AmbientMin", "Region2AmbientMin", "Region3AmbientMin" };

	const char* regionAngX[MAX_MISSION_REGIONS] = { "Region0RotationX", "Region1RotationX", "Region2RotationX", "Region3RotationX" };
	const char* regionAngY[MAX_MISSION_REGIONS] = { "Region0RotationY", "Region1RotationY", "Region2RotationY", "Region3RotationY" };
	const char* regionAngZ[MAX_MISSION_REGIONS] = { "Region0RotationZ", "Region1RotationZ", "Region2RotationZ", "Region3RotationZ" };

	const char* regionOvrAngX[MAX_MISSION_REGIONS] = {
		"Region0OverlayRotationX", "Region1OverlayRotationX", "Region2OverlayRotationX", "Region3OverlayRotationX" };
	const char* regionOvrAngY[MAX_MISSION_REGIONS] = {
		"Region0OverlayRotationY", "Region1OverlayRotationY", "Region2OverlayRotationY", "Region3OverlayRotationY" };
	const char* regionOvrAngZ[MAX_MISSION_REGIONS] = {
		"Region0OverlayRotationZ", "Region1OverlayRotationZ", "Region2OverlayRotationZ", "Region3OverlayRotationZ" };
	const char* regionMipRes[MAX_MISSION_REGIONS] = { "Region0ReflectionRes", "Region1ReflectionRes", "Region2ReflectionRes", "Region3ReflectionRes" };

	for (int i = 0; i < MAX_MISSION_REGIONS; i++)
	{
		g_CubeMaps.regionSpecular[i] = GetFileKeyValueFloat(lines, regionSpecNames[i], 0.70f);
		g_CubeMaps.regionAmbientInt[i] = GetFileKeyValueFloat(lines, regionAmbientIntNames[i], 0.15f);
		g_CubeMaps.regionAmbientMin[i] = GetFileKeyValueFloat(lines, regionAmbientMinNames[i], 0.01f);

		g_CubeMaps.regionAngX[i] = GetFileKeyValueFloat(lines, regionAngX[i], 0.0f);
		g_CubeMaps.regionAngY[i] = GetFileKeyValueFloat(lines, regionAngY[i], 0.0f);
		g_CubeMaps.regionAngZ[i] = GetFileKeyValueFloat(lines, regionAngZ[i], 0.0f);

		g_CubeMaps.regionOvrAngX[i] = GetFileKeyValueFloat(lines, regionOvrAngX[i], g_CubeMaps.regionAngX[i]);
		g_CubeMaps.regionOvrAngY[i] = GetFileKeyValueFloat(lines, regionOvrAngY[i], g_CubeMaps.regionAngY[i]);
		g_CubeMaps.regionOvrAngZ[i] = GetFileKeyValueFloat(lines, regionOvrAngZ[i], g_CubeMaps.regionAngZ[i]);

		g_CubeMaps.regionMipRes[i] = GetFileKeyValueFloat(lines, regionMipRes[i], 16.0f);
		g_CubeMaps.regionDiffuseMipLevel[i] = FindMipLevel(g_CubeMaps.regionTexRes[i], g_CubeMaps.regionMipRes[i]);
	}
}

/// <summary>
/// Check if the current mission has changed, and if so, load new cube maps and set
/// global flags (g_bRenderCubeMapInThisRegion) and SRVs (g_cubeTextureSRV).
/// This is currently called from D3dOptCreateTextureColorLight() while a mission
/// is being loaded.
/// </summary>
void LoadMissionCubeMaps(DeviceResources* deviceResources)
{
	g_CubeMaps.bEnabled = g_config.EnableCubeMaps;

	static std::string _mission;
	static int _missionIndex = 0;

	std::string xwaMissionFileName = (const char*)0x06002E8;
	const int missionFileNameIndex = *(int*)0x06002E4;
	const bool s_XwaIsInConcourse = *(int*)0x005FFD9C != 0;

	if (!g_CubeMaps.bEnabled || s_XwaIsInConcourse)
	{
		return;
	}

	if ((missionFileNameIndex == 0) ? (_mission == xwaMissionFileName) : (_missionIndex == missionFileNameIndex))
	{
		return;
	}

	_mission = xwaMissionFileName;
	_missionIndex = missionFileNameIndex;

	PopulateStarfieldMap();

	const std::string path = GetStringWithoutExtension(_mission);
	auto lines = GetFileLines(path + "_SkyBoxes.txt");
	if (!lines.size())
	{
		lines = GetFileLines(path + ".ini", "SkyBoxes");
	}

	static ID3D11Texture2D* cubeTextures[MAX_MISSION_REGIONS] = { nullptr, nullptr, nullptr, nullptr };
	static ID3D11Texture2D* cubeTexturesIllum[MAX_MISSION_REGIONS] = { nullptr, nullptr, nullptr, nullptr };
	static ID3D11Texture2D* cubeTexturesOvr[MAX_MISSION_REGIONS] = { nullptr, nullptr, nullptr, nullptr };
	static ID3D11Texture2D* allRegionsCubeTexture = nullptr;
	static ID3D11Texture2D* allRegionsIllumCubeTexture = nullptr;
	static ID3D11Texture2D* allRegionsOvrCubeTexture = nullptr;

	HRESULT res = S_OK;
	auto& resources = deviceResources;
	auto& device = deviceResources->_d3dDevice;
	auto& context = deviceResources->_d3dDeviceContext;

	// Disable all cubemaps as soon as a new mission is loaded.
	// We'll re-enable them if we find the relevant settings in the .ini file.
	g_CubeMaps.bRenderAllRegions = false;
	for (int i = 0; i < MAX_MISSION_REGIONS; i++) g_CubeMaps.bRenderInThisRegion[i] = false;

	OutputDebugString(("[DBG] [CUBE] Loading ini: " + _mission).c_str());

	if (!lines.size())
	{
		OutputDebugString("[DBG] [CUBE] --- g_bRenderCubeMapInThisRegion = false (1)");
		return;
	}

	const std::string illumStr = "_illum";
	const std::string ovrStr = "_overlay";
	std::string allRegionsPath = GetFileKeyValue(lines, "AllRegions");
	std::string allRegionsIllumPath = GetFileKeyValue(lines, "AllRegionsIllum",
		allRegionsPath.size() > 0 ? allRegionsPath + illumStr : "");
	std::string allRegionsOvrPath = GetFileKeyValue(lines, "AllRegionsOverlay",
		allRegionsPath.size() > 0 ? allRegionsPath + ovrStr : "");
	std::string regionPath[4];
	std::string regionIllumPath[4];
	std::string regionOvrPath[4];
	regionPath[0] = GetFileKeyValue(lines, "Region0");
	regionPath[1] = GetFileKeyValue(lines, "Region1");
	regionPath[2] = GetFileKeyValue(lines, "Region2");
	regionPath[3] = GetFileKeyValue(lines, "Region3");
	regionIllumPath[0] = GetFileKeyValue(lines, "Region0Illum",
		regionPath[0].size() > 0 ? regionPath[0] + illumStr : "");
	regionIllumPath[1] = GetFileKeyValue(lines, "Region1Illum",
		regionPath[1].size() > 0 ? regionPath[1] + illumStr : "");
	regionIllumPath[2] = GetFileKeyValue(lines, "Region2Illum",
		regionPath[2].size() > 0 ? regionPath[2] + illumStr : "");
	regionIllumPath[3] = GetFileKeyValue(lines, "Region3Illum",
		regionPath[3].size() > 0 ? regionPath[3] + illumStr : "");

	regionOvrPath[0] = GetFileKeyValue(lines, "Region0Overlay",
		regionPath[0].size() > 0 ? regionPath[0] + ovrStr : "");
	regionOvrPath[1] = GetFileKeyValue(lines, "Region1Overlay",
		regionPath[1].size() > 0 ? regionPath[1] + ovrStr : "");
	regionOvrPath[2] = GetFileKeyValue(lines, "Region2Overlay",
		regionPath[2].size() > 0 ? regionPath[2] + ovrStr : "");
	regionOvrPath[3] = GetFileKeyValue(lines, "Region3Overlay",
		regionPath[3].size() > 0 ? regionPath[3] + ovrStr : "");

	ParseCubeMapMissionIni(lines);

	if (allRegionsPath.size() > 0)
		g_CubeMaps.bRenderAllRegions = LoadCubeMap(deviceResources, allRegionsPath,
			g_CubeMaps.allRegionsMipRes, &g_CubeMaps.allRegionsTexRes,
			&allRegionsCubeTexture, &g_CubeMaps.allRegionsSRV, &g_CubeMaps.allRegionsDiffuseMipLevel);

	if (allRegionsIllumPath.size() > 0)
		g_CubeMaps.bAllRegionsIllum = LoadCubeMap(deviceResources, allRegionsIllumPath,
			g_CubeMaps.allRegionsIllumMipRes, &g_CubeMaps.allRegionsIllumTexRes,
			&allRegionsIllumCubeTexture, &g_CubeMaps.allRegionsIllumSRV, &g_CubeMaps.allRegionsIllumDiffuseMipLevel);

	float dummy1, dummy2;
	if (allRegionsOvrPath.size() > 0)
		g_CubeMaps.bAllRegionsOvr = LoadCubeMap(deviceResources, allRegionsOvrPath, 1024, &dummy1,
			&allRegionsOvrCubeTexture, &g_CubeMaps.allRegionsOvrSRV, &dummy2);

	for (int i = 0; i < MAX_MISSION_REGIONS; i++)
	{
		if (regionPath[i].size() > 0)
			g_CubeMaps.bRenderInThisRegion[i] = LoadCubeMap(deviceResources, regionPath[i],
				g_CubeMaps.regionMipRes[i], &(g_CubeMaps.regionTexRes[i]),
				&cubeTextures[i], &g_CubeMaps.regionSRV[i], &g_CubeMaps.regionDiffuseMipLevel[i]);

		if (regionIllumPath[i].size() > 0)
			g_CubeMaps.bRenderIllumInThisRegion[i] = LoadCubeMap(deviceResources, regionIllumPath[i],
				g_CubeMaps.regionIllumMipRes[i], &(g_CubeMaps.regionIllumTexRes[i]),
				&cubeTexturesIllum[i], &g_CubeMaps.regionIllumSRV[i], &g_CubeMaps.regionIllumDiffuseMipLevel[i]);

		if (regionIllumPath[i].size() > 0)
			g_CubeMaps.bRenderOvrInThisRegion[i] = LoadCubeMap(deviceResources, regionOvrPath[i], 1024, &dummy1,
				&cubeTexturesOvr[i], &g_CubeMaps.regionOvrSRV[i], &dummy2);
	}

	ReloadCubeMapsResources(deviceResources);
}

struct CubeMapsConstants
{
	float viewportScale[4];
	float projectionValue1;
	float projectionValue2;
	float projectionDeltaX;
	float projectionDeltaY;
	float projectionParameterA;
	float projectionParameterB;
	float projectionParameterC;
	float projectionParameterD;
	float transformView[16];
	Matrix4 transformWorld;
};

class CubeMapsData
{
public:
	int s_region;
	bool s_drawCubeMap;
	Matrix4 s_cubeMapRot;
	bool s_drawOvrCubeMap;
	Matrix4 s_ovrCubeMapRot;

	ComPtr<ID3D11Buffer> s_constantBuffer;
	ComPtr<ID3D11RasterizerState> s_rasterizerState;
	ComPtr<ID3D11SamplerState> s_samplerState;
	ComPtr<ID3D11BlendState> s_blendState;
	ComPtr<ID3D11DepthStencilState> s_depthState;
	ComPtr<ID3D11InputLayout> s_inputLayout;
	ComPtr<ID3D11VertexShader> s_vertexShader;
	ComPtr<ID3D11PixelShader> s_pixelShader;
	ComPtr<ID3D11Buffer> s_vertexBuffer;
	ComPtr<ID3D11Buffer> s_indexBuffer;
};

CubeMapsData* g_cubeMapsData = nullptr;

void ReleaseCubeMaps()
{
	if (g_cubeMapsData)
	{
		delete g_cubeMapsData;
		g_cubeMapsData = nullptr;
	}
}

void RenderDefaultBackground(DeviceResources* deviceResources)
{
	auto& resources = deviceResources;
	auto& device = resources->_d3dDevice;
	auto& context = resources->_d3dDeviceContext;

	if (g_bDefaultStarfieldRendered)
	{
		return;
	}

	g_bDefaultStarfieldRendered = true;

	if (!g_cubeMapsData)
	{
		return;
	}

	g_cubeMapsData->s_region = GetCurrentCubeMapRegion();

	RenderDefaultBackground(deviceResources, &g_cubeMapsData->s_drawCubeMap, &g_cubeMapsData->s_cubeMapRot, &g_cubeMapsData->s_drawOvrCubeMap, &g_cubeMapsData->s_ovrCubeMapRot);

	resources->BeginAnnotatedEvent(L"RenderDefaultBackground");
	RenderCubeMaps(deviceResources);
	resources->EndAnnotatedEvent();
}

static void ReloadCubeMapsResources(DeviceResources* deviceResources)
{
	auto& resources = deviceResources;
	auto& device = resources->_d3dDevice;
	auto& context = resources->_d3dDeviceContext;

	ReleaseCubeMaps();
	g_cubeMapsData = new CubeMapsData();

	// Constant buffer
	CD3D11_BUFFER_DESC _constantBufferDesc(sizeof(CubeMapsConstants), D3D11_BIND_CONSTANT_BUFFER);
	device->CreateBuffer(&_constantBufferDesc, nullptr, &g_cubeMapsData->s_constantBuffer);

	// Rasterizer state
	D3D11_RASTERIZER_DESC rsDesc{};
	rsDesc.FillMode = D3D11_FILL_SOLID;
	rsDesc.CullMode = D3D11_CULL_BACK;
	rsDesc.FrontCounterClockwise = FALSE;
	rsDesc.DepthBias = 0;
	rsDesc.DepthBiasClamp = 0.0f;
	rsDesc.SlopeScaledDepthBias = 0.0f;
	rsDesc.DepthClipEnable = FALSE;
	rsDesc.ScissorEnable = FALSE;
	rsDesc.MultisampleEnable = TRUE;
	rsDesc.AntialiasedLineEnable = FALSE;
	device->CreateRasterizerState(&rsDesc, &g_cubeMapsData->s_rasterizerState);

	// Sampler state
	D3D11_SAMPLER_DESC samplerDesc{};
	samplerDesc.Filter = resources->_useAnisotropy ? D3D11_FILTER_ANISOTROPIC : D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.MaxAnisotropy = resources->_useAnisotropy ? resources->GetMaxAnisotropy() : 1;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = FLT_MAX;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.BorderColor[0] = 0.0f;
	samplerDesc.BorderColor[1] = 0.0f;
	samplerDesc.BorderColor[2] = 0.0f;
	samplerDesc.BorderColor[3] = 0.0f;
	device->CreateSamplerState(&samplerDesc, &g_cubeMapsData->s_samplerState);

	// Blend state
	D3D11_BLEND_DESC blendDesc{};
	blendDesc.AlphaToCoverageEnable = FALSE;
	blendDesc.IndependentBlendEnable = FALSE;
	blendDesc.RenderTarget[0].BlendEnable = FALSE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	device->CreateBlendState(&blendDesc, &g_cubeMapsData->s_blendState);

	// Depth stencil state
	D3D11_DEPTH_STENCIL_DESC depthDesc{};
	depthDesc.DepthEnable = FALSE;
	depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	depthDesc.StencilEnable = FALSE;
	device->CreateDepthStencilState(&depthDesc, &g_cubeMapsData->s_depthState);

	// Vertex shader
	device->CreateVertexShader(g_CubeMapsVertexShader, sizeof(g_CubeMapsVertexShader), nullptr, &g_cubeMapsData->s_vertexShader);

	// Input layout
	const D3D11_INPUT_ELEMENT_DESC vertexLayoutDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	device->CreateInputLayout(vertexLayoutDesc, ARRAYSIZE(vertexLayoutDesc), g_CubeMapsVertexShader, sizeof(g_CubeMapsVertexShader), &g_cubeMapsData->s_inputLayout);

	// Pixel shader
	device->CreatePixelShader(g_CubeMapsPixelShader, sizeof(g_CubeMapsPixelShader), nullptr, &g_cubeMapsData->s_pixelShader);

	// Vertex buffer
	Vector4 vertices[] =
	{
		{-1.0f, 1.0f, -1.0f, 1.0f},
		{1.0f, 1.0f, -1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f, 1.0f},
		{-1.0f, 1.0f, 1.0f, 1.0f},
		{-1.0f, -1.0f, -1.0f, 1.0f},
		{1.0f, -1.0f, -1.0f, 1.0f},
		{1.0f, -1.0f, 1.0f, 1.0f},
		{-1.0f, -1.0f, 1.0f, 1.0f},
	};
	CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(vertices), D3D11_BIND_VERTEX_BUFFER);
	D3D11_SUBRESOURCE_DATA vertexBufferData{ vertices, 0, 0 };
	device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &g_cubeMapsData->s_vertexBuffer);

	// Index buffer
	WORD indices[] =
	{
		3, 1, 0,
		2, 1, 3,
		0, 5, 4,
		1, 5, 0,
		3, 4, 7,
		0, 4, 3,
		1, 6, 5,
		2, 6, 1,
		2, 7, 6,
		3, 7, 2,
		6, 4, 5,
		7, 4, 6,
	};
	CD3D11_BUFFER_DESC indexBufferDesc(sizeof(indices), D3D11_BIND_INDEX_BUFFER);
	D3D11_SUBRESOURCE_DATA indexBufferData{ indices, 0, 0 };
	device->CreateBuffer(&indexBufferDesc, &indexBufferData, &g_cubeMapsData->s_indexBuffer);
}

void GetViewport(DeviceResources* deviceResources, D3D11_VIEWPORT* viewport)
{
	UINT w;
	UINT h;

	if (g_config.AspectRatioPreserved)
	{
		if (deviceResources->_backbufferHeight * deviceResources->_displayWidth <= deviceResources->_backbufferWidth * deviceResources->_displayHeight)
		{
			w = deviceResources->_backbufferHeight * deviceResources->_displayWidth / deviceResources->_displayHeight;
			h = deviceResources->_backbufferHeight;
		}
		else
		{
			w = deviceResources->_backbufferWidth;
			h = deviceResources->_backbufferWidth * deviceResources->_displayHeight / deviceResources->_displayWidth;
		}
	}
	else
	{
		w = deviceResources->_backbufferWidth;
		h = deviceResources->_backbufferHeight;
	}

	UINT left = (deviceResources->_backbufferWidth - w) / 2;
	UINT top = (deviceResources->_backbufferHeight - h) / 2;

	viewport->TopLeftX = (float)left;
	viewport->TopLeftY = (float)top;
	viewport->Width = (float)w;
	viewport->Height = (float)h;
	viewport->MinDepth = D3D11_MIN_DEPTH;
	viewport->MaxDepth = D3D11_MAX_DEPTH;
}

void GetViewportScale(DeviceResources* deviceResources, float* viewportScale)
{
	float scale;

	if (deviceResources->_frontbufferSurface == nullptr)
	{
		scale = 1.0f;
	}
	else
	{
		if (deviceResources->_backbufferHeight * deviceResources->_displayWidth <= deviceResources->_backbufferWidth * deviceResources->_displayHeight)
		{
			scale = (float)deviceResources->_backbufferHeight / (float)deviceResources->_displayHeight;
		}
		else
		{
			scale = (float)deviceResources->_backbufferWidth / (float)deviceResources->_displayWidth;
		}

		scale *= g_config.Concourse3DScale;
	}

	viewportScale[0] = 2.0f / (float)deviceResources->_displayWidth;
	viewportScale[1] = -2.0f / (float)deviceResources->_displayHeight;
	viewportScale[2] = scale;
	viewportScale[3] = 0.0f;
}

static void RenderCubeMaps(DeviceResources* deviceResources)
{
	auto& resources = deviceResources;
	auto& device = resources->_d3dDevice;
	auto& context = resources->_d3dDeviceContext;

	if (!g_cubeMapsData)
	{
		return;
	}

	ComPtr<ID3D11Buffer> oldVSConstantBuffer;
	ComPtr<ID3D11Buffer> oldPSConstantBuffer;
	ComPtr<ID3D11ShaderResourceView> oldVSSRV[3];
	context->VSGetConstantBuffers(0, 1, oldVSConstantBuffer.GetAddressOf());
	context->PSGetConstantBuffers(0, 1, oldPSConstantBuffer.GetAddressOf());
	context->VSGetShaderResources(0, 3, oldVSSRV[0].GetAddressOf());

	context->VSSetConstantBuffers(0, 1, g_cubeMapsData->s_constantBuffer.GetAddressOf());
	context->PSSetConstantBuffers(0, 1, g_cubeMapsData->s_constantBuffer.GetAddressOf());
	resources->InitRasterizerState(g_cubeMapsData->s_rasterizerState);
	resources->InitSamplerState(g_cubeMapsData->s_samplerState.GetAddressOf(), nullptr);
	resources->InitBlendState(g_cubeMapsData->s_blendState, nullptr);
	resources->InitDepthStencilState(g_cubeMapsData->s_depthState, nullptr);

	D3D11_VIEWPORT viewport{};
	GetViewport(deviceResources, &viewport);
	resources->InitViewport(&viewport);

	resources->InitTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	resources->InitInputLayout(g_cubeMapsData->s_inputLayout);
	resources->InitVertexShader(g_cubeMapsData->s_vertexShader);
	resources->InitPixelShader(g_cubeMapsData->s_pixelShader);

	UINT vertexBufferStride = sizeof(Vector4);
	UINT vertexBufferOffset = 0;
	resources->InitVertexBuffer(g_cubeMapsData->s_vertexBuffer.GetAddressOf(), &vertexBufferStride, &vertexBufferOffset);
	resources->InitIndexBuffer(g_cubeMapsData->s_indexBuffer, false);

	CubeMapsConstants constants{};
	GetViewportScale(deviceResources, constants.viewportScale);
	constants.projectionParameterA = g_config.ProjectionParameterA;
	constants.projectionParameterB = g_config.ProjectionParameterB;
	constants.projectionParameterC = g_config.ProjectionParameterC;
	constants.projectionParameterD = 0;
	constants.projectionValue1 = *(float*)0x08B94CC;
	constants.projectionValue2 = *(float*)0x05B46B4;
	constants.projectionDeltaX = *(float*)0x08C1600 + *(float*)0x0686ACC;
	constants.projectionDeltaY = *(float*)0x080ACF8 + *(float*)0x07B33C0 + *(float*)0x064D1AC;
	constants.transformView[0] = *(float*)0x007B4BEC;
	constants.transformView[1] = *(float*)0x007B6FF8;
	constants.transformView[2] = *(float*)0x007B33DC;
	constants.transformView[3] = 0.0f;
	constants.transformView[4] = *(float*)0x007B4BE8;
	constants.transformView[5] = *(float*)0x007B6FF0;
	constants.transformView[6] = *(float*)0x007B33D8;
	constants.transformView[7] = 0.0f;
	constants.transformView[8] = *(float*)0x007B4BF4;
	constants.transformView[9] = *(float*)0x007B33D4;
	constants.transformView[10] = *(float*)0x007B4BE4;
	constants.transformView[11] = 0.0f;
	constants.transformView[12] = 0.0f;
	constants.transformView[13] = 0.0f;
	constants.transformView[14] = 0.0f;
	constants.transformView[15] = 1.0f;

	if (g_cubeMapsData->s_drawCubeMap)
	{
		constants.transformWorld = g_cubeMapsData->s_cubeMapRot;
		context->UpdateSubresource(g_cubeMapsData->s_constantBuffer, 0, nullptr, &constants, 0, 0);

		if (g_cubeMapsData->s_region != -1)
		{
			resources->InitPSShaderResourceView(g_CubeMaps.regionSRV[g_cubeMapsData->s_region], nullptr);
		}
		else if (g_CubeMaps.bRenderAllRegions)
		{
			resources->InitPSShaderResourceView(g_CubeMaps.allRegionsSRV, nullptr);
		}
		else
		{
			resources->InitPSShaderResourceView(nullptr, nullptr);
		}

		context->DrawIndexed(36, 0, 0);
	}

	if (g_cubeMapsData->s_drawOvrCubeMap)
	{
		constants.transformWorld = g_cubeMapsData->s_ovrCubeMapRot;
		context->UpdateSubresource(g_cubeMapsData->s_constantBuffer, 0, nullptr, &constants, 0, 0);

		if (g_cubeMapsData->s_region != -1)
		{
			resources->InitPSShaderResourceView(g_CubeMaps.regionOvrSRV[g_cubeMapsData->s_region], nullptr);
		}
		else if (g_CubeMaps.bRenderAllRegions)
		{
			resources->InitPSShaderResourceView(g_CubeMaps.allRegionsOvrSRV, nullptr);
		}
		else
		{
			resources->InitPSShaderResourceView(nullptr, nullptr);
		}

		context->DrawIndexed(36, 0, 0);
	}

	context->VSSetConstantBuffers(0, 1, oldVSConstantBuffer.GetAddressOf());
	context->PSSetConstantBuffers(0, 1, oldPSConstantBuffer.GetAddressOf());
	context->VSSetShaderResources(0, 3, oldVSSRV[0].GetAddressOf());
}

enum RenderStateEnum : unsigned int
{
	RenderState_None = 0,
	RenderState_TexturePerspective = 1,
	RenderState_Dithering = 2,
	RenderState_SpecularLight = 4,
	RenderState_Antialiasing = 8,
	RenderState_SubPixelCorrection = 16,
	RenderState_SubPixelXCorrection = 32,
	RenderState_Fog = 64,
	RenderState_TextureMagLinear = 128,
	RenderState_TextureMinLinear = 256,
	RenderState_Blend = 512,
	RenderState_BlendModulateAlpha = 1024,
	RenderState_ZFunc = 2048,
	RenderState_ZWrite = 4096,
	RenderState_TextureClamp = 8192,
	RenderState_ColorRendering = 32768,
	RenderState_ZFuncEqual = 65536,
	RenderState_BlendDecal = 262144,
	RenderState_NoAlphaTest = 524288,
};

RenderStateEnum& s_XwaD3dRenderState = *(RenderStateEnum*)0x007B1CD8;

bool CubeMapsSkipBackdrop(const std::string& name)
{
	if (!g_CubeMaps.bEnabled)
	{
		return false;
	}

	const RenderStateEnum backdropRenderState = (RenderStateEnum)(RenderState_Dithering | RenderState_SubPixelCorrection | RenderState_TextureMagLinear | RenderState_TextureMinLinear | RenderState_Blend | RenderState_BlendModulateAlpha);
	bool isBackdrop = (s_XwaD3dRenderState & ~RenderState_TextureClamp) == backdropRenderState;

	if (!isBackdrop)
	{
		return false;
	}

	std::vector<std::string> values = Tokennize(name);

	if (values.size() < 3 || values[0] != "dat")
	{
		return false;
	}

	int GroupId = std::stoi(values[1]);
	int ImageId = std::stoi(values[2]);
	const int key = MakeKeyFromGroupIdImageId(GroupId, ImageId);

	// If this GroupId-ImageId is disabled...
	if (!IsInMap(g_EnabledOvrGroupIdImageIdMap, -1) && // "EnabledBackdrops = ALL" enables all backdrops
		(IsInMap(g_DisabledGroupIdImageIdMap, -1) || // Are all backdrops disabled?
			IsInMap(g_StarfieldGroupIdImageIdMap, key) || // Is it a known (default) backdrop in XWAU?
			IsInMap(g_DisabledGroupIdImageIdMap, key)) && // Is it explicitly disabled?
		// ... but is not in the enabled-override list...
		!IsInMap(g_EnabledOvrGroupIdImageIdMap, key))
		// Then this is a skippable backdrop (starfield)
	{
		return true;
	}

	return false;
}
