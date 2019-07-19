#include "BloomCommon.fxh"

// From:
// https://learnopengl.com/Advanced-Lighting/Bloom
// uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);
Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

cbuffer ConstantBuffer : register(b0)
{
	float factor;
};

struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD;
};

//void PS_Downsample1(in float4 pos : SV_Position, in float2 uv : TEXCOORD0, out float4 bloom : SV_Target0)
float4 main(PixelShaderInput input) : SV_TARGET
{
	return downsample(texture0, sampler0, ldexp(SCREEN_SIZE, factor), input.uv);
}