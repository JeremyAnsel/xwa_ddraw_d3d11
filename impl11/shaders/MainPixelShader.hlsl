// Copyright (c) 2014 Jérémy Ansel
// Licensed under the MIT license. See LICENSE.txt
// Extended for VR by Leo Reyes (c) 2019

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

cbuffer ConstantBuffer : register(b0)
{
	float scale;
	float aspect_ratio;
	float parallax;
	float brightness; // Only one used in the PixelShader
};

struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
	float4 texelColor = texture0.Sample(sampler0, input.tex);

	/*if (texelColor.a > 0.4f)
	{
		discard;
	}*/

	return float4(brightness * texelColor.xyz, 1.0f);
}
