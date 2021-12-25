
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
	//if (input.color.w == 1.0f)
	//{
	//	return input.color;
	//}

	float4 color = float4(input.color.xyz, 1.0f);
	float4 texelColor = texture0.Sample(sampler0, input.tex);

	float4 c = texelColor * color;

	if (renderTypeIllum == 1)
	{
		float4 texelColorIllum = texture1.Sample(sampler0, input.tex);
		c.xyz += texelColorIllum.xyz * texelColorIllum.w;
		c = saturate(c);
	}

	return c;
}
