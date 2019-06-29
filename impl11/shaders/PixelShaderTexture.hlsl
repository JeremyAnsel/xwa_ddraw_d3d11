// Copyright (c) 2014 Jérémy Ansel
// Licensed under the MIT license. See LICENSE.txt
// Extended for VR by Leo Reyes (c) 2019

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

cbuffer ConstantBuffer : register(b0)
{
	float brightness;  // Used to dim some elements to prevent the Bloom effect -- mostly for ReShade compatibility
	float bShadeless;  // Ignore diffuse component. Used to render the dynamic cockpit.
	float2 uv_scale;   // Scale the UVs by this much
	// 16 bytes
	float2 uv_offset;  // Move the UVs by this much *after* applying uv_scale (dynamic cockpit)
	float bUseBGColor; // When set, use the background color (dynamic cockpit)
	float dummy;
	// 32 bytes
	float4 bgColor;    // Background color to use (dynamic cockpit)
	// 48 bytes
};

struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
	float2 tex : TEXCOORD;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
	float4 texelColor = texture0.Sample(sampler0, uv_scale * input.tex + uv_offset);
	// Blend the bgColor with the current texture color (used to add a background to dynamic cockpit elements)
	if (bUseBGColor > 0.5f)
		texelColor.xyz = texelColor.w * texelColor.xyz + (1 - texelColor.w) * bgColor;

	if (bShadeless > 0.5)
		return float4(brightness * texelColor.xyz, texelColor.w);
	else
		return float4(brightness * texelColor.xyz, texelColor.w) * input.color;
}
