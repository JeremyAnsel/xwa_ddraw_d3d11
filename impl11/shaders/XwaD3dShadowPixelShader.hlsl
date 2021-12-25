
#include "XwaD3dCommon.hlsl"

Texture2D texture0 : register(t0);
Texture2D texture1 : register(t1);
SamplerState sampler0 : register(s0);

struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float4 position : COLOR2;
	float4 normal : COLOR1;
	float2 tex : TEXCOORD;
	float4 color : COLOR0;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
	return float4(0.0f, 0.0f, 0.0f, 1.0f);
}
