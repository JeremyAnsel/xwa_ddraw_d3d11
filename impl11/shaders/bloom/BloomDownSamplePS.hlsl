#include "BloomCommon.fxh"

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD;
};

//void PS_Downsample1(in float4 pos : SV_Position, in float2 uv : TEXCOORD0, out float4 bloom : SV_Target0)
float4 main(PixelShaderInput input) : SV_TARGET
{
	return downsample(texture0, sampler0, ldexp(SCREEN_SIZE, -2.0), input.uv);
}