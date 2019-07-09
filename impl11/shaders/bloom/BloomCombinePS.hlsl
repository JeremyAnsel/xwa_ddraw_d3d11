#include "BloomCommon.fxh"

// Based on the implementation from:
// https://learnopengl.com/Advanced-Lighting/Bloom

// This texture slot should be the original backbuffer SRV
Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);
// This texture slot should be the bloom texture
Texture2D bloomTex: register(t1);
SamplerState bloomSampler : register(s1);

/*
cbuffer ConstantBuffer : register(b0)
{
	float factor;
};
*/

struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
	float2 input_uv_sub = input.uv / 4.0;
	float4 color = texture0.Sample(sampler0, input.uv);
	float4 bloom = bloomTex.Sample(bloomSampler, input_uv_sub);
	color += bloom;
	color.w = 1.0f;
	return color;
}