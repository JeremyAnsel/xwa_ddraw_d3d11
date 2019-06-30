// Copyright (c) 2014 Jérémy Ansel
// Licensed under the MIT license. See LICENSE.txt
// Extended for VR by Leo Reyes (c) 2019

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

Texture2D texture1 : register(t1);
SamplerState sampler1 : register(s1);

cbuffer ConstantBuffer : register(b0)
{
	float brightness;  // Used to dim some elements to prevent the Bloom effect -- mostly for ReShade compatibility
	float bShadeless;  // Ignore diffuse component. Used to render the dynamic cockpit.
	float2 uv_scale;   // Scale the UVs by this much
	// 16 bytes
	float2 uv_offset;      // Move the UVs by this much *after* applying uv_scale (dynamic cockpit)
	float bUseBGColor;     // When set, use the background color (dynamic cockpit)
	float bUseDynCockpit;  // Use the second texture slot for the dynamic cockpit
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
	float4 texelColor = texture0.Sample(sampler0, input.tex);

	if (bUseDynCockpit > 0.5) {
		// Sample the dynamic cockpit texture:
		float2 dyn_uvs = uv_scale * input.tex + uv_offset;
		float4 dc_texelColor = texture1.Sample(sampler1, dyn_uvs); // "dc" is for "dynamic cockpit"
		texelColor = dc_texelColor;
		// Blend the dynamic cockpit color with either the background color or the previous texture:
		if (bUseBGColor > 0.5)
			texelColor.xyz = dc_texelColor.w * dc_texelColor.xyz + (1 - dc_texelColor.w) * bgColor.xyz;
		else
			texelColor.xyz = dc_texelColor.w * dc_texelColor.xyz + (1 - dc_texelColor.w) * texelColor.xyz;
		texelColor.w = dc_texelColor.w;
	}

	if (bShadeless > 0.5)
		return float4(brightness * texelColor.xyz, texelColor.w);
	else
		return float4(brightness * texelColor.xyz, texelColor.w) * input.color;
}
