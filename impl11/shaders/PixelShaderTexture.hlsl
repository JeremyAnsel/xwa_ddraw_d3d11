// Copyright (c) 2014 Jérémy Ansel
// Licensed under the MIT license. See LICENSE.txt
// Extended for VR by Leo Reyes (c) 2019

Texture2D    texture0 : register(t0);
SamplerState sampler0 : register(s0);

// Used for the dynamic cockpit contents, if bUseDynCockpit is set, then texture0 will be the "cover" and
// texture1 will be the dynamic element taken from the previous frame's HUD
Texture2D    texture1 : register(t1);
SamplerState sampler1 : register(s1);

cbuffer ConstantBuffer : register(b0)
{
	float brightness;      // Used to dim some elements to prevent the Bloom effect -- mostly for ReShade compatibility
	uint DynCockpitSlots;  // How many DC slots will be used. This setting was "bShadeless" previously
	uint bUseCoverTexture; // When set, use the first texture as cover texture for the dynamic cockpit
	uint unused;
	// 16 bytes
	float4 bgColor;        // Background color to use (dynamic cockpit)
	// 32 bytes
	float4 src[4];         // HLSL packs each element in an array in its own 4-vector (16 bytes) slot, so .xy is src0 and .zw is src1
	// 96 bytes
	float4 dst[4];
	// 160 bytes
};

struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
	float2 tex : TEXCOORD;
};

// From http://www.chilliant.com/rgb2hsv.html
static float Epsilon = 1e-10;

float3 RGBtoHCV(in float3 RGB)
{
	// Based on work by Sam Hocevar and Emil Persson
	float4 P = (RGB.g < RGB.b) ? float4(RGB.bg, -1.0, 2.0 / 3.0) : float4(RGB.gb, 0.0, -1.0 / 3.0);
	float4 Q = (RGB.r < P.x) ? float4(P.xyw, RGB.r) : float4(RGB.r, P.yzx);
	float C = Q.x - min(Q.w, Q.y);
	float H = abs((Q.w - Q.y) / (6 * C + Epsilon) + Q.z);
	return float3(H, C, Q.x);
}

float3 RGBtoHSV(in float3 RGB)
{
	float3 HCV = RGBtoHCV(RGB);
	float S = HCV.y / (HCV.z + Epsilon);
	return float3(HCV.x, S, HCV.z);
}

float3 RGBtoHSL(in float3 RGB)
{
	float3 HCV = RGBtoHCV(RGB);
	float L = HCV.z - HCV.y * 0.5;
	float S = HCV.y / (1 - abs(L * 2 - 1) + Epsilon);
	return float3(HCV.x, S, L);
}

float4 main(PixelShaderInput input) : SV_TARGET
{
	float4 texelColor = texture0.Sample(sampler0, input.tex);
	float alpha = texelColor.w;
	float3 diffuse = input.color.xyz;
	
	// Bloom fix
	//if (dot(texelColor.xyz, 0.333) < 0.9)
	//	diffuse = 0.8 * diffuse;
	//diffuse = float3(1.0, 1.0, 1.0);

	if (DynCockpitSlots > 0) {
		// DEBUG: Display uvs as colors. Some meshes have UVs beyond the range [0..1]
		//if (input.tex.x > 1.0) 	return float4(1, 0, 1, 1);
		//if (input.tex.y > 1.0) 	return float4(0, 0, 1, 1);
		//return float4(input.tex.xy, 0, 1); // DEBUG: Display the uvs as colors
		//return 0.7*dc_texelColor + 0.3*texelColor; // DEBUG DEBUG DEBUG!!! Remove this later! This helps position the elements easily
				
		// HLSL packs each element in an array in its own 4-vector (16-byte) row. So src[0].xy is the
		// upper-left corner of the box and src[0].zw is the lower-right corner. The same applies to
		// dst uv coords
			
		float4 dc_texelColor = bgColor;
		for (uint i = 0; i < DynCockpitSlots; i++) {
			float2 delta = dst[i].zw - dst[i].xy;
			float2 s = (input.tex - dst[i].xy) / delta;
			float2 dyn_uv = lerp(src[i].xy, src[i].zw, s);

			//if (input.tex.x >= src[i].x && input.tex.y >= src[i].y &&
			//	input.tex.x <= src[i].z && input.tex.y <= src[i].w)
			//if (input.tex.x >= dst[i].x && input.tex.y >= dst[i].y &&
			//	input.tex.x <= dst[i].z && input.tex.y <= dst[i].w)
			//	texelColor = float4(1, 0, 0, 1);

			if (dyn_uv.x >= src[i].x && dyn_uv.x <= src[i].z &&
				dyn_uv.y >= src[i].y && dyn_uv.y <= src[i].w)
			{
				// Sample the dynamic cockpit texture:
				dc_texelColor = texture1.Sample(sampler1, dyn_uv); // "dc" is for "dynamic cockpit"
				float dc_alpha = dc_texelColor.w;
				// Add the background color to the dynamic cockpit display:
				dc_texelColor = lerp(bgColor, dc_texelColor, dc_alpha);
			}
		}
		// At this point dc_texelColor has the color from the offscreen HUD buffer blended with bgColor

		// Blend the offscreen buffer HUD texture with the cover texture and go shadeless where transparent
		// or where the cover texture is bright enough
		if (bUseCoverTexture > 0) {
			// We don't have an alpha overlay texture anymore; but we can fake it by disabling shading
			// on areas with a high lightness value
			float3 HSV = RGBtoHSV(texelColor.xyz);
			// Display the dynamic cockpit texture only where the texture cover is transparent:
			texelColor = lerp(dc_texelColor, texelColor, alpha);
			// The diffuse value will be 1 (shadeless) wherever the cover texture is transparent:
			diffuse = lerp(float3(1, 1, 1), input.color.xyz, alpha);
			if (HSV.z >= 0.80)
				diffuse = float3(1, 1, 1);
		}
		else {
			texelColor = dc_texelColor;
			diffuse = float3(1, 1, 1);
		}
	}
	
	return float4(brightness * diffuse * texelColor.xyz, texelColor.w);
}
