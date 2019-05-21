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
	float width;
	float height;
	texture0.GetDimensions(width, height);

	int2 coords = floor(input.tex * float2(width, height));
	float4 pointColor = texture0.Load(int3(coords, 0));

	if (abs(pointColor.r - (4.0 / 31.0)) < 0.001 && pointColor.g == 0 && pointColor.b == 0)
	{
		discard;
	}

	float4 texelColor = texture0.Sample(sampler0, input.tex);

	return float4(brightness * texelColor.xyz, 1.0f);
}
