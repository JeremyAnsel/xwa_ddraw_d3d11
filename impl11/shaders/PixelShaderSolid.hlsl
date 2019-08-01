// Copyright (c) 2014 Jérémy Ansel
// Licensed under the MIT license. See LICENSE.txt
// Extended for VR by Leo Reyes, 2019

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
	float2 tex : TEXCOORD;
};

struct PixelShaderOutput
{
	float4 color0 : SV_TARGET0;
	float4 color1 : SV_TARGET1;
};

PixelShaderOutput main(PixelShaderInput input)
{
	PixelShaderOutput output;
	output.color0 = input.color;
	output.color1 = input.color;
	return output;
}
