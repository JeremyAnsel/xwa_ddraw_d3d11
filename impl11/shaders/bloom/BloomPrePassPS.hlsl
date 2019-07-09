/*=============================================================================

	Adapted by Leo Reyes with permission from:

	ReShade 4 effect file
	github.com/martymcmodding

	Support Pascal Gilcher at:
		paypal.me/mcflypg
		patreon.com/mcflypg

	Simple Bloom
	by Marty McFly / P.Gilcher
	part of qUINT shader library for ReShade 3

	CC BY-NC-ND 3.0 licensed.

=============================================================================*/

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

#include "BloomCommon.fxh"

struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD;
};

//void PS_BloomPrepass(in float4 pos : SV_Position, in float2 uv : TEXCOORD0, out float4 color : SV_Target0)
float4 main(PixelShaderInput input) : SV_TARGET
{
	float4 color = downsample(texture0, sampler0, SCREEN_SIZE, input.uv);
	color.w = saturate(dot(color.rgb, 0.333)); // Original value

	color.rgb = lerp(color.w, color.rgb, BLOOM_SAT);
	color.rgb *= (pow(color.w, BLOOM_CURVE) * BLOOM_INTENSITY * BLOOM_INTENSITY * BLOOM_INTENSITY) / (color.w + 1e-3);
	color.w = 1.0;
	return color;

	//float4 color = texture0.Sample(sampler0, input.uv);
	//color.w = 1.0f;
	//return color;
}

/*
float4 main(PixelShaderInput input) : SV_TARGET
{
	float4 color = texture0.Sample(sampler0, input.uv);
	color.w = 1.0f;
	return color;
}
*/