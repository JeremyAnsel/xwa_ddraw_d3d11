#include "BloomCommon.fxh"

// reshadeOutput1
Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

// reshadeOutput2
Texture2D texture1 : register(t1);
SamplerState sampler1 : register(s1);

cbuffer ConstantBuffer : register(b0)
{
	float factor;
};

struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD;
};

//void PS_Upsample1(in float4 pos : SV_Position, in float2 uv : TEXCOORD0, out float4 bloom : SV_Target0)
float4 main(PixelShaderInput input) : SV_TARGET
{
	float4 color2 = float4(upsample(texture1, sampler1, ldexp(PIXEL_SIZE, factor), input.uv), BLOOM_LAYER_MULT_1);
	//float4 color2 = float4(upsample(texture1, sampler1, ldexp(SCREEN_SIZE, -2.0), input.uv), BLOOM_LAYER_MULT_1);
	//return color2;

	float4 color1 = texture0.Sample(sampler0, input.uv);
	float w = color1.w;
	return float4(saturate(color1.rgb + color2.rgb), w);
}
