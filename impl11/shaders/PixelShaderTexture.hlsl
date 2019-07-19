// Copyright (c) 2014 Jérémy Ansel
// Licensed under the MIT license. See LICENSE.txt
// Extended for VR by Leo Reyes (c) 2019

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

// Used for the dynamic cockpit contents, if bUseDynCockpit is set, then texture0 will be the "cover" and
// texture1 will be the dynamic element taken from the previous frame's HUD
Texture2D texture1 : register(t1);
SamplerState sampler1 : register(s1);

cbuffer ConstantBuffer : register(b0)
{
	float brightness;  // Used to dim some elements to prevent the Bloom effect -- mostly for ReShade compatibility
	float bUnused;     // This setting was "bShadeless" previously
	float2 uv_scale;   // Scale the UVs by this much
	// 16 bytes
	float2 uv_offset;       // Move the UVs by this much *after* applying uv_scale (dynamic cockpit)
	float bUseCoverTexture; // When set, use the first texture as a cover for the dynamic cockpit
	float bUseDynCockpit;   // Use the second texture slot for the dynamic cockpit
	// 32 bytes
	float4 bgColor;         // Background color to use (dynamic cockpit)
	// 48 bytes
	float2 uv_src_dyn0;
	float2 uv_src_dyn1;
	// 64 bytes
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
	float alpha = texelColor.w;
	float3 diffuse = input.color.xyz;
	
	// Bloom fix
	//if (dot(texelColor.xyz, 0.333) < 0.9)
	//	diffuse = 0.8 * diffuse;
	//diffuse = float3(1.0, 1.0, 1.0);

	//if (bShadeless > 0.5)
	//	diffuse = float3(1.0, 1.0, 1.0);

	if (bUseDynCockpit > 0.5) {
		//return 0.7*dc_texelColor + 0.3*texelColor; // DEBUG DEBUG DEBUG!!! Remove this later! This helps position the elements easily

		// Sample the dynamic cockpit texture:
		float2 dyn_uv = uv_scale * input.tex + uv_offset;
		float4 dc_texelColor = bgColor;
		if (dyn_uv.x >= 0 && dyn_uv.y >= 0 &&
			dyn_uv.x <= 1 && dyn_uv.y <= 1) 
		{
			dc_texelColor = texture1.Sample(sampler1, dyn_uv); // "dc" is for "dynamic cockpit"
			float dc_alpha = dc_texelColor.w;
			// Add the background color to the dynamic cockpit display:
			dc_texelColor = dc_alpha * dc_texelColor + (1.0 - dc_alpha) * bgColor;
		}

		if (bUseCoverTexture > 0.5) {
			// Display the dynamic cockpit texture only where the texture cover is transparent:
			texelColor = alpha * texelColor + (1.0 - alpha) * dc_texelColor;
			// The diffuse value will be 1 (shadeless) wherever the cover texture is transparent:
			diffuse = alpha * input.color.xyz + (1.0 - alpha) * float3(1.0, 1.0, 1.0);
		}
		else
			texelColor = dc_texelColor;
	}

	return float4(brightness * diffuse * texelColor.xyz, texelColor.w);
}
