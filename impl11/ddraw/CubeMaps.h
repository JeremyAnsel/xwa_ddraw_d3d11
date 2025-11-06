#pragma once
#include "common.h"
#include "DeviceResources.h"
#include "Vectors.h"
#include "Matrices.h"
#include <map>

/// <summary>
/// Maps GroupId-ImageId keys to Direct3DTexture pointers.
/// Used to associate Backdrop Ids with GroupId-ImageId pairs.
/// </summary>
extern std::map<int, void*> g_GroupIdImageIdToTextureMap;

namespace STARFIELD_TYPE
{
	enum STARFIELD_TYPE
	{
		TOP = 0,
		BOTTOM,
		FRONT,
		BACK,
		LEFT,
		RIGHT,
		MAX
	};
};

class Direct3DTexture;
extern Direct3DTexture* g_StarfieldSRVs[STARFIELD_TYPE::MAX];

constexpr int MAX_MISSION_REGIONS = 4;

enum class CubeMapEditMode
{
	DISABLED,
	AZIMUTH_ELEVATION,
	LOCAL_COORDS
};

struct CubeMapData
{
	bool bEnabled = false;
	bool bRenderAllRegions = false;
	bool bAllRegionsIllum = false;
	bool bAllRegionsOvr = false;
	bool bRenderInThisRegion[MAX_MISSION_REGIONS] = { false, false, false, false };
	bool bRenderIllumInThisRegion[MAX_MISSION_REGIONS] = { false, false, false, false };
	bool bRenderOvrInThisRegion[MAX_MISSION_REGIONS] = { false, false, false, false };
	float allRegionsSpecular = 0.7f;
	float allRegionsAmbientInt = 0.15f;
	float allRegionsAmbientMin = 0.0f;
	float allRegionsDiffuseMipLevel = 5;
	float allRegionsIllumDiffuseMipLevel = 5;
	float allRegionsAngX = 0.0f, allRegionsAngY = 0.0f, allRegionsAngZ = 0.0f;
	float allRegionsOvrAngX = 0, allRegionsOvrAngY = 0, allRegionsOvrAngZ = 0;
	Vector4 allRegionsR, allRegionsU, allRegionsF;
	float allRegionsTexRes = -1;
	float allRegionsIllumTexRes = -1;
	float allRegionsMipRes = 16.0f;
	float allRegionsIllumMipRes = 16.0f;

	float regionSpecular[MAX_MISSION_REGIONS];
	float regionAmbientInt[MAX_MISSION_REGIONS];
	float regionAmbientMin[MAX_MISSION_REGIONS];
	float regionDiffuseMipLevel[MAX_MISSION_REGIONS];
	float regionIllumDiffuseMipLevel[MAX_MISSION_REGIONS];
	float regionAngX[MAX_MISSION_REGIONS] = { 0, 0, 0, 0 };
	float regionAngY[MAX_MISSION_REGIONS] = { 0, 0, 0, 0 };
	float regionAngZ[MAX_MISSION_REGIONS] = { 0, 0, 0, 0 };
	float regionOvrAngX[MAX_MISSION_REGIONS] = { 0, 0, 0, 0 };
	float regionOvrAngY[MAX_MISSION_REGIONS] = { 0, 0, 0, 0 };
	float regionOvrAngZ[MAX_MISSION_REGIONS] = { 0, 0, 0, 0 };
	float regionTexRes[MAX_MISSION_REGIONS] = { -1, -1, -1, -1 };
	float regionIllumTexRes[MAX_MISSION_REGIONS] = { -1, -1, -1, -1 };
	float regionMipRes[MAX_MISSION_REGIONS];
	float regionIllumMipRes[MAX_MISSION_REGIONS];

	ID3D11ShaderResourceView* allRegionsSRV = nullptr;
	ID3D11ShaderResourceView* allRegionsIllumSRV = nullptr;
	ID3D11ShaderResourceView* allRegionsOvrSRV = nullptr;
	ID3D11ShaderResourceView* regionSRV[MAX_MISSION_REGIONS] = { nullptr, nullptr, nullptr, nullptr };
	ID3D11ShaderResourceView* regionIllumSRV[MAX_MISSION_REGIONS] = { nullptr, nullptr, nullptr, nullptr };
	ID3D11ShaderResourceView* regionOvrSRV[MAX_MISSION_REGIONS] = { nullptr, nullptr, nullptr, nullptr };
};

extern CubeMapData g_CubeMaps;
extern bool g_bDefaultStarfieldRendered;

void RenderDefaultBackground(DeviceResources* deviceResources);
void LoadMissionCubeMaps(DeviceResources* deviceResources);
void ReleaseCubeMaps();
bool CubeMapsSkipBackdrop(const std::string& name);
