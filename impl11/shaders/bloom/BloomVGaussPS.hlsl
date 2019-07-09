#include "BloomCommon.fxh"

// Based on the implementation from:
// https://learnopengl.com/Advanced-Lighting/Bloom

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

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
	float2 input_uv = input.uv / 4.0;
	float4 color = texture0.Sample(sampler0, input_uv) * weight[0];
	float4 s1, s2;
	float2 dy = 2 * float2(0, PIXEL_SIZE.y);
	float2 uv1 = input_uv + dy;
	float2 uv2 = input_uv - dy;

	[unroll]
	for (int i = 1; i < 5; i++) {
		s1 = texture0.Sample(sampler0, uv1) * weight[i];
		s2 = texture0.Sample(sampler0, uv2) * weight[i];
		color += s1 + s2;
		uv1 += dy;
		uv2 -= dy;
	}
	return 2.0 * color;
}