#include "common.h"
#include "XwaD3dRendererHook.h"
#include "DeviceResources.h"
#include "Direct3DTexture.h"
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <iomanip>

#ifdef _DEBUG
#include "../Debug/XwaD3dVertexShader.h"
#include "../Debug/XwaD3dPixelShader.h"
#include "../Debug/XwaD3dNormalsPixelShader.h"
#include "../Debug/XwaD3dShadowVertexShader.h"
#include "../Debug/XwaD3dShadowPixelShader.h"
#else
#include "../Release/XwaD3dVertexShader.h"
#include "../Release/XwaD3dPixelShader.h"
#include "../Release/XwaD3dNormalsPixelShader.h"
#include "../Release/XwaD3dShadowVertexShader.h"
#include "../Release/XwaD3dShadowPixelShader.h"
#endif

#undef LOGGER_DUMP
#define LOGGER_DUMP 0

#pragma pack(push, 1)

struct D3dGlobalLight
{
	float direction[4];
	float color[4];
};

struct D3dLocalLight
{
	float position[4];
	float color[4];
};

struct D3dConstants
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
	float floorLevel;
	float cameraPositionX;
	float cameraPositionY;
	float hangarShadowAccStartEnd;
	float s_V0x0662814;
	float s_V0x064D1A4;
	float s_V0x06626F4;
	float s_V0x063D194;
	float transformView[16];
	float transformWorldView[16];
	int globalLightsCount;
	int localLightsCount;
	int renderType;
	int renderTypeIllum;
	D3dGlobalLight globalLights[8];
	D3dLocalLight localLights[8];
};

static_assert(sizeof(D3dConstants) % 16 == 0, "D3dConstants size must be multiple of 16");

struct D3dVertex
{
	int iV;
	int iN;
	int iT;
	int c;
};

struct D3dTriangle
{
	int v1;
	int v2;
	int v3;
};

#pragma pack(pop)

#if LOGGER_DUMP

static int s_currentFrameIndex;
static std::string g_currentTextureName;

std::ofstream& get_d3d_file()
{
	static std::ofstream g_d3d_file;
	static bool initialized = false;

	if (!initialized)
	{
		initialized = true;
		g_d3d_file.open("ddraw_d3d.txt");
	}

	return g_d3d_file;
}

void DumpConstants(D3dConstants& constants)
{
	std::ostringstream str;
	str << std::setprecision(30);

	str << "\tConstants" << std::endl;
	str << "\t" << constants.viewportScale[0] << "; " << constants.viewportScale[1] << "; " << constants.viewportScale[2] << "; " << constants.viewportScale[3] << std::endl;
	str << "\t" << constants.projectionValue1 << std::endl;
	str << "\t" << constants.projectionValue2 << std::endl;
	str << "\t" << constants.projectionDeltaX << std::endl;
	str << "\t" << constants.projectionDeltaY << std::endl;

	str << "\t" << constants.transformWorldView[0] << "; " << constants.transformWorldView[1] << "; " << constants.transformWorldView[2] << "; " << constants.transformWorldView[3] << std::endl;
	str << "\t" << constants.transformWorldView[4] << "; " << constants.transformWorldView[5] << "; " << constants.transformWorldView[6] << "; " << constants.transformWorldView[7] << std::endl;
	str << "\t" << constants.transformWorldView[8] << "; " << constants.transformWorldView[9] << "; " << constants.transformWorldView[10] << "; " << constants.transformWorldView[11] << std::endl;
	str << "\t" << constants.transformWorldView[12] << "; " << constants.transformWorldView[13] << "; " << constants.transformWorldView[14] << "; " << constants.transformWorldView[15] << std::endl;

	get_d3d_file() << str.str() << std::endl;
}

void DumpFrame()
{
	std::ostringstream str;

	str << "\tFRAME " << s_currentFrameIndex;
	str << std::endl;

	get_d3d_file() << str.str() << std::endl;

	s_currentFrameIndex++;
}

void DumpSurface()
{
	std::ostringstream str;

	str << "\tSURFACE " << g_currentTextureName;
	str << std::endl;

	get_d3d_file() << str.str() << std::endl;
}

void DumpVector3(XwaVector3* vertices, int count)
{
	std::ostringstream str;
	str << std::setprecision(30);

	for (int index = 0; index < count; index++)
	{
		XwaVector3 v = vertices[index];

		str << "\tVERT" << index << ":";
		str << "\t( " << v.x << "\t; " << v.y << "\t; " << v.z << " )";
		str << std::endl;
	}

	get_d3d_file() << str.str() << std::endl;
}

void DumpTextureVertices(XwaTextureVertex* vertices, int count)
{
	std::ostringstream str;
	str << std::setprecision(30);

	for (int index = 0; index < count; index++)
	{
		XwaTextureVertex v = vertices[index];

		str << "\tTEX" << index << ":";
		str << "\t( " << v.u << "\t; " << v.v << " )";
		str << std::endl;
	}

	get_d3d_file() << str.str() << std::endl;
}

void DumpD3dVertices(D3dVertex* vertices, int count)
{
	std::ostringstream str;

	for (int index = 0; index < count; index++)
	{
		D3dVertex v = vertices[index];

		str << "\tTRI" << index << ":";
		str << "\t( " << v.iV << "\t; " << v.iT << " )";
		str << std::endl;
	}

	get_d3d_file() << str.str() << std::endl;
}

#endif

static bool g_isInRenderLasers = false;
static bool g_isInRenderMiniature = false;
static bool g_isInRenderHyperspaceLines = false;

enum RendererType
{
	RendererType_Unknown,
	RendererType_Main,
	RendererType_Shadow,
};

static RendererType g_rendererType = RendererType_Unknown;

static DeviceResources* g_deviceResources = nullptr;

class D3dRenderer
{
public:
	D3dRenderer();
	void SceneBegin(DeviceResources* deviceResources);
	void SceneEnd();
	void FlightStart();
	void MainSceneHook(const SceneCompData* scene);
	void HangarShadowSceneHook(const SceneCompData* scene);
	void UpdateTextures(const SceneCompData* scene);
	void UpdateMeshBuffers(const SceneCompData* scene);
	void ResizeDataVector(const SceneCompData* scene);
	void CreateDataScene(const SceneCompData* scene);
	void UpdateVertexAndIndexBuffers(const SceneCompData* scene);
	void UpdateConstantBuffer(const SceneCompData* scene);
	void RenderScene();
	void BuildGlowMarks(SceneCompData* scene);
	void RenderGlowMarks();
	void Initialize();
	void CreateConstantBuffer();
	void CreateStates();
	void CreateShaders();
	void GetViewport(D3D11_VIEWPORT* viewport);
	void GetViewportScale(float* viewportScale);

public:
	int _currentOptMeshIndex;

private:
	DeviceResources* _deviceResources;

	int _totalVerticesCount;
	int _totalTrianglesCount;

	int _currentFaceIndex;

	int _verticesCount;
	std::vector<D3dVertex> _vertices;
	int _trianglesCount;
	std::vector<D3dTriangle> _triangles;

	std::vector<XwaD3dVertex> _glowMarksVertices;
	std::vector<XwaD3dTriangle> _glowMarksTriangles;

	bool _isInitialized;
	UINT _meshBufferInitialCount;
	std::map<int, ComPtr<ID3D11ShaderResourceView>> _meshVerticesViews;
	std::map<int, ComPtr<ID3D11ShaderResourceView>> _meshNormalsViews;
	std::map<int, ComPtr<ID3D11ShaderResourceView>> _meshTextureCoordsViews;
	XwaVector3* _lastMeshVertices;
	ID3D11ShaderResourceView* _lastMeshVerticesView;
	XwaVector3* _lastMeshVertexNormals;
	ID3D11ShaderResourceView* _lastMeshVertexNormalsView;
	XwaTextureVertex* _lastMeshTextureVertices;
	ID3D11ShaderResourceView* _lastMeshTextureVerticesView;
	std::map<int, ComPtr<ID3D11Buffer>> _vertexBuffers;
	std::map<int, ComPtr<ID3D11Buffer>> _triangleBuffers;
	std::map<int, std::pair<int, int>> _vertexCounters;
	D3dConstants _constants;
	ComPtr<ID3D11Buffer> _constantBuffer;
	ComPtr<ID3D11RasterizerState> _rasterizerState;
	ComPtr<ID3D11RasterizerState> _rasterizerStateCull;
	ComPtr<ID3D11RasterizerState> _rasterizerStateWireframe;
	ComPtr<ID3D11SamplerState> _samplerState;
	ComPtr<ID3D11BlendState> _solidBlendState;
	ComPtr<ID3D11DepthStencilState> _solidDepthState;
	ComPtr<ID3D11BlendState> _transparentBlendState;
	ComPtr<ID3D11DepthStencilState> _transparentDepthState;
	ComPtr<ID3D11InputLayout> _inputLayout;
	ComPtr<ID3D11VertexShader> _vertexShader;
	ComPtr<ID3D11PixelShader> _pixelShader;
	ComPtr<ID3D11PixelShader> _normalsPixelShader;
	ComPtr<ID3D11VertexShader> _shadowVertexShader;
	ComPtr<ID3D11PixelShader> _shadowPixelShader;
	D3D11_VIEWPORT _viewport;
};

D3dRenderer::D3dRenderer()
{
	_isInitialized = false;
	_deviceResources = nullptr;
	_totalVerticesCount = 0;
	_totalTrianglesCount = 0;
	_currentFaceIndex = 0;
	_verticesCount = 0;
	_trianglesCount = 0;
	_meshBufferInitialCount = 65536;
	_lastMeshVertices = nullptr;
	_lastMeshVerticesView = nullptr;
	_lastMeshVertexNormals = nullptr;
	_lastMeshVertexNormalsView = nullptr;
	_lastMeshTextureVertices = nullptr;
	_lastMeshTextureVerticesView = nullptr;
	_constants = {};
	_viewport = {};
	_currentOptMeshIndex = -1;
}

void D3dRenderer::SceneBegin(DeviceResources* deviceResources)
{
	g_deviceResources = deviceResources;

	_deviceResources = deviceResources;

	_deviceResources->BeginAnnotatedEvent(L"D3dRendererScene");

	if (!_isInitialized)
	{
		Initialize();
		_isInitialized = true;
	}

	_totalVerticesCount = 0;
	_totalTrianglesCount = 0;

	_lastMeshVertices = nullptr;
	_lastMeshVerticesView = nullptr;
	_lastMeshVertexNormals = nullptr;
	_lastMeshVertexNormalsView = nullptr;
	_lastMeshTextureVertices = nullptr;
	_lastMeshTextureVerticesView = nullptr;

	GetViewport(&_viewport);
	GetViewportScale(_constants.viewportScale);

#if LOGGER_DUMP
	DumpFrame();
#endif
}

void D3dRenderer::SceneEnd()
{
	_deviceResources->EndAnnotatedEvent();
}

void D3dRenderer::FlightStart()
{
	_lastMeshVertices = nullptr;
	_lastMeshVertexNormals = nullptr;
	_lastMeshTextureVertices = nullptr;

	_meshVerticesViews.clear();
	_meshNormalsViews.clear();
	_meshTextureCoordsViews.clear();
	_vertexBuffers.clear();
	_triangleBuffers.clear();
	_vertexCounters.clear();

#if LOGGER_DUMP
	s_currentFrameIndex = 0;
#endif
}

void D3dRenderer::MainSceneHook(const SceneCompData* scene)
{
	ID3D11DeviceContext* context = _deviceResources->_d3dDeviceContext;

	ComPtr<ID3D11Buffer> oldVSConstantBuffer;
	ComPtr<ID3D11Buffer> oldPSConstantBuffer;
	ComPtr<ID3D11ShaderResourceView> oldVSSRV[3];
	context->VSGetConstantBuffers(0, 1, oldVSConstantBuffer.GetAddressOf());
	context->PSGetConstantBuffers(0, 1, oldPSConstantBuffer.GetAddressOf());
	context->VSGetShaderResources(0, 3, oldVSSRV[0].GetAddressOf());

	context->VSSetConstantBuffers(0, 1, _constantBuffer.GetAddressOf());
	context->PSSetConstantBuffers(0, 1, _constantBuffer.GetAddressOf());
	_deviceResources->InitRasterizerState(g_isInRenderLasers ? _rasterizerState : _rasterizerStateCull);
	_deviceResources->InitSamplerState(_samplerState.GetAddressOf(), nullptr);

	if (scene->TextureAlphaMask == 0)
	{
		_deviceResources->InitBlendState(_solidBlendState, nullptr);
		_deviceResources->InitDepthStencilState(_solidDepthState, nullptr);
	}
	else
	{
		_deviceResources->InitBlendState(_transparentBlendState, nullptr);
		_deviceResources->InitDepthStencilState(_transparentDepthState, nullptr);
	}

	_deviceResources->InitViewport(&_viewport);
	_deviceResources->InitTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	_deviceResources->InitInputLayout(_inputLayout);
	_deviceResources->InitVertexShader(_vertexShader);
	_deviceResources->InitPixelShader(g_config.D3dRendererHookShowNormals ? _normalsPixelShader : _pixelShader);

	UpdateTextures(scene);
	UpdateMeshBuffers(scene);
	UpdateVertexAndIndexBuffers(scene);
	UpdateConstantBuffer(scene);
	RenderScene();

	context->VSSetConstantBuffers(0, 1, oldVSConstantBuffer.GetAddressOf());
	context->PSSetConstantBuffers(0, 1, oldPSConstantBuffer.GetAddressOf());
	context->VSSetShaderResources(0, 3, oldVSSRV[0].GetAddressOf());

#if LOGGER_DUMP
	DumpConstants(_constants);
	DumpSurface();
	DumpVector3(scene->MeshVertices, *(int*)((int)scene->MeshVertices - 8));
	DumpTextureVertices(scene->MeshTextureVertices, *(int*)((int)scene->MeshTextureVertices - 8));
	DumpD3dVertices(_vertices.data(), _verticesCount);
#endif
}

void D3dRenderer::HangarShadowSceneHook(const SceneCompData* scene)
{
	ID3D11DeviceContext* context = _deviceResources->_d3dDeviceContext;

	ComPtr<ID3D11Buffer> oldVSConstantBuffer;
	ComPtr<ID3D11Buffer> oldPSConstantBuffer;
	ComPtr<ID3D11ShaderResourceView> oldVSSRV[3];
	context->VSGetConstantBuffers(0, 1, oldVSConstantBuffer.GetAddressOf());
	context->PSGetConstantBuffers(0, 1, oldPSConstantBuffer.GetAddressOf());
	context->VSGetShaderResources(0, 3, oldVSSRV[0].GetAddressOf());

	context->VSSetConstantBuffers(0, 1, _constantBuffer.GetAddressOf());
	context->PSSetConstantBuffers(0, 1, _constantBuffer.GetAddressOf());
	_deviceResources->InitRasterizerState(_rasterizerState);
	_deviceResources->InitSamplerState(_samplerState.GetAddressOf(), nullptr);

	_deviceResources->InitBlendState(_solidBlendState, nullptr);
	_deviceResources->InitDepthStencilState(_solidDepthState, nullptr, 0U);

	_deviceResources->InitViewport(&_viewport);
	_deviceResources->InitTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	_deviceResources->InitInputLayout(_inputLayout);
	_deviceResources->InitVertexShader(_shadowVertexShader);
	_deviceResources->InitPixelShader(_shadowPixelShader);

	UpdateTextures(scene);
	UpdateMeshBuffers(scene);
	UpdateVertexAndIndexBuffers(scene);
	UpdateConstantBuffer(scene);
	RenderScene();

	context->VSSetConstantBuffers(0, 1, oldVSConstantBuffer.GetAddressOf());
	context->PSSetConstantBuffers(0, 1, oldPSConstantBuffer.GetAddressOf());
	context->VSSetShaderResources(0, 3, oldVSSRV[0].GetAddressOf());
}

void D3dRenderer::UpdateTextures(const SceneCompData* scene)
{
	const unsigned char ShipCategory_PlayerProjectile = 6;
	const unsigned char ShipCategory_OtherProjectile = 7;
	unsigned char category = scene->pObject->ShipCategory;
	bool isProjectile = category == ShipCategory_PlayerProjectile || category == ShipCategory_OtherProjectile;

	const auto XwaD3dTextureCacheUpdateOrAdd = (void(*)(XwaTextureSurface*))0x00597784;
	const auto L00432750 = (short(*)(unsigned short, short, short))0x00432750;
	const ExeEnableEntry* s_ExeEnableTable = (ExeEnableEntry*)0x005FB240;

	XwaTextureSurface* surface = nullptr;
	XwaTextureSurface* surface2 = nullptr;

	_constants.renderType = 0;
	_constants.renderTypeIllum = 0;

	if (g_isInRenderLasers)
	{
		_constants.renderType = 2;
	}

	if (isProjectile)
	{
		_constants.renderType = 2;
	}

	if (scene->D3DInfo != nullptr)
	{
		surface = scene->D3DInfo->ColorMap[0];

		if (scene->D3DInfo->LightMap[0] != nullptr)
		{
			surface2 = scene->D3DInfo->LightMap[0];
			_constants.renderTypeIllum = 1;
		}
	}
	else
	{
		const unsigned short ModelIndex_237_1000_0_ResData_LightingEffects = 237;
		L00432750(ModelIndex_237_1000_0_ResData_LightingEffects, 0x02, 0x100);
		XwaSpeciesTMInfo* esi = (XwaSpeciesTMInfo*)s_ExeEnableTable[ModelIndex_237_1000_0_ResData_LightingEffects].pData1;
		surface = (XwaTextureSurface*)esi->pData;
	}

	if (scene->D3DInfo != nullptr && scene->D3DInfo->MipMapsCount < 0)
	{
		ID3D11ShaderResourceView* colorMap = (ID3D11ShaderResourceView*)scene->D3DInfo->ColorMap[0];
		ID3D11ShaderResourceView* lightMap = (ID3D11ShaderResourceView*)scene->D3DInfo->LightMap[0];

		_deviceResources->InitPSShaderResourceView(colorMap, lightMap);
	}
	else
	{
		XwaD3dTextureCacheUpdateOrAdd(surface);

		if (surface2 != nullptr)
		{
			XwaD3dTextureCacheUpdateOrAdd(surface2);
		}

		Direct3DTexture* texture = (Direct3DTexture*)surface->D3dTexture.D3DTextureHandle;
		Direct3DTexture* texture2 = surface2 == nullptr ? nullptr : (Direct3DTexture*)surface2->D3dTexture.D3DTextureHandle;
		_deviceResources->InitPSShaderResourceView(texture->_textureView, texture2 == nullptr ? nullptr : texture2->_textureView.Get());

#if LOGGER_DUMP
		g_currentTextureName = texture->_name;
#endif
	}
}

void D3dRenderer::UpdateMeshBuffers(const SceneCompData* scene)
{
	ID3D11Device* device = _deviceResources->_d3dDevice;
	ID3D11DeviceContext* context = _deviceResources->_d3dDeviceContext;

	XwaVector3* vertices = scene->MeshVertices;
	XwaVector3* normals = scene->MeshVertexNormals;
	XwaTextureVertex* textureCoords = scene->MeshTextureVertices;

	if (vertices != _lastMeshVertices)
	{
		_lastMeshVertices = vertices;
		_currentFaceIndex = 0;

		auto it = _meshVerticesViews.find((int)vertices);

		if (it != _meshVerticesViews.end())
		{
			_lastMeshVerticesView = it->second;
		}
		else
		{
			int verticesCount = *(int*)((int)vertices - 8);

			D3D11_SUBRESOURCE_DATA initialData;
			initialData.pSysMem = vertices;
			initialData.SysMemPitch = 0;
			initialData.SysMemSlicePitch = 0;

			ComPtr<ID3D11Buffer> meshVerticesBuffer;
			CD3D11_BUFFER_DESC meshVerticesBufferDesc(verticesCount * sizeof(XwaVector3), D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_IMMUTABLE);
			device->CreateBuffer(&meshVerticesBufferDesc, &initialData, &meshVerticesBuffer);

			ID3D11ShaderResourceView* meshVerticesView;
			CD3D11_SHADER_RESOURCE_VIEW_DESC meshVerticesViewDesc(meshVerticesBuffer, DXGI_FORMAT_R32G32B32_FLOAT, 0, verticesCount);
			device->CreateShaderResourceView(meshVerticesBuffer, &meshVerticesViewDesc, &meshVerticesView);

			_meshVerticesViews.insert(std::make_pair((int)vertices, meshVerticesView));
			_lastMeshVerticesView = meshVerticesView;
		}
	}

	if (normals != _lastMeshVertexNormals)
	{
		_lastMeshVertexNormals = normals;

		auto it = _meshNormalsViews.find((int)normals);

		if (it != _meshNormalsViews.end())
		{
			_lastMeshVertexNormalsView = it->second;
		}
		else
		{
			int normalsCount = *(int*)((int)normals - 8);

			D3D11_SUBRESOURCE_DATA initialData;
			initialData.pSysMem = normals;
			initialData.SysMemPitch = 0;
			initialData.SysMemSlicePitch = 0;

			ComPtr<ID3D11Buffer> meshNormalsBuffer;
			CD3D11_BUFFER_DESC meshNormalsBufferDesc(normalsCount * sizeof(XwaVector3), D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_IMMUTABLE);
			device->CreateBuffer(&meshNormalsBufferDesc, &initialData, &meshNormalsBuffer);

			ID3D11ShaderResourceView* meshNormalsView;
			CD3D11_SHADER_RESOURCE_VIEW_DESC meshNormalsViewDesc(meshNormalsBuffer, DXGI_FORMAT_R32G32B32_FLOAT, 0, normalsCount);
			device->CreateShaderResourceView(meshNormalsBuffer, &meshNormalsViewDesc, &meshNormalsView);

			_meshNormalsViews.insert(std::make_pair((int)normals, meshNormalsView));
			_lastMeshVertexNormalsView = meshNormalsView;
		}
	}

	if (textureCoords != _lastMeshTextureVertices)
	{
		_lastMeshTextureVertices = textureCoords;

		auto it = _meshTextureCoordsViews.find((int)textureCoords);

		if (it != _meshTextureCoordsViews.end())
		{
			_lastMeshTextureVerticesView = it->second;
		}
		else
		{
			int textureCoordsCount = *(int*)((int)textureCoords - 8);

			D3D11_SUBRESOURCE_DATA initialData;
			initialData.pSysMem = textureCoords;
			initialData.SysMemPitch = 0;
			initialData.SysMemSlicePitch = 0;

			ComPtr<ID3D11Buffer> meshTextureCoordsBuffer;
			CD3D11_BUFFER_DESC meshTextureCoordsBufferDesc(textureCoordsCount * sizeof(XwaTextureVertex), D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_IMMUTABLE);
			device->CreateBuffer(&meshTextureCoordsBufferDesc, &initialData, &meshTextureCoordsBuffer);

			ID3D11ShaderResourceView* meshTextureCoordsView;
			CD3D11_SHADER_RESOURCE_VIEW_DESC meshTextureCoordsViewDesc(meshTextureCoordsBuffer, DXGI_FORMAT_R32G32_FLOAT, 0, textureCoordsCount);
			device->CreateShaderResourceView(meshTextureCoordsBuffer, &meshTextureCoordsViewDesc, &meshTextureCoordsView);

			_meshTextureCoordsViews.insert(std::make_pair((int)textureCoords, meshTextureCoordsView));
			_lastMeshTextureVerticesView = meshTextureCoordsView;
		}
	}

	ID3D11ShaderResourceView* vsSSRV[3] = { _lastMeshVerticesView, _lastMeshVertexNormalsView, _lastMeshTextureVerticesView };
	context->VSSetShaderResources(0, 3, vsSSRV);
}

void D3dRenderer::ResizeDataVector(const SceneCompData* scene)
{
	size_t verticesRequiredSize = scene->FacesCount * 4;
	size_t trianglesRequiredSize = scene->FacesCount * 2;

	if (_vertices.capacity() < verticesRequiredSize)
	{
		_vertices.reserve(verticesRequiredSize * 2);
	}

	if (_triangles.capacity() < trianglesRequiredSize)
	{
		_triangles.reserve(trianglesRequiredSize * 2);
	}
}

void D3dRenderer::CreateDataScene(const SceneCompData* scene)
{
	const unsigned char ShipCategory_PlayerProjectile = 6;
	const unsigned char ShipCategory_OtherProjectile = 7;
	unsigned char category = scene->pObject->ShipCategory;
	int isProjectile = (category == ShipCategory_PlayerProjectile || category == ShipCategory_OtherProjectile) ? 1 : 0;

	D3dVertex* v = _vertices.data();
	D3dTriangle* t = _triangles.data();
	int verticesIndex = 0;
	_verticesCount = 0;
	_trianglesCount = 0;

	for (int faceIndex = 0; faceIndex < scene->FacesCount; faceIndex++)
	{
		OptFaceDataNode_01_Data_Indices& faceData = scene->FaceIndices[faceIndex];
		int edgesCount = faceData.Edge[3] == -1 ? 3 : 4;

		for (int vertexIndex = 0; vertexIndex < edgesCount; vertexIndex++)
		{
			v->iV = faceData.Vertex[vertexIndex];
			v->iN = faceData.VertexNormal[vertexIndex];
			v->iT = faceData.TextureVertex[vertexIndex];
			//v->c = isProjectile;
			v->c = 0;
			v++;
		}

		_verticesCount += edgesCount;

		for (int edge = 2; edge < edgesCount; edge++)
		{
			t->v1 = verticesIndex;
			t->v2 = verticesIndex + edge - 1;
			t->v3 = verticesIndex + edge;
			t++;
		}

		verticesIndex += edgesCount;
		_trianglesCount += edgesCount - 2;
	}

	_totalVerticesCount += _verticesCount;
	_totalTrianglesCount += _trianglesCount;
}

void D3dRenderer::UpdateVertexAndIndexBuffers(const SceneCompData* scene)
{
	ID3D11Device* device = _deviceResources->_d3dDevice;
	OptFaceDataNode_01_Data_Indices* faceData = scene->FaceIndices;

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	int verticesCount;
	int trianglesCount;

	auto itVertex = _vertexBuffers.find((int)faceData);
	auto itTriangle = _triangleBuffers.find((int)faceData);
	auto itCounters = _vertexCounters.find((int)faceData);

	if (itVertex != _vertexBuffers.end())
	{
		vertexBuffer = itVertex->second;
		indexBuffer = itTriangle->second;
		verticesCount = itCounters->second.first;
		trianglesCount = itCounters->second.second;
	}
	else
	{
		ResizeDataVector(scene);
		CreateDataScene(scene);

		D3D11_SUBRESOURCE_DATA initialData;
		initialData.SysMemPitch = 0;
		initialData.SysMemSlicePitch = 0;

		initialData.pSysMem = _vertices.data();
		CD3D11_BUFFER_DESC vertexBufferDesc(_verticesCount * sizeof(D3dVertex), D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_IMMUTABLE);
		device->CreateBuffer(&vertexBufferDesc, &initialData, &vertexBuffer);

		initialData.pSysMem = _triangles.data();
		CD3D11_BUFFER_DESC indexBufferDesc(_trianglesCount * sizeof(D3dTriangle), D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_IMMUTABLE);
		device->CreateBuffer(&indexBufferDesc, &initialData, &indexBuffer);

		verticesCount = _verticesCount;
		trianglesCount = _trianglesCount;

		_vertexBuffers.insert(std::make_pair((int)faceData, vertexBuffer));
		_triangleBuffers.insert(std::make_pair((int)faceData, indexBuffer));
		_vertexCounters.insert(std::make_pair((int)faceData, std::make_pair(_verticesCount, _trianglesCount)));
	}

	UINT vertexBufferStride = sizeof(D3dVertex);
	UINT vertexBufferOffset = 0;
	_deviceResources->InitVertexBuffer(nullptr, nullptr, nullptr);
	_deviceResources->InitVertexBuffer(&vertexBuffer, &vertexBufferStride, &vertexBufferOffset);
	_deviceResources->InitIndexBuffer(nullptr, true);
	_deviceResources->InitIndexBuffer(indexBuffer, true);

	_verticesCount = verticesCount;
	_trianglesCount = trianglesCount;

	unsigned int& s_SceneNumVerts = *(unsigned int*)0x009EAA00;
	unsigned int& s_SceneNumTris = *(unsigned int*)0x009EA96C;
	unsigned int& s_SceneNumTexChanges = *(unsigned int*)0x009EA984;
	unsigned int& s_SceneNumStateChanges = *(unsigned int*)0x009EA980;

	s_SceneNumVerts += verticesCount;
	s_SceneNumTris += trianglesCount;
	s_SceneNumStateChanges += 1;
}

void D3dRenderer::UpdateConstantBuffer(const SceneCompData* scene)
{
	ID3D11DeviceContext* context = _deviceResources->_d3dDeviceContext;

	_constants.projectionParameterA = g_config.ProjectionParameterA;
	_constants.projectionParameterB = g_config.ProjectionParameterB;
	_constants.projectionParameterC = g_config.ProjectionParameterC;
	_constants.projectionParameterD = 0;

	switch (g_rendererType)
	{
	case RendererType_Main:
	default:
		_constants.projectionValue1 = *(float*)0x08B94CC;
		_constants.projectionValue2 = *(float*)0x05B46B4;
		_constants.projectionDeltaX = *(float*)0x08C1600 + *(float*)0x0686ACC;
		_constants.projectionDeltaY = *(float*)0x080ACF8 + *(float*)0x07B33C0 + *(float*)0x064D1AC;
		_constants.floorLevel = 0.0f;
		_constants.cameraPositionX = (float)(*(int*)(0x08B94E0 + *(int*)0x08C1CC8 * 0xBCF + 0xB48 + 0x00));
		_constants.cameraPositionY = (float)(*(int*)(0x08B94E0 + *(int*)0x08C1CC8 * 0xBCF + 0xB48 + 0x04));
		_constants.hangarShadowAccStartEnd = 0.0f;
		break;

	case RendererType_Shadow:
		_constants.projectionValue1 = *(float*)0x08B94CC;
		_constants.projectionValue2 = *(float*)0x05B46B4;
		_constants.projectionDeltaX = (float)(*(short*)0x08052B8 / 2) + *(float*)0x0686ACC;
		_constants.projectionDeltaY = (float)(*(short*)0x07B33BC / 2 + *(int*)0x080811C) + *(float*)0x064D1AC;
		_constants.floorLevel = *(float*)0x0686B34 - -1.5f - (float)(*(int*)(0x08B94E0 + *(int*)0x08C1CC8 * 0xBCF + 0xB48 + 0x08));
		_constants.cameraPositionX = (float)(*(int*)(0x08B94E0 + *(int*)0x08C1CC8 * 0xBCF + 0xB48 + 0x00));
		_constants.cameraPositionY = (float)(*(int*)(0x08B94E0 + *(int*)0x08C1CC8 * 0xBCF + 0xB48 + 0x04));
		_constants.hangarShadowAccStartEnd = *(short*)0x0686B38 != 0 ? 1.0f : 0.0f;
		_constants.s_V0x0662814 = *(float*)0x0662814;
		_constants.s_V0x064D1A4 = *(float*)0x064D1A4;
		_constants.s_V0x06626F4 = *(float*)0x06626F4;
		_constants.s_V0x063D194 = *(float*)0x063D194;
		break;
	}

	_constants.transformView[0] = *(float*)0x007B4BEC;
	_constants.transformView[1] = *(float*)0x007B6FF8;
	_constants.transformView[2] = *(float*)0x007B33DC;
	_constants.transformView[3] = 0.0f;
	_constants.transformView[4] = *(float*)0x007B4BE8;
	_constants.transformView[5] = *(float*)0x007B6FF0;
	_constants.transformView[6] = *(float*)0x007B33D8;
	_constants.transformView[7] = 0.0f;
	_constants.transformView[8] = *(float*)0x007B4BF4;
	_constants.transformView[9] = *(float*)0x007B33D4;
	_constants.transformView[10] = *(float*)0x007B4BE4;
	_constants.transformView[11] = 0.0f;
	_constants.transformView[12] = 0.0f;
	_constants.transformView[13] = 0.0f;
	_constants.transformView[14] = 0.0f;
	_constants.transformView[15] = 1.0f;

	_constants.transformWorldView[0] = scene->WorldViewTransform.Rotation._11;
	_constants.transformWorldView[1] = scene->WorldViewTransform.Rotation._21;
	_constants.transformWorldView[2] = scene->WorldViewTransform.Rotation._31;
	_constants.transformWorldView[3] = scene->WorldViewTransform.Position.x;
	_constants.transformWorldView[4] = scene->WorldViewTransform.Rotation._12;
	_constants.transformWorldView[5] = scene->WorldViewTransform.Rotation._22;
	_constants.transformWorldView[6] = scene->WorldViewTransform.Rotation._32;
	_constants.transformWorldView[7] = scene->WorldViewTransform.Position.y;
	_constants.transformWorldView[8] = scene->WorldViewTransform.Rotation._13;
	_constants.transformWorldView[9] = scene->WorldViewTransform.Rotation._23;
	_constants.transformWorldView[10] = scene->WorldViewTransform.Rotation._33;
	_constants.transformWorldView[11] = scene->WorldViewTransform.Position.z;
	_constants.transformWorldView[12] = 0.0f;
	_constants.transformWorldView[13] = 0.0f;
	_constants.transformWorldView[14] = 0.0f;
	_constants.transformWorldView[15] = 1.0f;

	int s_XwaGlobalLightsCount = *(int*)0x00782848;
	XwaGlobalLight* s_XwaGlobalLights = (XwaGlobalLight*)0x007D4FA0;
	int s_XwaLocalLightsCount = *(int*)0x00782844;
	XwaLocalLight* s_XwaLocalLights = (XwaLocalLight*)0x008D42C0;

	_constants.globalLightsCount = s_XwaGlobalLightsCount;

	for (int i = 0; i < 8; i++)
	{
		_constants.globalLights[i].direction[0] = s_XwaGlobalLights[i].DirectionX;
		_constants.globalLights[i].direction[1] = s_XwaGlobalLights[i].DirectionY;
		_constants.globalLights[i].direction[2] = s_XwaGlobalLights[i].DirectionZ;
		_constants.globalLights[i].direction[3] = 1.0f;

		_constants.globalLights[i].color[0] = s_XwaGlobalLights[i].ColorR;
		_constants.globalLights[i].color[1] = s_XwaGlobalLights[i].ColorG;
		_constants.globalLights[i].color[2] = s_XwaGlobalLights[i].ColorB;
		_constants.globalLights[i].color[3] = s_XwaGlobalLights[i].Intensity;
	}

	_constants.localLightsCount = s_XwaLocalLightsCount;

	for (int i = 0; i < 8; i++)
	{
		_constants.localLights[i].position[0] = s_XwaLocalLights[i].fPositionX;
		_constants.localLights[i].position[1] = s_XwaLocalLights[i].fPositionY;
		_constants.localLights[i].position[2] = s_XwaLocalLights[i].fPositionZ;
		_constants.localLights[i].position[3] = 1.0f;

		_constants.localLights[i].color[0] = s_XwaLocalLights[i].ColorR;
		_constants.localLights[i].color[1] = s_XwaLocalLights[i].ColorG;
		_constants.localLights[i].color[2] = s_XwaLocalLights[i].ColorB;
		_constants.localLights[i].color[3] = s_XwaLocalLights[i].m18;
	}

	context->UpdateSubresource(_constantBuffer, 0, nullptr, &_constants, 0, 0);
}

void D3dRenderer::RenderScene()
{
	if (_deviceResources->_displayWidth == 0 || _deviceResources->_displayHeight == 0)
	{
		return;
	}

	ID3D11DeviceContext* context = _deviceResources->_d3dDeviceContext;
	unsigned short scissorLeft = *(unsigned short*)0x07D5244;
	unsigned short scissorTop = *(unsigned short*)0x07CA354;
	unsigned short scissorWidth = *(unsigned short*)0x08052B8;
	unsigned short scissorHeight = *(unsigned short*)0x07B33BC;
	float scaleX = _viewport.Width / _deviceResources->_displayWidth;
	float scaleY = _viewport.Height / _deviceResources->_displayHeight;

	if (_deviceResources->IsInConcourseHd())
	{
		scissorLeft = 0;
		scissorTop = 0;
		scissorWidth = 640;
		scissorHeight = 480;
	}

	D3D11_RECT scissor{};
	scissor.left = (LONG)(_viewport.TopLeftX + scissorLeft * scaleX + 0.5f);
	scissor.top = (LONG)(_viewport.TopLeftY + scissorTop * scaleY + 0.5f);
	scissor.right = scissor.left + (LONG)(scissorWidth * scaleX + 0.5f);
	scissor.bottom = scissor.top + (LONG)(scissorHeight * scaleY + 0.5f);
	_deviceResources->InitScissorRect(&scissor);

	context->DrawIndexed(_trianglesCount * 3, 0, 0);
}

void D3dRenderer::BuildGlowMarks(SceneCompData* scene)
{
	_glowMarksVertices.clear();
	_glowMarksTriangles.clear();

	const auto XwaD3dTextureCacheUpdateOrAdd = (void(*)(XwaTextureSurface*))0x00597784;
	const auto XwaVector3Transform = (void(*)(XwaVector3*, const XwaMatrix3x3*))0x00439B30;
	const auto L004E8110 = (void(*)(SceneCompData*, XwaKnockoutData*))0x004E8110;

	const XwaObject* s_XwaObjects = *(XwaObject**)0x007B33C4;
	const XwaObject3D* s_XwaObjects3D = *(XwaObject3D**)0x00782854;
	const int s_XwaIsInConcourse = *(int*)0x005FFD9C;

	scene->GlowMarksCount = 0;

	if (s_XwaIsInConcourse == 0)
	{
		int objectIndex = scene->pObject - s_XwaObjects;
		XwaKnockoutData* knockout = s_XwaObjects3D[objectIndex].pKnockoutData;

		if (knockout != nullptr)
		{
			L004E8110(scene, knockout);
		}
	}

	for (int glowMarkIndex = 0; glowMarkIndex < scene->GlowMarksCount; glowMarkIndex++)
	{
		XwaGlowMark* glowMark = scene->GlowMarks[glowMarkIndex];

		for (int glowMarkItemIndex = 0; glowMarkItemIndex < glowMark->Count; glowMarkItemIndex++)
		{
			XwaGlowMarkItem* glowMarkItem = &glowMark->Items[glowMarkItemIndex];

			if (glowMarkItem->pTextureSurface == nullptr)
			{
				continue;
			}

			XwaD3dTextureCacheUpdateOrAdd(glowMarkItem->pTextureSurface);

			int baseFaceIndex = _currentFaceIndex;

			for (int faceIndex = 0; faceIndex < scene->FacesCount; faceIndex++)
			{
				unsigned short faceOn = glowMark->FaceOn[baseFaceIndex + faceIndex];

				if (faceOn == 0)
				{
					continue;
				}

				OptFaceDataNode_01_Data_Indices& faceData = scene->FaceIndices[faceIndex];
				int edgesCount = faceData.Edge[3] == -1 ? 3 : 4;
				int verticesIndex = _glowMarksVertices.size();

				for (int vertexIndex = 0; vertexIndex < edgesCount; vertexIndex++)
				{
					int iV = faceData.Vertex[vertexIndex];
					int iN = faceData.VertexNormal[vertexIndex];
					int iT = faceData.TextureVertex[vertexIndex];

					XwaD3dVertex v;
					v.x = scene->MeshVertices[iV].x;
					v.y = scene->MeshVertices[iV].y;
					v.z = scene->MeshVertices[iV].z;
					v.rhw = 0;
					v.color = glowMarkItem->Color;
					v.specular = 0;
					v.tu = glowMark->UVArray[iV].u * glowMarkItem->Size + 0.5f;
					v.tv = glowMark->UVArray[iV].v * glowMarkItem->Size + 0.5f;

					XwaVector3Transform((XwaVector3*)&v.x, &scene->WorldViewTransform.Rotation);
					v.x += scene->WorldViewTransform.Position.x;
					v.y += scene->WorldViewTransform.Position.y;
					v.z += scene->WorldViewTransform.Position.z;

					float st0 = _constants.projectionValue1 / v.z;
					v.x = v.x * st0 + _constants.projectionDeltaX;
					v.y = v.y * st0 + _constants.projectionDeltaY;
					v.z = (st0 * _constants.projectionValue2) / (abs(st0) * _constants.projectionValue2 + _constants.projectionValue1);
					v.rhw = st0;

					_glowMarksVertices.push_back(v);
				}

				for (int edge = 2; edge < edgesCount; edge++)
				{
					XwaD3dTriangle t;

					t.v1 = verticesIndex;
					t.v2 = verticesIndex + edge - 1;
					t.v3 = verticesIndex + edge;
					t.RenderStatesFlags = *(unsigned int*)0x06626F0;
					t.pTexture = &glowMarkItem->pTextureSurface->D3dTexture;

					_glowMarksTriangles.push_back(t);
				}
			}
		}
	}

	_currentFaceIndex += scene->FacesCount;
}

void D3dRenderer::RenderGlowMarks()
{
	if (_glowMarksTriangles.size() == 0)
	{
		return;
	}

	const auto XwaD3dExecuteBufferLock = (char(*)())0x00595006;
	const auto XwaD3dExecuteBufferAddVertices = (char(*)(void*, int))0x00595095;
	const auto XwaD3dExecuteBufferProcessVertices = (char(*)())0x00595106;
	const auto XwaD3dExecuteBufferAddTriangles = (char(*)(void*, int))0x00595191;
	const auto XwaD3dExecuteBufferUnlockAndExecute = (char(*)())0x005954D6;

	XwaD3dExecuteBufferLock();
	XwaD3dExecuteBufferAddVertices(_glowMarksVertices.data(), _glowMarksVertices.size());
	XwaD3dExecuteBufferProcessVertices();
	XwaD3dExecuteBufferAddTriangles(_glowMarksTriangles.data(), _glowMarksTriangles.size());
	XwaD3dExecuteBufferUnlockAndExecute();

	_glowMarksVertices.clear();
	_glowMarksTriangles.clear();
}

void D3dRenderer::Initialize()
{
	CreateConstantBuffer();
	CreateStates();
	CreateShaders();
}

void D3dRenderer::CreateConstantBuffer()
{
	ID3D11Device* device = _deviceResources->_d3dDevice;

	// constant buffer
	CD3D11_BUFFER_DESC _constantBufferDesc(sizeof(D3dConstants), D3D11_BIND_CONSTANT_BUFFER);
	device->CreateBuffer(&_constantBufferDesc, nullptr, &_constantBuffer);
}

void D3dRenderer::CreateStates()
{
	ID3D11Device* device = _deviceResources->_d3dDevice;

	D3D11_RASTERIZER_DESC rsDesc{};
	rsDesc.FillMode = g_config.WireframeFillMode ? D3D11_FILL_WIREFRAME : D3D11_FILL_SOLID;
	rsDesc.CullMode = D3D11_CULL_NONE;
	rsDesc.FrontCounterClockwise = FALSE;
	rsDesc.DepthBias = 0;
	rsDesc.DepthBiasClamp = 0.0f;
	rsDesc.SlopeScaledDepthBias = 0.0f;
	rsDesc.DepthClipEnable = TRUE;
	rsDesc.ScissorEnable = TRUE;
	rsDesc.MultisampleEnable = FALSE;
	rsDesc.AntialiasedLineEnable = FALSE;
	device->CreateRasterizerState(&rsDesc, &_rasterizerState);

	rsDesc.CullMode = D3D11_CULL_FRONT;
	device->CreateRasterizerState(&rsDesc, &_rasterizerStateCull);

	rsDesc.FillMode = D3D11_FILL_WIREFRAME;
	rsDesc.CullMode = D3D11_CULL_NONE;
	device->CreateRasterizerState(&rsDesc, &_rasterizerStateWireframe);

	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = _deviceResources->_useAnisotropy ? D3D11_FILTER_ANISOTROPIC : D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.MaxAnisotropy = _deviceResources->_useAnisotropy ? _deviceResources->GetMaxAnisotropy() : 1;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = FLT_MAX;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.BorderColor[0] = 0.0f;
	samplerDesc.BorderColor[1] = 0.0f;
	samplerDesc.BorderColor[2] = 0.0f;
	samplerDesc.BorderColor[3] = 0.0f;
	device->CreateSamplerState(&samplerDesc, &_samplerState);

	D3D11_BLEND_DESC solidBlendDesc{};
	solidBlendDesc.AlphaToCoverageEnable = FALSE;
	solidBlendDesc.IndependentBlendEnable = FALSE;
	solidBlendDesc.RenderTarget[0].BlendEnable = FALSE;
	solidBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	solidBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	solidBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	solidBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	solidBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	solidBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	solidBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	device->CreateBlendState(&solidBlendDesc, &_solidBlendState);

	D3D11_DEPTH_STENCIL_DESC solidDepthDesc{};
	solidDepthDesc.DepthEnable = TRUE;
	solidDepthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	solidDepthDesc.DepthFunc = D3D11_COMPARISON_GREATER;
	solidDepthDesc.StencilEnable = FALSE;
	device->CreateDepthStencilState(&solidDepthDesc, &_solidDepthState);

	D3D11_BLEND_DESC transparentBlendDesc{};
	transparentBlendDesc.AlphaToCoverageEnable = FALSE;
	transparentBlendDesc.IndependentBlendEnable = FALSE;
	transparentBlendDesc.RenderTarget[0].BlendEnable = TRUE;
	transparentBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	transparentBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	transparentBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	transparentBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
	transparentBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	transparentBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	transparentBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	device->CreateBlendState(&transparentBlendDesc, &_transparentBlendState);

	D3D11_DEPTH_STENCIL_DESC transparentDepthDesc{};
	transparentDepthDesc.DepthEnable = TRUE;
	transparentDepthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	transparentDepthDesc.DepthFunc = D3D11_COMPARISON_GREATER;
	transparentDepthDesc.StencilEnable = FALSE;
	device->CreateDepthStencilState(&transparentDepthDesc, &_transparentDepthState);
}

void D3dRenderer::CreateShaders()
{
	ID3D11Device* device = _deviceResources->_d3dDevice;

	device->CreateVertexShader(g_XwaD3dVertexShader, sizeof(g_XwaD3dVertexShader), nullptr, &_vertexShader);

	const D3D11_INPUT_ELEMENT_DESC vertexLayoutDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	device->CreateInputLayout(vertexLayoutDesc, ARRAYSIZE(vertexLayoutDesc), g_XwaD3dVertexShader, sizeof(g_XwaD3dVertexShader), &_inputLayout);

	device->CreatePixelShader(g_XwaD3dPixelShader, sizeof(g_XwaD3dPixelShader), nullptr, &_pixelShader);
	device->CreatePixelShader(g_XwaD3dNormalsPixelShader, sizeof(g_XwaD3dNormalsPixelShader), nullptr, &_normalsPixelShader);

	device->CreateVertexShader(g_XwaD3dShadowVertexShader, sizeof(g_XwaD3dShadowVertexShader), nullptr, &_shadowVertexShader);
	device->CreatePixelShader(g_XwaD3dShadowPixelShader, sizeof(g_XwaD3dShadowPixelShader), nullptr, &_shadowPixelShader);
}

void D3dRenderer::GetViewport(D3D11_VIEWPORT* viewport)
{
	UINT w;
	UINT h;

	if (g_config.AspectRatioPreserved)
	{
		if (_deviceResources->_backbufferHeight * _deviceResources->_displayWidth <= _deviceResources->_backbufferWidth * _deviceResources->_displayHeight)
		{
			w = _deviceResources->_backbufferHeight * _deviceResources->_displayWidth / _deviceResources->_displayHeight;
			h = _deviceResources->_backbufferHeight;
		}
		else
		{
			w = _deviceResources->_backbufferWidth;
			h = _deviceResources->_backbufferWidth * _deviceResources->_displayHeight / _deviceResources->_displayWidth;
		}
	}
	else
	{
		w = _deviceResources->_backbufferWidth;
		h = _deviceResources->_backbufferHeight;
	}

	UINT left = (_deviceResources->_backbufferWidth - w) / 2;
	UINT top = (_deviceResources->_backbufferHeight - h) / 2;

	viewport->TopLeftX = (float)left;
	viewport->TopLeftY = (float)top;
	viewport->Width = (float)w;
	viewport->Height = (float)h;
	viewport->MinDepth = D3D11_MIN_DEPTH;
	viewport->MaxDepth = D3D11_MAX_DEPTH;
}

void D3dRenderer::GetViewportScale(float* viewportScale)
{
	float scale;

	if (_deviceResources->_frontbufferSurface == nullptr)
	{
		scale = 1.0f;
	}
	else
	{
		if (_deviceResources->_backbufferHeight * _deviceResources->_displayWidth <= _deviceResources->_backbufferWidth * _deviceResources->_displayHeight)
		{
			scale = (float)_deviceResources->_backbufferHeight / (float)_deviceResources->_displayHeight;
		}
		else
		{
			scale = (float)_deviceResources->_backbufferWidth / (float)_deviceResources->_displayWidth;
		}

		scale *= g_config.Concourse3DScale;
	}

	viewportScale[0] = 2.0f / (float)_deviceResources->_displayWidth;
	viewportScale[1] = -2.0f / (float)_deviceResources->_displayHeight;
	viewportScale[2] = scale;
	viewportScale[3] = 0.0f;
}

D3dRenderer* g_xwa_d3d_renderer = nullptr;

void D3dRendererInitialize()
{
	if (!g_config.D3dRendererHookEnabled)
	{
		return;
	}

	if (!g_xwa_d3d_renderer)
	{
		g_xwa_d3d_renderer = new D3dRenderer();
	}
}

void D3dRendererUninitialize()
{
	if (!g_config.D3dRendererHookEnabled)
	{
		return;
	}

	if (g_xwa_d3d_renderer)
	{
		delete g_xwa_d3d_renderer;
		g_xwa_d3d_renderer = nullptr;
	}
}

void D3dRendererSceneBegin(DeviceResources* deviceResources)
{
	if (!g_config.D3dRendererHookEnabled)
	{
		return;
	}

	g_xwa_d3d_renderer->SceneBegin(deviceResources);
}

void D3dRendererSceneEnd()
{
	if (!g_config.D3dRendererHookEnabled)
	{
		return;
	}

	g_xwa_d3d_renderer->SceneEnd();
}

void D3dRendererFlightStart()
{
	if (!g_config.D3dRendererHookEnabled)
	{
		return;
	}

	g_xwa_d3d_renderer->FlightStart();
}

void D3dRenderLasersHook(int A4)
{
	const auto L0042BBA0 = (void(*)(int))0x0042BBA0;

	g_isInRenderLasers = true;
	L0042BBA0(A4);
	g_isInRenderLasers = false;
}

void D3dRenderMiniatureHook(int A4, int A8, int AC, int A10, int A14)
{
	const auto L00478490 = (void(*)(int, int, int, int, int))0x00478490;

	g_isInRenderMiniature = true;
	L00478490(A4, A8, AC, A10, A14);
	g_isInRenderMiniature = false;
}

void D3dRenderHyperspaceLinesHook(int A4)
{
	const auto L00480A80 = (void(*)(int))0x00480A80;

	g_isInRenderHyperspaceLines = true;
	L00480A80(A4);
	g_isInRenderHyperspaceLines = false;
}

void D3dRendererMainHook(SceneCompData* scene)
{
	if (*(int*)0x06628E0 != 0)
	{
		const auto XwaD3dExecuteBufferLock = (char(*)())0x00595006;
		const auto XwaD3dExecuteBufferAddVertices = (char(*)(void*, int))0x00595095;
		const auto XwaD3dExecuteBufferProcessVertices = (char(*)())0x00595106;
		const auto XwaD3dExecuteBufferAddTriangles = (char(*)(void*, int))0x00595191;
		const auto XwaD3dExecuteBufferUnlockAndExecute = (char(*)())0x005954D6;

		XwaD3dExecuteBufferLock();
		XwaD3dExecuteBufferAddVertices(*(void**)0x064D1A8, *(int*)0x06628E0);
		XwaD3dExecuteBufferProcessVertices();
		XwaD3dExecuteBufferAddTriangles(*(void**)0x066283C, *(int*)0x0686ABC);
		XwaD3dExecuteBufferUnlockAndExecute();

		*(int*)0x0686ABC = 0;
		*(int*)0x06628E0 = 0;
	}

	const auto L00480370 = (void(*)(const SceneCompData * scene))0x00480370;

	if (g_isInRenderHyperspaceLines)
	{
		L00480370(scene);
		return;
	}

	g_rendererType = RendererType_Main;
	g_xwa_d3d_renderer->MainSceneHook(scene);
	g_xwa_d3d_renderer->BuildGlowMarks(scene);
	g_xwa_d3d_renderer->RenderGlowMarks();
}

void D3dRendererShadowHook(SceneCompData* scene)
{
	g_rendererType = RendererType_Shadow;
	g_xwa_d3d_renderer->HangarShadowSceneHook(scene);
}

void D3dRendererOptLoadHook(int handle)
{
	const auto GetSizeFromHandle = (int(*)(int))0x0050E3B0;
	GetSizeFromHandle(handle);

	D3dRendererFlightStart();

	// XwaIOFileName contains the filename of the loaded 
	// const char* XwaIOFileName = (const char*)0x0080DA60;
}

void D3dRendererOptNodeHook(OptHeader* optHeader, int nodeIndex, SceneCompData* scene)
{
	const auto L00482000 = (void(*)(OptHeader*, OptNode*, SceneCompData*))0x00482000;

	OptNode* node = optHeader->Nodes[nodeIndex];
	g_xwa_d3d_renderer->_currentOptMeshIndex = (node->NodeType == OptNode_Texture || node->NodeType == OptNode_D3DTexture) ? (nodeIndex - 1) : nodeIndex;

	L00482000(optHeader, node, scene);
}

std::vector<XwaD3DInfo>& GetD3dInfosArray()
{
	static bool _initialized = false;
	static std::vector<XwaD3DInfo> d3dInfos;

	if (!_initialized)
	{
		d3dInfos.reserve(1000000);
		memset(d3dInfos.data(), 0, d3dInfos.capacity() * sizeof(XwaD3DInfo));
		_initialized = true;
	}

	return d3dInfos;
}

bool D3dInfosArrayContains(XwaD3DInfo* d3dInfo)
{
	auto& d3dInfosArray = GetD3dInfosArray();

	bool contains = (d3dInfo >= d3dInfosArray.data()) && (d3dInfo < d3dInfosArray.data() + d3dInfosArray.capacity());
	return contains;
}

XwaD3DInfo* GetD3dInfosArrayFirstEmpty()
{
	auto& d3dInfosArray = GetD3dInfosArray();

	for (int i = 0; i < d3dInfosArray.capacity(); i++)
	{
		if (d3dInfosArray.data()[i].MipMapsCount == 0)
		{
			return d3dInfosArray.data() + i;
		}
	}

	return nullptr;
}

void D3dInfosArrayRelease(XwaD3DInfo* d3dInfo)
{
	if (d3dInfo->TextureDescription.Palettes)
	{
		delete[] d3dInfo->TextureDescription.Palettes;
		d3dInfo->TextureDescription.Palettes = nullptr;
	}

	ID3D11ShaderResourceView* colorMap = (ID3D11ShaderResourceView*)d3dInfo->ColorMap[0];

	if (colorMap)
	{
		colorMap->Release();
	}

	ID3D11ShaderResourceView* lightMap = (ID3D11ShaderResourceView*)d3dInfo->LightMap[0];

	if (lightMap)
	{
		lightMap->Release();
	}

	memset(d3dInfo, 0, sizeof(XwaD3DInfo));
}

void D3dRleaseD3DINFO(XwaD3DInfo* d3dInfo)
{
	const auto XwaRleaseD3DINFO = (void(*)(XwaD3DInfo*))0x004417B0;

	if (D3dInfosArrayContains(d3dInfo))
	{
		D3dInfosArrayRelease(d3dInfo);
	}
	else
	{
		XwaRleaseD3DINFO(d3dInfo);
	}
}

void D3dOptSetSceneTextureTag(XwaD3DInfo* d3dInfo, OptNode* textureNode)
{
	const char* XwaIOFileName = (const char*)0x0080DA60;
	std::string textureTag = std::string(XwaIOFileName) + "," + textureNode->Name;
	d3dInfo->TextureDescription.Palettes = (unsigned short*)new char[textureTag.size() + 1];
	strcpy_s((char*)d3dInfo->TextureDescription.Palettes, textureTag.size() + 1, textureTag.c_str());
}

class ColorConvert
{
public:
	ColorConvert()
	{
		for (unsigned int color = 0; color < 0x10000; color++)
		{
			unsigned int r = (color >> 11) & 0x1f;
			unsigned int g = (color >> 5) & 0x3f;
			unsigned int b = color & 0x1f;

			r = (r * 539086 + 32768) >> 16;
			g = (g * 265264 + 32768) >> 16;
			b = (b * 539086 + 32768) >> 16;

			this->color16to32[color] = (r << 16) | (g << 8) | b;
		}
	}

	unsigned int color16to32[0x10000];
};

ColorConvert g_colorConvert;

std::string g_optName;
std::vector<unsigned char> g_lightMapBuffer;
std::vector<unsigned char> g_colorMapBuffer;
std::vector<unsigned char> g_illumMapBuffer;

HRESULT D3dOptCreateTextureColorLight(XwaD3DInfo* d3dInfo, OptNode* textureNode, int textureId, XwaTextureDescription* textureDescription, unsigned char* optTextureData, unsigned char* optTextureAlphaData, unsigned short* optPalette8, unsigned short* optPalette0)
{
	// code from the 32bpp hook
	const int XwaFlightBrightness = *(int*)0x006002C8;
	const int brightnessLevel = (XwaFlightBrightness - 0x100) / 0x40;

	int size = textureDescription->Width * textureDescription->Height;

	int bytesSize;

	if (size == textureDescription->TextureSize)
	{
		bytesSize = textureDescription->DataSize;
	}
	else
	{
		bytesSize = size;
	}

	int bpp = 0;

	if (bytesSize >= size && bytesSize < size * 2)
	{
		bpp = 8;
	}
	else if (bytesSize >= size * 4 && bytesSize < size * 8)
	{
		bpp = 32;
	}

	bool optHasAlpha = false;
	bool optHasIllum = false;

	if (bpp == 8)
	{
		if ((int)g_colorMapBuffer.capacity() < size * 8)
		{
			g_colorMapBuffer.reserve(size * 8);
		}

		if ((int)g_illumMapBuffer.capacity() < size * 8)
		{
			g_illumMapBuffer.reserve(size * 2);
		}

		bool hasAlpha = optTextureAlphaData != 0;
		bool hasIllum = false;

		unsigned char* illumBuffer = g_illumMapBuffer.data();
		unsigned char* colorBuffer = g_colorMapBuffer.data();

		for (int i = 0; i < bytesSize; i++)
		{
			int colorIndex = *(unsigned char*)(optTextureData + i);
			unsigned short color = optPalette0[4 * 256 + colorIndex];
			unsigned short color8 = optPalette0[8 * 256 + colorIndex];

			unsigned char r = (unsigned char)((color & 0xF800U) >> 11);
			unsigned char g = (unsigned char)((color & 0x7E0U) >> 5);
			unsigned char b = (unsigned char)(color & 0x1FU);

			if (r <= 8 && g <= 16 && b <= 8)
			{
				illumBuffer[i] = 0;
			}
			//else if (hasAlpha && *(unsigned char*)(optTextureAlphaData + i) != 255)
			//{
			//	illumBuffer[i] = 0;
			//}
			else if (color == color8)
			{
				hasIllum = true;
				illumBuffer[i] = 0x7f;
				//illumBuffer[i] = 0x3f;
			}
			else
			{
				illumBuffer[i] = 0;
			}
		}

		for (int i = 0; i < bytesSize; i++)
		{
			int paletteIndex = 4 + brightnessLevel;

			int colorIndex = *(unsigned char*)(optTextureData + i);
			//unsigned short color = optPalette8[colorIndex];
			unsigned short color = optPalette0[256 * paletteIndex + colorIndex];
			unsigned char illum = illumBuffer[i];

			//unsigned int a = (optTextureAlphaData == 0 || illum != 0) ? (unsigned char)255 : *(unsigned char*)(optTextureAlphaData + i);
			unsigned int a = optTextureAlphaData == 0 ? (unsigned char)255 : *(unsigned char*)(optTextureAlphaData + i);
			unsigned int color32 = g_colorConvert.color16to32[color];

			*(unsigned int*)(colorBuffer + i * 4) = (a << 24) | color32;
		}

		optHasAlpha = hasAlpha;
		optHasIllum = hasIllum;
	}
	else if (bpp == 32)
	{
		if ((int)g_colorMapBuffer.capacity() < size * 8)
		{
			g_colorMapBuffer.reserve(size * 8);
		}

		if ((int)g_illumMapBuffer.capacity() < size * 2)
		{
			g_illumMapBuffer.reserve(size * 2);
		}

		unsigned char* illumBuffer = g_illumMapBuffer.data();
		unsigned char* colorBuffer = g_colorMapBuffer.data();

		bool hasAlpha = ((char*)textureDescription->Palettes)[2] != 0;
		bool hasIllum = ((char*)textureDescription->Palettes)[4] != 0;

		if (!hasIllum)
		{
			optTextureAlphaData = 0;
		}

		bytesSize /= 4;

		for (int i = 0; i < bytesSize; i++)
		{
			int paletteIndex = 4 + brightnessLevel;
			unsigned char illum = optTextureAlphaData == 0 ? (unsigned char)0 : *(unsigned char*)(optTextureAlphaData + i);

			//illum = min(illum, 0x3f);

			//if (*(unsigned char*)(optTextureData + i * 4 + 3) != 255)
			//{
			//	illum = 0;
			//}

			illumBuffer[i] = illum;

			unsigned int b = *(unsigned char*)(optTextureData + i * 4 + 0);
			unsigned int g = *(unsigned char*)(optTextureData + i * 4 + 1);
			unsigned int r = *(unsigned char*)(optTextureData + i * 4 + 2);
			//unsigned int a = illum != 0 ? (unsigned char)255 : *(unsigned char*)(optTextureData + i * 4 + 3);
			unsigned int a = *(unsigned char*)(optTextureData + i * 4 + 3);

			if (illum == 0)
			{
				b = (b * 128 * paletteIndex / 8 + b * 128) / 256;
				g = (g * 128 * paletteIndex / 8 + g * 128) / 256;
				r = (r * 128 * paletteIndex / 8 + r * 128) / 256;
			}

			*(unsigned char*)(colorBuffer + i * 4 + 0) = b;
			*(unsigned char*)(colorBuffer + i * 4 + 1) = g;
			*(unsigned char*)(colorBuffer + i * 4 + 2) = r;
			*(unsigned char*)(colorBuffer + i * 4 + 3) = a;
		}

		optHasAlpha = hasAlpha;
		optHasIllum = hasIllum;
	}
	else
	{
		return E_FAIL;
	}

	HRESULT hr = S_OK;

	D3D11_TEXTURE2D_DESC textureDesc{};
	textureDesc.Width = textureDescription->Width;
	textureDesc.Height = textureDescription->Height;
	textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	D3D11_SUBRESOURCE_DATA textureData{};
	textureData.pSysMem = g_colorMapBuffer.data();
	textureData.SysMemPitch = textureDesc.Width * 4;
	textureData.SysMemSlicePitch = 0;

	D3D11_SHADER_RESOURCE_VIEW_DESC textureViewDesc{};
	textureViewDesc.Format = textureDesc.Format;
	textureViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	textureViewDesc.Texture2D.MipLevels = textureDesc.MipLevels;
	textureViewDesc.Texture2D.MostDetailedMip = 0;

	ComPtr<ID3D11Texture2D> texture;

	if (SUCCEEDED(hr))
	{
		hr = g_deviceResources->_d3dDevice->CreateTexture2D(&textureDesc, &textureData, &texture);
	}

	if (SUCCEEDED(hr))
	{
		hr = g_deviceResources->_d3dDevice->CreateShaderResourceView(texture, &textureViewDesc, (ID3D11ShaderResourceView**)d3dInfo->ColorMap);
	}

	if (optHasIllum)
	{
		unsigned char* illumBuffer = g_illumMapBuffer.data();
		unsigned char* colorBuffer = g_colorMapBuffer.data();

		for (int i = 0; i < bytesSize; i++)
		{
			unsigned char illum = illumBuffer[i];

			*(unsigned char*)(colorBuffer + i * 4 + 3) = illum;
		}

		if (SUCCEEDED(hr))
		{
			hr = g_deviceResources->_d3dDevice->CreateTexture2D(&textureDesc, &textureData, &texture);
		}

		if (SUCCEEDED(hr))
		{
			hr = g_deviceResources->_d3dDevice->CreateShaderResourceView(texture, &textureViewDesc, (ID3D11ShaderResourceView**)d3dInfo->LightMap);
		}
	}

	d3dInfo->AlphaMask = optHasAlpha;

	return hr;
}

XwaD3DInfo* D3dOptCreateTexture(OptNode* textureNode, int textureId, XwaTextureDescription* textureDescription, unsigned char* optTextureData, unsigned char* optTextureAlphaData, unsigned short* optPalette8, unsigned short* optPalette0)
{
	XwaD3DInfo* d3dInfo = GetD3dInfosArrayFirstEmpty();

	if (d3dInfo == nullptr)
	{
		return nullptr;
	}

	d3dInfo->MipMapsCount = -1;

	HRESULT hr;

	D3dOptSetSceneTextureTag(d3dInfo, textureNode);

	hr = D3dOptCreateTextureColorLight(d3dInfo, textureNode, textureId, textureDescription, optTextureData, optTextureAlphaData, optPalette8, optPalette0);

	return d3dInfo;
}

XwaD3DInfo* D3dOptCreateD3DfromTexture(OptNode* textureNode, int A8, XwaTextureDescription* AC, unsigned char* A10, unsigned char* A14, unsigned short* A18, unsigned short* A1C)
{
	const auto XwaCreateD3DfromTexture = (XwaD3DInfo * (*)(const char*, int, XwaTextureDescription*, unsigned char*, unsigned char*, unsigned short*, unsigned short*))0x004418A0;

	XwaD3DInfo* d3dInfo;

	d3dInfo = D3dOptCreateTexture(textureNode, A8, AC, A10, A14, A18, A1C);
	//d3dInfo = XwaCreateD3DfromTexture(textureNode->Name, A8, AC, A10, A14, A18, A1C);

	return d3dInfo;
}

HGLOBAL WINAPI DatGlobalAllocHook(UINT uFlags, SIZE_T dwBytes)
{
	HGLOBAL ptr = GlobalAlloc(uFlags, dwBytes);

	if (ptr == NULL)
	{
		OutputDebugString(__FUNCTION__ ": GlobalAlloc failed");
	}

	return ptr;
}

HGLOBAL WINAPI DatGlobalReAllocHook(HGLOBAL hMem, SIZE_T dwBytes, UINT uFlags)
{
	//return GlobalReAlloc(hMem, dwBytes, uFlags);
	return hMem;
}
