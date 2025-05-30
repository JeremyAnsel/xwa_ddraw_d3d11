#pragma once

#include "xwa_structures.h"

class DeviceResources;

#pragma pack(push, 1)

#pragma pack(pop)

void D3dRendererInitialize();
void D3dRendererUninitialize();
void D3dRendererSceneBegin(DeviceResources* deviceResources);
void D3dRendererSceneEnd();
void D3dRendererFlightStart();
void D3dRenderLasersHook(int A4);
void D3dRenderMiniatureHook(int A4, int A8, int AC, int A10, int A14);
void D3dRenderHyperspaceLinesHook(int A4);
void D3dRendererMainHook(SceneCompData* scene);
void D3dRendererShadowHook(SceneCompData* scene);
void D3dRendererOptLoadHook(int handle);
void D3dRendererOptNodeHook(OptHeader* optHeader, int nodeIndex, SceneCompData* scene);

void D3dRleaseD3DINFO(XwaD3DInfo* d3dInfo);
XwaD3DInfo* D3dOptCreateD3DfromTexture(OptNode* A4, int A8, XwaTextureDescription* AC, unsigned char* A10, unsigned char* A14, unsigned short* A18, unsigned short* A1C);

HGLOBAL WINAPI DatGlobalAllocHook(UINT uFlags, SIZE_T dwBytes);
HGLOBAL WINAPI DatGlobalReAllocHook(HGLOBAL hMem, SIZE_T dwBytes, UINT uFlags);
