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

enum OptNodeEnum : unsigned int
{
	OptNode_NodeGroup = 0,
	OptNode_FaceData = 1,
	OptNode_TransformPositionRotation = 2,
	OptNode_MeshVertices = 3,
	OptNode_TransformPosition = 4,
	OptNode_TransformRotation = 5,
	OptNode_TransformScale = 6,
	OptNode_NodeReference = 7,
	OptNode_Unknown9 = 9,
	OptNode_Unknown10 = 10,
	OptNode_VertexNormals = 11,
	OptNode_Unknown12 = 12,
	OptNode_TextureVertices = 13,
	OptNode_Unknown14 = 14,
	OptNode_FaceData_0F = 15,
	OptNode_FaceData_10 = 16,
	OptNode_FaceData_11 = 17,
	OptNode_Unknown19 = 19,
	OptNode_Texture = 20,
	OptNode_FaceGrouping = 21,
	OptNode_Hardpoint = 22,
	OptNode_RotationScale = 23,
	OptNode_NodeSwitch = 24,
	OptNode_MeshDescriptor = 25,
	OptNode_TextureAlpha = 26,
	OptNode_D3DTexture = 27,
	OptNode_EngineGlow = 28,
};

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

struct XwaObject;

struct XwaCraft
{
	/* 0x0000 */ int XwaCraft_m000;
	/* 0x0004 */ unsigned short CraftIndex;
	/* 0x0006 */ int LeaderObjectIndex;
	/* 0x000A */ byte XwaCraft_m00A;
	/* 0x000B */ byte XwaCraft_m00B;
	/* 0x000C */ byte XwaCraft_m00C;
	/* 0x000D */ short XwaCraft_m00D;
	/* 0x000F */ short XwaCraft_m00F;
	/* 0x0011 */ short XwaCraft_m011;
	/* 0x0013 */ int XwaCraft_m013[5];
	/* 0x0027 */ byte SFoilsState; // flags
	/* 0x0028 */ char AIData[102];
	/* 0x008E */ short PickedUpObjectIndex;
	/* 0x0090 */ short XwaCraft_m090;
	/* 0x0092 */ short XwaCraft_m092;
	/* 0x0094 */ short XwaCraft_m094;
	/* 0x0096 */ short XwaCraft_m096;
	/* 0x0098 */ short XwaCraft_m098;
	/* 0x009A */ short XwaCraft_m09A;
	/* 0x009C */ short XwaCraft_m09C;
	/* 0x009E */ short XwaCraft_m09E;
	/* 0x00A0 */ short XwaCraft_m0A0;
	/* 0x00A2 */ short XwaCraft_m0A2;
	/* 0x00A4 */ byte XwaCraft_m0A4;
	/* 0x00A5 */ byte XwaCraft_m0A5;
	/* 0x00A6 */ byte XwaCraft_m0A6;
	/* 0x00A7 */ byte XwaCraft_m0A7;
	/* 0x00A8 */ byte XwaCraft_m0A8;
	/* 0x00A9 */ byte XwaCraft_m0A9;
	/* 0x00AA */ byte XwaCraft_m0AA;
	/* 0x00AB */ byte XwaCraft_m0AB;
	/* 0x00AC */ byte XwaCraft_m0AC;
	/* 0x00AD */ byte BeingBoarded;
	/* 0x00AE */ byte XwaCraft_m0AE;
	/* 0x00AF */ byte Docking;
	/* 0x00B0 */ short XwaCraft_m0B0;
	/* 0x00B2 */ short XwaCraft_m0B2;
	/* 0x00B4 */ int XwaCraft_m0B4;
	/* 0x00B8 */ int XwaCraft_m0B8;
	/* 0x00BC */ int XwaCraft_m0BC;
	/* 0x00C0 */ int XwaCraft_m0C0;
	/* 0x00C4 */ short Speed;
	/* 0x00C6 */ short XwaCraft_m0C6;
	/* 0x00C8 */ byte XwaCraft_m0C8;
	/* 0x00C9 */ byte XwaCraft_m0C9;
	/* 0x00CA */ short Pitch;
	/* 0x00CC */ short XwaCraft_m0CC;
	/* 0x00CE */ byte XwaCraft_m0CE;
	/* 0x00CF */ byte XwaCraft_m0CF;
	/* 0x00D0 */ short XwaCraft_m0D0;
	/* 0x00D2 */ short Roll;
	/* 0x00D4 */ short XwaCraft_m0D4;
	/* 0x00D6 */ byte XwaCraft_m0D6;
	/* 0x00D7 */ short XwaCraft_m0D7;
	/* 0x00D9 */ short Yaw;
	/* 0x00DB */ short XwaCraft_m0DB;
	/* 0x00DD */ byte XwaCraft_m0DD;
	/* 0x00DE */ short XwaCraft_m0DE;
	/* 0x00E0 */ byte FormationType;
	/* 0x00E1 */ byte FormationSpacing;
	/* 0x00E2 */ byte SpecialCraft;
	/* 0x00E3 */ byte XwaCraft_m0E3;
	/* 0x00E4 */ int XwaCraft_m0E4;
	/* 0x00E8 */ int XwaCraft_m0E8;
	/* 0x00EC */ int XwaCraft_m0EC;
	/* 0x00F0 */ unsigned short PresetThrottle;
	/* 0x00F2 */ unsigned short PercentOfActiveEngines;
	/* 0x00F4 */ short XwaCraft_m0F4;
	/* 0x00F6 */ short IsSlamEnabled;
	/* 0x00F8 */ unsigned int Damages;
	/* 0x00FC */ unsigned int CriticalDamageThreshold;
	/* 0x0100 */ unsigned int HullStrength;
	/* 0x0104 */ unsigned short SystemStrength;
	/* 0x0106 */ int XwaCraft_m106;
	/* 0x010A */ byte XwaCraft_m10A;
	/* 0x010B */ int XwaCraft_m10B;
	/* 0x010F */ int XwaCraft_m10F;
	/* 0x0113 */ int XwaCraft_m113;
	/* 0x0117 */ int XwaCraft_m117;
	/* 0x011B */ int XwaCraft_m11B;
	/* 0x011F */ int XwaCraft_m11F[8];
	/* 0x013F */ byte XwaCraft_m13F[8];
	/* 0x0147 */ int XwaCraft_m147[8];
	/* 0x0167 */ int XwaCraft_m167[6];
	/* 0x017F */ unsigned short XwaCraft_m17F; // flags
	/* 0x0181 */ unsigned short XwaCraft_m181; // flags
	/* 0x0183 */ unsigned short XwaCraft_m183; // flags
	/* 0x0185 */ unsigned short XwaCraft_m185; // flags
	/* 0x0187 */ short XwaCraft_m187;
	/* 0x0189 */ byte XwaCraft_m189;
	/* 0x018A */ byte XwaCraft_m18A;
	/* 0x018B */ byte IsCaptured;
	/* 0x018C */ byte XwaCraft_m18C[10];
	/* 0x0196 */ byte XwaCraft_m196[10];
	/* 0x01A0 */ byte CargoIndex;
	/* 0x01A1 */ byte XwaCraft_m1A1;
	/* 0x01A2 */ unsigned int ShieldStrength[2];
	/* 0x01AA */ byte PresetShield;
	/* 0x01AB */ unsigned char ShieldSetting;
	/* 0x01AC */ byte LasersCount;
	/* 0x01AD */ byte PresetLaser;
	/* 0x01AE */ byte WeaponRacksCount;
	/* 0x01AF */ unsigned char LasersConvergence;
	/* 0x01B0 */ unsigned short LaserTypeId[3];
	/* 0x01B6 */ byte XwaCraft_m1B6[3];
	/* 0x01B9 */ byte XwaCraft_m1B9[3];
	/* 0x01BC */ byte LaserStartRack[3];
	/* 0x01BF */ short XwaCraft_m1BF[3];
	/* 0x01C5 */ int XwaCraft_m1C5[3];
	/* 0x01D1 */ byte WarheadsCount;
	/* 0x01D2 */ unsigned short WarheadsModelIndex[2];
	/* 0x01D6 */ byte XwaCraft_m1D6[2];
	/* 0x01D8 */ short XwaCraft_m1D8[2];
	/* 0x01DC */ short XwaCraft_m1DC;
	/* 0x01DE */ unsigned char BeamType;
	/* 0x01DF */ byte PresetBeam;
	/* 0x01E0 */ short XwaCraft_m1E0;
	/* 0x01E2 */ bool IsBeamSystemEnabled;
	/* 0x01E3 */ short XwaCraft_m1E3;
	/* 0x01E5 */ short XwaCraft_m1E5;
	/* 0x01E7 */ unsigned char CounterMeasuresType;
	/* 0x01E8 */ byte CounterMeasuresCount;
	/* 0x01E9 */ short XwaCraft_m1E9;
	/* 0x01EB */ short XwaCraft_m1EB;
	/* 0x01ED */ short XwaCraft_m1ED;
	/* 0x01EF */ short XwaCraft_m1EF;
	/* 0x01F1 */ short XwaCraft_m1F1;
	/* 0x01F3 */ short XwaCraft_m1F3;
	/* 0x01F5 */ byte XwaCraft_m1F5;
	/* 0x01F6 */ byte XwaCraft_m1F6;
	/* 0x01F7 */ byte DamageAssessmentsOrder[11];
	/* 0x0202 */ short DamageAssessmentsPercent[11]; // 100 = not damaged
	/* 0x0218 */ short DamageAssessmentsTimeRemaining[11];
	/* 0x022E */ byte XwaCraft_m22E[50];
	/* 0x0260 */ byte MeshRotationAngles[50];
	/* 0x0292 */ byte XwaCraft_m292[50];
	/* 0x02C4 */ short XwaCraft_m2C4;
	/* 0x02C6 */ byte XwaCraft_m2C6;
	/* 0x02C7 */ byte XwaCraft_m2C7;
	/* 0x02C8 */ short XwaCraft_m2C8;
	/* 0x02CA */ byte XwaCraft_m2CA;
	/* 0x02CB */ unsigned char XwaCraft_m2CB;
	/* 0x02CC */ unsigned char XwaCraft_m2CC;
	/* 0x02CD */ short XwaCraft_m2CD;
	/* 0x02CF */ unsigned char EngineThrottles[16];
	/* 0x02DF */ char WeaponRacks[224];
	/* 0x03BF */ short XwaCraft_m3BF;
	/* 0x03C1 */ short RotGunAngles[2];
	/* 0x03C5 */ short RotBeamAngles[2];
	/* 0x03C9 */ float XwaCraft_m3C9[2];
	/* 0x03D1 */ float XwaCraft_m3D1[2];
	/* 0x03D9 */ char Cargo[16];
	/* 0x03E9 */ int XwaCraft_m3E9;
	/* 0x03ED */ int XwaCraft_m3ED;
	/* 0x03F1 */ int XwaCraft_m3F1;
	/* 0x03F5 */ XwaObject* pObject;
};

static_assert(sizeof(XwaCraft) == 1017, "size of XwaCraft must be 1017");

struct S0x08052A0_16
{
	/* 0x0000 */ unsigned char CollisionChecksActive;
	/* 0x0001 */ int CollisionCheckTimers[16];
	/* 0x0041 */ short CollisionCheckObjectIndex[16];
	/* 0x0061 */ int CollisionCheckMasterCountdown;
};

static_assert(sizeof(S0x08052A0_16) == 101, "size of S0x08052A0_16 must be 101");

struct XwaMobileObject
{
	/* 0x0000 */ unsigned char ObjectCategory;
	/* 0x0001 */ byte XwaMobileObject_m01;
	/* 0x0002 */ int ObjectSize;
	/* 0x0006 */ int LastUpdateTime;
	/* 0x000A */ int PositionX;
	/* 0x000E */ int PositionY;
	/* 0x0012 */ int PositionZ;
	/* 0x0016 */ S0x08052A0_16 CollisionChecks;
	/* 0x007B */ short XwaMobileObject_m7B;
	/* 0x007D */ short XwaMobileObject_m7D;
	/* 0x007F */ short XwaMobileObject_m7F;
	/* 0x0081 */ short XwaMobileObject_m81;
	/* 0x0083 */ short RotationAxisAngle;
	/* 0x0085 */ short Speed;
	/* 0x0087 */ short SpeedRelease;
	/* 0x0089 */ int Power;
	/* 0x008D */ int Duration;
	/* 0x0091 */ short XwaMobileObject_m91;
	/* 0x0093 */ short ObjectIndex;
	/* 0x0095 */ unsigned short ModelIndex;
	/* 0x0097 */ byte Iff;
	/* 0x0098 */ byte Team;
	/* 0x0099 */ byte Markings;
	/* 0x009A */ short EscapePodsCount; // escape pods or people
	/* 0x009C */ int XwaMobileObject_m9C;
	/* 0x00A0 */ byte XwaMobileObject_mA0;
	/* 0x00A1 */ short Velocity;
	/* 0x00A3 */ short XwaMobileObject_mA3;
	/* 0x00A5 */ short XwaMobileObject_mA5;
	/* 0x00A7 */ short XwaMobileObject_mA7;
	/* 0x00A9 */ short XwaMobileObject_mA9;
	/* 0x00AB */ short XwaMobileObject_mAB;
	/* 0x00AD */ short XwaMobileObject_mAD;
	/* 0x00AF */ short XwaMobileObject_mAF;
	/* 0x00B1 */ short XwaMobileObject_mB1;
	/* 0x00B3 */ float RotationAxisX;
	/* 0x00B7 */ float RotationAxisY;
	/* 0x00BB */ float RotationAxisZ;
	/* 0x00BF */ bool RecalculateForwardVector;
	/* 0x00C0 */ short ForwardX;
	/* 0x00C2 */ short ForwardY;
	/* 0x00C4 */ short ForwardZ;
	/* 0x00C6 */ bool RecalculateTransformMatrix;
	/* 0x00C7 */ short TransformMatrixFrontX;
	/* 0x00C9 */ short TransformMatrixFrontY;
	/* 0x00CB */ short TransformMatrixFrontZ;
	/* 0x00CD */ short TransformMatrixRightX;
	/* 0x00CF */ short TransformMatrixRightY;
	/* 0x00D1 */ short TransformMatrixRightZ;
	/* 0x00D3 */ short TransformMatrixUpX;
	/* 0x00D5 */ short TransformMatrixUpY;
	/* 0x00D7 */ short TransformMatrixUpZ;
	/* 0x00D9 */ int pWarhead;
	/* 0x00DD */ XwaCraft* pCraft;
	/* 0x00E1 */ int pChar;
};

static_assert(sizeof(XwaMobileObject) == 229, "size of XwaMobileObject must be 229");

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
	XwaMobileObject* pMobileObject;
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

struct OptNode
{
	const char* Name;
	OptNodeEnum NodeType;
	int NumOfNodes;
	OptNode** Nodes;
	int Parameter1;
	int Parameter2;
};

static_assert(sizeof(OptNode) == 24, "size of OptNode must be 24");

struct OptHeader
{
	int GlobalOffset;
	char unk04[2];
	int NumOfNodes;
	OptNode** Nodes;
};

static_assert(sizeof(OptHeader) == 14, "size of OptHeader must be 14");

struct XwaPlayerCamera
{
	/* 0x0000 */ int PositionX;
	/* 0x0004 */ int PositionY;
	/* 0x0008 */ int PositionZ;
	/* 0x000C */ int ObjectIndex;
	/* 0x0010 */ int TargetObjectIndex;
	/* 0x0014 */ short CraftPitch;
	/* 0x0016 */ short CraftYaw;
	/* 0x0018 */ short CraftRoll;
	/* 0x001A */ short Roll;
	/* 0x001C */ short Pitch;
	/* 0x001E */ short Yaw;
	/* 0x0020 */ int ShakeX;
	/* 0x0024 */ int ShakeY;
	/* 0x0028 */ int ShakeZ;
	/* 0x002C */ short collisionPitch;
	/* 0x002E */ short collisionYaw;
	/* 0x0030 */ short collisionRoll;
	/* 0x0032 */ short XwaPlayerCamera_m32;
	/* 0x0034 */ byte ViewMode1;
	/* 0x0035 */ byte ViewMode2;
	/* 0x0036 */ byte XwaPlayerCamera_m36;
	/* 0x0037 */ byte XwaPlayerCamera_m37;
	/* 0x0F38 */ byte unk38[1];
	/* 0x0039 */ short XwaPlayerCamera_m39;
	/* 0x003B */ short XwaPlayerCamera_m3B;
	/* 0x003D */ short MapMode;
	/* 0x003F */ short XwaPlayerCamera_m3F;
	/* 0x0041 */ short ExternalCamera;
	/* 0x0043 */ int ExternalCameraZoomDist;
	/* 0x0047 */ short XwaPlayerCamera_m47;
	/* 0x0049 */ short XwaPlayerCamera_m49;
	/* 0x0F4B */ byte unk4B[2];
};

static_assert(sizeof(XwaPlayerCamera) == 77, "size of XwaPlayerCamera must be 77");

struct XwaPlayer
{
	/* 0x0000 */ int ObjectIndex;
	/* 0x0004 */ int JumpNextCraftID;
	/* 0x0008 */ short PilotRating;
	/* 0x000A */ short Iff;
	/* 0x000C */ short Team;
	/* 0x000E */ short FGIndex;
	/* 0x0010 */ byte Region;
	/* 0x0011 */ byte ParticipationState;
	/* 0x0012 */ byte IsEjecting;
	/* 0x0013 */ byte AutopilotAction;
	/* 0x0014 */ byte EnginesCount;
	/* 0x0015 */ byte MapState;
	/* 0x0016 */ byte Autopilot;
	/* 0x0017 */ byte HyperspacePhase;
	/* 0x0018 */ byte HyperingToRegion;
	/* 0x0019 */ byte HyperspaceCooldown;
	/* 0x001A */ byte HyperedToRegion[5];
	/* 0x001F */ int TimeInHyperspace;
	/* 0x0023 */ byte ShowTargetBox;
	/* 0x0024 */ byte WarheadLockState;
	/* 0x0025 */ short CurrentTargetIndex;
	/* 0x0027 */ short TargetTimeTargeted;
	/* 0x0029 */ short LastTargetIndex;
	/* 0x002B */ short CraftMemory[4];
	/* 0x0033 */ byte PrimarySecondaryArmed;
	/* 0x0034 */ byte WarheadArmed;
	/* 0x0035 */ short ComponentTargetIndex;
	/* 0x0037 */ short XwaPlayer_m037;
	/* 0x0039 */ short EngineWashCraftIndex;
	/* 0x003B */ short EngineWashAmount;
	/* 0x003D */ short PresetThrottle[2];
	/* 0x0041 */ byte PresetLaser[2];
	/* 0x0043 */ byte PresetShield[2];
	/* 0x0045 */ byte PresetBeam[2];
	/* 0x0047 */ short Throttle;
	/* 0x0049 */ byte Laser;
	/* 0x004A */ byte Shield;
	/* 0x004B */ byte Beam;
	/* 0x004C */ unsigned char ShieldSetting;
	/* 0x004D */ byte XwaPlayer_m04D;
	/* 0x004E */ byte XwaPlayer_m04E;
	/* 0x004F */ byte XwaPlayer_m04F;
	/* 0x0050 */ byte XwaPlayer_m050;
	/* 0x0051 */ byte XwaPlayer_m051;
	/* 0x0052 */ byte CriticalMessageType;
	/* 0x0053 */ short CriticalMessageTargetObjectIndex;
	/* 0x0055 */ short XwaPlayer_m055;
	/* 0x0057 */ short JoystickRollHeld;
	/* 0x0059 */ short JoystickYawDrift;
	/* 0x005B */ short JoystickPitchDrift;
	/* 0x005D */ short JoystickRollDrift;
	/* 0x005F */ short JoystickTriggerFlags;
	/* 0x0061 */ short JoystickRollDelayTimer;
	/* 0x0063 */ byte SwapXR;
	/* 0x0064 */ byte HudActive1;
	/* 0x0065 */ byte HudActive2;
	/* 0x0066 */ byte SimpleHUD;
	/* 0x0067 */ byte BottomLeftPanel;
	/* 0x0068 */ byte BottomRightPanel;
	/* 0x0069 */ byte ActiveWeapon;
	/* 0x006A */ byte XwaPlayer_m06A;
	/* 0x006B */ byte ConsoleActivated;
	/* 0x006C */ byte PanelSelected1;
	/* 0x006D */ byte PanelSelected2;
	/* 0x006E */ unsigned char HudMfdType[3];
	/* 0x0071 */ unsigned char XwaPlayer_m071[3];
	/* 0x0074 */ byte FlightCommandSelected;
	/* 0x0075 */ byte FlightCommandSelection;
	/* 0xF076 */ byte unk076[2];
	/* 0x0078 */ byte FlightCommandNumWingmenSelectable;
	/* 0x0079 */ byte FlightCommandWingmanSelected;
	/* 0x007A */ byte XwaPlayer_m07A[7];
	/* 0x0081 */ int XwaPlayer_m081[7];
	/* 0x009D */ int XwaPlayer_m09D[7];
	/* 0xF0B9 */ byte unk0B9[12];
	/* 0x00C5 */ byte XwaPlayer_m0C5;
	/* 0x00C6 */ short XwaPlayer_m0C6[7];
	/* 0x00D4 */ short XwaPlayer_m0D4[7];
	/* 0xF0E2 */ byte unk0E2[6];
	/* 0x00E8 */ byte XwaPlayer_m0E8;
	/* 0x00E9 */ char ConsoleScript[256];
	/* 0x01E9 */ byte XwaPlayer_m1E9;
	/* 0x01EA */ short ConsoleScriptLength;
	/* 0x01EC */ int AiObjectIndex;
	/* 0x01F0 */ byte XwaPlayer_m1F0;
	/* 0x01F1 */ byte XwaPlayer_m1F1[8];
	/* 0x01F9 */ byte CockpitDisplayed;
	/* 0x01FA */ bool HasCockpitOpt;
	/* 0x01FB */ bool HasTurretOpt;
	/* 0x01FC */ byte XwaPlayer_m1FC;
	/* 0x01FD */ short MousePositionX;
	/* 0x01FF */ short MousePositionY;
	/* 0x0201 */ float XwaPlayer_m201;
	/* 0x0205 */ float XwaPlayer_m205;
	/* 0x0209 */ float XwaPlayer_m209;
	/* 0x020D */ float XwaPlayer_m20D;
	/* 0x0211 */ float XwaPlayer_m211;
	/* 0x0215 */ float XwaPlayer_m215;
	/* 0x0219 */ short TurretIndex;
	/* 0x021B */ short NumberOfGunnerHardpoints;
	/* 0x021D */ byte CurrentGunnerHardpointActive;
	/* 0x021E */ short GunnerRotationMatrix_Front_X;
	/* 0x0220 */ short GunnerRotationMatrix_Front_Y;
	/* 0x0222 */ short GunnerRotationMatrix_Front_Z;
	/* 0x0224 */ short GunnerRotationMatrix_Right_X;
	/* 0x0226 */ short GunnerRotationMatrix_Right_Y;
	/* 0x0228 */ short GunnerRotationMatrix_Right_Z;
	/* 0x022A */ short GunnerRotationMatrix_Up_X;
	/* 0x022C */ short GunnerRotationMatrix_Up_Y;
	/* 0x022E */ short GunnerRotationMatrix_Up_Z;
	/* 0x0230 */ int Score;
	/* 0x0234 */ int XwaPlayer_m234;
	/* 0x0238 */ int PromoPoints;
	/* 0x023C */ int WorsePromoPoints;
	/* 0x0240 */ int XwaPlayer_m240;
	/* 0x0244 */ int XwaPlayer_m244;
	/* 0x0248 */ int XwaPlayer_m248;
	/* 0x024C */ short EnergyWeapon1Fired;
	/* 0x024E */ short EnergyWeapon1Hits;
	/* 0x0250 */ short EnergyWeapon2Fired;
	/* 0x0252 */ short EnergyWeapon2Hits;
	/* 0x0254 */ short WarheadsFired;
	/* 0x0256 */ short WarheadHits;
	/* 0x0258 */ short NumOfCraftInspected;
	/* 0x025A */ short NumOfSpecialCraftInspected;
	/* 0x025C */ short KillsOnFG[192];
	/* 0x03DC */ short KillsSharedOnFG[192];
	/* 0x055C */ short KillsAssistOnFG[192];
	/* 0x06DC */ short KillsFullOnPlayerRating[25];
	/* 0x070E */ short KillsSharedOnPlayerRating[25];
	/* 0x0740 */ short KillsAssistOnPlayerRating[25];
	/* 0x0772 */ short KillsFullOnAIRating[6];
	/* 0x077E */ short KillsSharedOnAIRating[6];
	/* 0x078A */ short KillsAssistOnAIRating[6];
	/* 0x0796 */ short KillsFullOnPlayer[8];
	/* 0x07A6 */ short KillsSharedOnPlayer[8];
	/* 0x07B6 */ short FriendliesKilled;
	/* 0x07B8 */ short TotalLosses;
	/* 0x07BA */ short TotalLossesByCollision;
	/* 0x07BC */ short TotalLossesByStarship;
	/* 0x07BE */ short TotalLossesByMine;
	/* 0x07C0 */ short KilledByFullFromPlayer[8];
	/* 0x07D0 */ short KilledBySharedFromPlayer[8];
	/* 0x07E0 */ short KilledFullByFG[192];
	/* 0x0960 */ short KilledSharedByFG[192];
	/* 0x0AE0 */ short KilledByPlayerRating[25];
	/* 0x0B12 */ short KilledByAI;
	/* 0x0B14 */ char ChatString[50];
	/* 0x0B46 */ byte ChatStringLength;
	/* 0x0B47 */ byte MultiChatMode;
	/* 0x0B48 */ XwaPlayerCamera Camera;
	/* 0x0B95 */ short FlightScreenRes;
	/* 0x0B97 */ int DirectPlayId;
	/* 0x0B9B */ int MissionTime;
	/* 0x0B9F */ int PositionX;
	/* 0x0BA3 */ int PositionY;
	/* 0x0BA7 */ int PositionZ;
	/* 0x0BAB */ short XwaPlayer_mBAB;
	/* 0x0BAD */ short HeadingZ;
	/* 0x0BAF */ short HeadingXY;
	/* 0x0BB1 */ int Duration;
	/* 0x0BB5 */ short CurrentSpeed;
	/* 0x0BB7 */ short CurrentSpeedRelease;
	/* 0x0BB9 */ short CurrentSpeedFraction;
	/* 0x0BBB */ short ObjectID;
	/* 0x0BBD */ byte IsEjectingDelta;
	/* 0x0BBE */ int CriticalMessageTimer;
	/* 0x0BC2 */ int XwaPlayer_mBC2;
	/* 0x0BC6 */ int XwaPlayer_mBC6;
	/* 0x0BCA */ int TimeInMissionDelta;
	/* 0x0BCE */ unsigned char CoPilotControlled;
};

static_assert(sizeof(XwaPlayer) == 3023, "size of XwaPlayer must be 3023");

inline int* g_playerInHangar = (int*)0x09C6E40;
inline int* g_playerIndex = (int*)0x8C1CC8;

inline XwaObject** objects = (XwaObject**)0x7B33C4;
inline XwaPlayer* PlayerDataTable = (XwaPlayer*)0x8B94E0;

#pragma pack(pop)
