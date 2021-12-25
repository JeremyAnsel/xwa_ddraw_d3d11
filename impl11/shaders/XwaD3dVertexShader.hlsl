
#include "XwaD3dCommon.hlsl"

Buffer<float3> g_vertices : register(t0);
Buffer<float3> g_normals : register(t1);
Buffer<float2> g_textureCoords : register(t2);

struct VertexShaderInput
{
	int4 index : POSITION;
};

struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float4 position : COLOR2;
	float4 normal : COLOR1;
	float2 tex : TEXCOORD;
	float4 color : COLOR0;
};

PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput output;

	float3 v = g_vertices[input.index.x];
	float3 n = g_normals[input.index.y];
	float2 t = g_textureCoords[input.index.z];
	int c = input.index.w;

	output.pos = mul(float4(v, 1.0f), transformWorldView);
	output.pos = TransformProjection(output.pos.xyz);

	output.position = float4(v, 1.0f);
	output.normal = mul(float4(n, 1.0f), transformWorldView);
	output.tex = t;

	output.color = float4(1.0f, 1.0f, 1.0f, 0.0f);

	//if (c == 0)
	if (renderType == 0)
	{
		output.color = ComputeColorLight(v, n);
	}

	return output;
}
