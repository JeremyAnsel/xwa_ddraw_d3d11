#pragma once

#include "xwa_structures.h"

class DeviceResources;

#pragma pack(push, 1)

#pragma pack(pop)

void D3dRendererSceneBegin(DeviceResources* deviceResources);
void D3dRendererSceneEnd();
void D3dRendererFlightStart();
void D3dRenderLasersHook(int A4);
void D3dRenderMiniatureHook(int A4, int A8, int AC, int A10, int A14);
void D3dRenderHyperspaceLinesHook(int A4);
void D3dRendererMainHook(SceneCompData* scene);
void D3dRendererShadowHook(SceneCompData* scene);
