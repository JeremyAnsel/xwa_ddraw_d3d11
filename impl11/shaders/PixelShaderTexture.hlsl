// Copyright (c) 2014 Jérémy Ansel
// Licensed under the MIT license. See LICENSE.txt
// Extended for VR by Leo Reyes (c) 2019

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

cbuffer ConstantBuffer : register(b0)
{
	float brightness; // Used to dim some elements to prevent the Bloom effect -- mostly for ReShade compatibility
	float bShadeless; // Ignore diffuse component. Used to render the dynamic cockpit.
	float2 uv_scale; // Scale the UVs by this much
};

struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
	float2 tex : TEXCOORD;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
	float4 texelColor = texture0.Sample(sampler0, uv_scale * input.tex);

	if (bShadeless > 0.5)
		return float4(brightness * texelColor.xyz, texelColor.w);
	else
		return float4(brightness * texelColor.xyz, texelColor.w) * input.color;
}
