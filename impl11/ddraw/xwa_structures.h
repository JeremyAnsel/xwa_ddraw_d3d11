#pragma once

#include "targetver.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef STRICT
#define STRICT
#endif

#include <d3d.h>

#pragma pack(push, 1)

struct ExeEnableEntry
{
	unsigned char ExeEnableEntry_m00; // flags
	unsigned char ExeEnableEntry_m01; // flags
	unsigned char ObjectCategory;
	unsigned char ShipCategory;
	unsigned int ObjectSize;
	void* pData1;
	void* pData2;
	unsigned short ExeEnableEntry_m10; // flags
	unsigned short CraftIndex;
	short DataIndex1;
	short DataIndex2;
};

static_assert(sizeof(ExeEnableEntry) == 24, "size of ExeEnableEntry must be 24");

struct XwaObject
{
	short XwaObject_m00;
	unsigned short ModelIndex;
	unsigned char ShipCategory;
	unsigned char TieFlightGroupIndex;
	unsigned char Region;
	int PositionX;
	int PositionY;
	int PositionZ;
	short HeadingXY;
	short HeadingZ;
	short XwaObject_m17;
	short XwaObject_m19;
	short XwaObject_m1B;
	unsigned char XwaObject_m1D[2];
	int PlayerIndex;
	int pMobileObject;
};

static_assert(sizeof(XwaObject) == 39, "size of XwaObject must be 39");

struct XwaSpeciesTMInfo
{
	void* pData;
	unsigned short Width;
	unsigned short Height;
	unsigned int Color;
	unsigned char MipmapLevel;
};

static_assert(sizeof(XwaSpeciesTMInfo) == 13, "size of XwaSpeciesTMInfo must be 13");

struct XwaD3dTexture
{
	LPDIRECT3DTEXTURE lpD3DTexture;
	LPDIRECTDRAWSURFACE lpDDSurface;
	DDSURFACEDESC DDSurfaceDesc;
	D3DTEXTUREHANDLE D3DTextureHandle;
	bool IsCached;
	char unk79[3];
	int Width;
	int Height;
	unsigned int DataSize;
	int XwaD3dTexture_m88;
	XwaD3dTexture* pPrevious;
	XwaD3dTexture* pNext;
};

static_assert(sizeof(XwaD3dTexture) == 148, "size of XwaD3dTexture must be 148");

struct XwaD3dVertex
{
	float x;
	float y;
	float z;
	float rhw;
	unsigned int color;
	unsigned int specular;
	float tu;
	float tv;
};

static_assert(sizeof(XwaD3dVertex) == 32, "size of XwaD3dVertex must be 32");

struct XwaD3dTriangle
{
	int v1;
	int v2;
	int v3;
	unsigned int RenderStatesFlags;
	XwaD3dTexture* pTexture;
};

static_assert(sizeof(XwaD3dTriangle) == 20, "size of XwaD3dTriangle must be 20");

struct XwaDataMesh
{
	XwaD3dVertex Vertices[384];
	int VerticesCount;
	XwaD3dTriangle Triangles[384];
	int TrianglesCount;
	XwaDataMesh* Next;
};

static_assert(sizeof(XwaDataMesh) == 19980, "size of XwaDataMesh must be 19980");

struct XwaVector3
{
	float x;
	float y;
	float z;
};

static_assert(sizeof(XwaVector3) == 12, "size of XwaVector3 must be 12");

struct XwaTextureVertex
{
	float u;
	float v;
};

static_assert(sizeof(XwaTextureVertex) == 8, "size of XwaTextureVertex must be 8");

struct XwaMatrix3x3
{
	float _11;
	float _12;
	float _13;
	float _21;
	float _22;
	float _23;
	float _31;
	float _32;
	float _33;
};

static_assert(sizeof(XwaMatrix3x3) == 36, "size of XwaMatrix3x3 must be 36");

struct XwaTransform
{
	XwaVector3 Position;
	XwaMatrix3x3 Rotation;
};

static_assert(sizeof(XwaTransform) == 48, "size of XwaTransform must be 48");

struct OptFaceDataNode_01_Data_Indices
{
	int Vertex[4];
	int Edge[4];
	int TextureVertex[4];
	int VertexNormal[4];
};

static_assert(sizeof(OptFaceDataNode_01_Data_Indices) == 64, "size of OptFaceDataNode_01_Data_Indices must be 64");

struct XwaTextureDescription
{
	unsigned short* Palettes;
	int XwaTextureDescription_m04;
	int TextureSize;
	int DataSize;
	int Width;
	int Height;
};

static_assert(sizeof(XwaTextureDescription) == 24, "size of XwaTextureDescription must be 24");

struct XwaTextureSurface
{
	unsigned char XwaTextureSurface_m00;
	char unk01[3];
	LPDIRECTDRAWSURFACE lpDDSurface;
	LPDIRECT3DTEXTURE lpD3DTexture;
	LPDIRECTDRAWPALETTE lpDDPalette;
	XwaD3dTexture D3dTexture;
	unsigned char XwaTextureSurface_mA4;
	char unkA5[3];
	int MipmapLevel;
	int MipmapCount;
};

static_assert(sizeof(XwaTextureSurface) == 176, "size of XwaTextureSurface must be 176");

struct XwaD3DInfo
{
	int Id;
	short RefCounter;
	XwaTextureDescription TextureDescription;
	unsigned char AlphaMask;
	int MipMapsCount;
	XwaTextureSurface* ColorMap[6];
	XwaTextureSurface* LightMap[6];
	XwaD3DInfo* pNext;
	XwaD3DInfo* pPrevious;
};

static_assert(sizeof(XwaD3DInfo) == 91, "size of XwaD3DInfo must be 91");

struct XwaGlowMarkItem
{
	float Size;
	unsigned int Color;
	XwaTextureSurface* pTextureSurface;
};

static_assert(sizeof(XwaGlowMarkItem) == 12, "size of XwaGlowMarkItem must be 12");

struct XwaGlowMark
{
	int Count;
	XwaGlowMarkItem Items[2];
	XwaTextureVertex* UVArray;
	unsigned short* FaceOn;
};

static_assert(sizeof(XwaGlowMark) == 36, "size of XwaGlowMark must be 36");

struct XwaKnockoutData
{
	unsigned short GlowMarksCount;
	XwaVector3* GlowMarksPositions[16];
	XwaGlowMark GlowMarks[16];
	float XwaKnockoutData_m282;
	float XwaKnockoutData_m286;
	float XwaKnockoutData_m28A;
	float XwaKnockoutData_m28E;
	float XwaKnockoutData_m292;
	float XwaKnockoutData_m296;
	float XwaKnockoutData_m29A;
	float XwaKnockoutData_m29E;
	float XwaKnockoutData_m2A2;
	float XwaKnockoutData_m2A6;
	float XwaKnockoutData_m2AA;
	float XwaKnockoutData_m2AE;
	float XwaKnockoutData_m2B2;
	char unk2B6[4];
	short XwaKnockoutData_m2BA;
	short XwaKnockoutData_m2BC;
	int XwaKnockoutData_m2BE;
	short XwaKnockoutData_m2C2;
	unsigned short ModelIndex;
	short XwaKnockoutData_m2C6;
	unsigned int XwaKnockoutData_m2C8[2];
	float XwaKnockoutData_m2D0[2];
	XwaKnockoutData* XwaKnockoutData_m2D8;
	XwaKnockoutData* XwaKnockoutData_m2DC;
	XwaKnockoutData* XwaKnockoutData_m2E0;
};

static_assert(sizeof(XwaKnockoutData) == 740, "size of XwaKnockoutData must be 740");

struct XwaObject3D
{
	int XwaObject3D_m00;
	void* XwaObject3D_m04;
	void* pDataTrail;
	void* XwaObject3D_m0C;
	XwaKnockoutData* pKnockoutData;
	void* pDataKnockout;
};

static_assert(sizeof(XwaObject3D) == 24, "size of XwaObject3D must be 24");

struct SceneCompData
{
	XwaObject* pObject;
	float RotationAngle;
	XwaTransform WorldViewTransform;
	XwaTransform WorldViewTransformTransposed;
	XwaVector3 SceneCompData_m068;
	int SceneCompData_m074;
	int VerticesCount;
	XwaVector3* MeshVertices;
	XwaTextureVertex* MeshTextureVertices;
	XwaVector3* MeshVertexNormals;
	int SceneCompData_m088;
	int FacesCount;
	int EdgesCount;
	XwaVector3* FaceNormals;
	XwaVector3* FaceTexturingInfos;
	int SceneCompData_m09C;
	OptFaceDataNode_01_Data_Indices* FaceIndices;
	const char* TextureName;
	XwaTextureDescription* TextureDescription;
	unsigned char* pTextureData;
	unsigned short* TexturePalettes;
	unsigned int TextureAlphaMask;
	XwaD3DInfo* D3DInfo;
	int FaceListIndex;
	int VertListIndex;
	int EdgeListIndex;
	int FaceListCount;
	int VertListCount;
	int EdgeListCount;
	int MeshDescriptor;
	int GlowMarksCount;
	XwaGlowMark* GlowMarks[16];
};

static_assert(sizeof(SceneCompData) == 284, "size of SceneCompData must be 284");

struct SceneFaceList
{
	int FaceIndex;
	int Id;
	SceneCompData* pSceneCompData;
	int SceneFaceList_m0C;
	int SceneFaceList_m10;
	float TexturingDirectionX;
	float TexturingDirectionY;
	float TexturingDirectionZ;
	float TexturingMagnitureX;
	float TexturingMagnitureY;
	float TexturingMagnitureZ;
	float SceneFaceList_m2C;
	float SceneFaceList_m30;
	float SceneFaceList_m34;
	float SceneFaceList_m38;
	int SceneFaceList_m3C;
	int pPhongData;
	int SceneFaceList_m44;
	int SceneFaceList_m48;
	float SceneFaceList_m4C;
	float SceneFaceList_m50;
	int pEdgeLists[4];
	char unk64[4];
	int SceneFaceList_m68;
	int SceneFaceList_m6C;
	int SceneFaceList_m70;
};

static_assert(sizeof(SceneFaceList) == 116, "size of SceneFaceList must be 116");

struct SceneVertList
{
	XwaVector3 Position;
	float ColorIntensity;
	float ColorA;
	float ColorR;
	float ColorG;
	float ColorB;
	float TextureU;
	float TextureV;
	XwaTextureVertex TextureVertices[16];
	int TextureVerticesCount;
};

static_assert(sizeof(SceneVertList) == 172, "size of SceneVertList must be 172");

struct XwaGlobalLight
{
	int PositionX;
	int PositionY;
	int PositionZ;
	float DirectionX;
	float DirectionY;
	float DirectionZ;
	float Intensity;
	float m1C;
	float ColorR;
	float ColorB;
	float ColorG;
	float BlendStartIntensity;
	float BlendStartColor1C;
	float BlendStartColorR;
	float BlendStartColorB;
	float BlendStartColorG;
	float BlendEndIntensity;
	float BlendEndColor1C;
	float BlendEndColorR;
	float BlendEndColorB;
	float BlendEndColorG;
};

static_assert(sizeof(XwaGlobalLight) == 84, "size of XwaGlobalLight must be 84");

struct XwaLocalLight
{
	int iPositionX;
	int iPositionY;
	int iPositionZ;
	float fPositionX;
	float fPositionY;
	float fPositionZ;
	float m18;
	int m1C;
	float m20;
	float ColorR;
	float ColorB;
	float ColorG;
};

static_assert(sizeof(XwaLocalLight) == 48, "size of XwaLocalLight must be 48");

#pragma pack(pop)
