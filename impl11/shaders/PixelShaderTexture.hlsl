// Copyright (c) 2014 Jérémy Ansel
// Licensed under the MIT license. See LICENSE.txt
// Extended for VR by Leo Reyes (c) 2019

Texture2D    texture0 : register(t0);
SamplerState sampler0 : register(s0);

Texture2D    texture1 : register(t1);
SamplerState sampler1 : register(s1);

// When the Dynamic Cockpit is active:
// texture0 == cover texture and
// texture1 == HUD offscreen buffer

// If bRenderHUD is set:
// texture0 == HUD foreground
// texture1 == HUD background


#define MAX_DC_COORDS 8

cbuffer ConstantBuffer : register(b0)
{
	float brightness;		// Used to dim some elements to prevent the Bloom effect -- mostly for ReShade compatibility
	uint DynCockpitSlots;	// How many DC slots will be used. This setting was "bShadeless" previously
	uint bUseCoverTexture;	// When set, use the first texture as cover texture for the dynamic cockpit
	uint bRenderHUD;			// When set, first texture is HUD foreground and second texture is HUD background
	// 16 bytes

	//uint bAlphaOnly;			// Output only the alpha component for debug purposes
	//uint unused[3];
	
	float4 src[MAX_DC_COORDS];		// HLSL packs each element in an array in its own 4-vector (16 bytes) slot, so .xy is src0 and .zw is src1
	float4 dst[MAX_DC_COORDS];
	uint4 bgColor[MAX_DC_COORDS / 4]; // Background colors to use for the dynamic cockpit, this divide by 4 is because HLSL packs each elem in a 4-vector,
									  // So each elem here is actually 4 bgColors.

	uint bIsLaser;					// 1 for Laser objects, setting this to 2 will make them brighter (intended for 32-bit mode)
	uint bIsLightTexture;			// 1 if this is a light texture, 2 will make it brighter (intended for 32-bit mode)
	uint bIsEngineGlow;				// 1 if this is an engine glow textures, 2 will make it brighter (intended for 32-bit mode)
	float ct_brightness;				// Cover texture brightness. In 32-bit mode the cover textures have to be dimmed.
};

struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
	float2 tex : TEXCOORD;
};

// From http://www.chilliant.com/rgb2hsv.html
static float Epsilon = 1e-10;

float3 HUEtoRGB(in float H)
{
	float R = abs(H * 6 - 3) - 1;
	float G = 2 - abs(H * 6 - 2);
	float B = 2 - abs(H * 6 - 4);
	return saturate(float3(R, G, B));
}

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

float3 HSVtoRGB(in float3 HSV)
{
	float3 RGB = HUEtoRGB(HSV.x);
	return ((RGB - 1) * HSV.y + 1) * HSV.z;
}

/*
float3 RGBtoHSL(in float3 RGB)
{
	float3 HCV = RGBtoHCV(RGB);
	float L = HCV.z - HCV.y * 0.5;
	float S = HCV.y / (1 - abs(L * 2 - 1) + Epsilon);
	return float3(HCV.x, S, L);
}

float3 HSLtoRGB(in float3 HSL)
{
	float3 RGB = HUEtoRGB(HSL.x);
	float C = (1 - abs(2 * HSL.z - 1)) * HSL.y;
	return (RGB - 0.5) * C + HSL.z;
}
*/

float4 uintColorToFloat4(uint color) {
	return float4(
		((color >> 16) & 0xFF) / 255.0,
		((color >> 8) & 0xFF) / 255.0,
		(color & 0xFF) / 255.0,
		1);
}

uint getBGColor(uint i) {
	uint idx = i / 4;
	uint sub_idx = i % 4;
	return bgColor[idx][sub_idx];
}

float4 main(PixelShaderInput input) : SV_TARGET
{
	float4 texelColor = texture0.Sample(sampler0, input.tex);
	float alpha = texelColor.w;
	float3 diffuse = input.color.xyz;

	// Process lasers (make them brighter in 32-bit mode)
	if (bIsLaser > 0) {
		// This is a laser texture, process the bloom mask accordingly
		if (bIsLaser > 1) {
			// Enhance the lasers in 32-bit mode
			float3 HSV = RGBtoHSV(texelColor.xyz);
			// Increase the saturation and lightness
			HSV.y *= 1.5;
			HSV.z *= 2.0;
			//return float4(0, 1, 0, 2.0 * 1.2 * alpha);
			return float4(HSVtoRGB(HSV), 1.2 * alpha);
		}
		else {
			//return float4(0, 1, 0, alpha);
			return texelColor; // Return the original color when 32-bit mode is off
		}
	}

	// Process light textures (make them brighter in 32-bit mode)
	if (bIsLightTexture > 0) {
		// This is a light texture, process the bloom mask accordingly
		float3 HSV = RGBtoHSV(texelColor.xyz);
		float val = HSV.z;
		if (bIsLightTexture > 1) {
			// Make the light textures brighter in 32-bit mode
			HSV.z *= 1.25;
			// The alpha for light textures is either 0 or >0.1, so we multiply by 10 to
			// make it [0, 1]
			alpha *= 10.0;
			//if (val > 0.8 && alpha > 0.5)
			//	return float4(0, 1, 0, val);				
			return float4(HSVtoRGB(HSV), alpha);
		}
		else {
			//if (val > 0.8 && alpha > 0.5)
			//	return float4(0, 1, 0, val);
			return texelColor; // Return the original color when 32-bit mode is off
		}
	}

	// Enhance engine glow. In this texture, the diffuse component also provides
	// the hue
	if (bIsEngineGlow > 0) {
		//return float4(0, 1, 0, alpha);
		texelColor.xyz *= diffuse;
		// This is an engine glow, process the bloom mask accordingly
		if (bIsEngineGlow > 1) {
			// Enhance the glow in 32-bit mode
			float3 HSV = RGBtoHSV(texelColor.xyz);
			HSV.y *= 1.15;
			HSV.z *= 1.25;
			return float4(HSVtoRGB(HSV), alpha);
		}
		else
			return texelColor; // Return the original color when 32-bit mode is off
	}

	// Render the captured HUD, execute the move_region commands.
	// The code returns a color from this path
	if (bRenderHUD) {
		// texture0 == HUD foreground
		// texture1 == HUD background
		float4 texelColorBG = texture1.Sample(sampler1, input.tex);
		float alphaBG = texelColorBG.w;
		uint i;
		
		// Execute the move_region commands: erase source regions
		for (i = 0; i < DynCockpitSlots; i++)
			if (input.tex.x >= src[i].x && input.tex.x <= src[i].z &&
				input.tex.y >= src[i].y && input.tex.y <= src[i].w) {
				texelColor.w = 0;
				alpha = 0;
				alphaBG = 0;
			}

		// Execute the move_region commands: copy regions
		[unroll]
		for (i = 0; i < DynCockpitSlots; i++) {
			float2 delta = dst[i].zw - dst[i].xy;
			float2 s = (input.tex - dst[i].xy) / delta;
			float2 dyn_uv = lerp(src[i].xy, src[i].zw, s);
			if (dyn_uv.x >= src[i].x && dyn_uv.x <= src[i].z &&
				dyn_uv.y >= src[i].y && dyn_uv.y <= src[i].w)
			{
				// Sample the HUD FG and BG from a different location:
				texelColor = texture0.Sample(sampler0, dyn_uv);
				alpha = texelColor.w;
				texelColorBG = texture1.Sample(sampler1, dyn_uv);
				alphaBG = texelColorBG.w;
			}
		}
		// Do the alpha blending
		texelColor.xyz = lerp(texelColorBG.xyz, texelColor.xyz, alpha);
		texelColor.w += 3.0 * alphaBG;
		return texelColor;
	} 
	// Render the Dynamic Cockpit captured buffer into the cockpit destination textures. 
	// The code returns a color from this path
	else if (DynCockpitSlots > 0) {
		// DEBUG: Display uvs as colors. Some meshes have UVs beyond the range [0..1]
		//if (input.tex.x > 1.0) 	return float4(1, 0, 1, 1);
		//if (input.tex.y > 1.0) 	return float4(0, 0, 1, 1);
		//return float4(input.tex.xy, 0, 1); // DEBUG: Display the uvs as colors
		//return 0.7*hud_texelColor + 0.3*texelColor; // DEBUG DEBUG DEBUG!!! Remove this later! This helps position the elements easily
				
		// HLSL packs each element in an array in its own 4-vector (16-byte) row. So src[0].xy is the
		// upper-left corner of the box and src[0].zw is the lower-right corner. The same applies to
		// dst uv coords
		
		float4 hud_texelColor = uintColorToFloat4(getBGColor(0));
		[unroll]
		for (uint i = 0; i < DynCockpitSlots; i++) {
			float2 delta = dst[i].zw - dst[i].xy;
			float2 s = (input.tex - dst[i].xy) / delta;
			float2 dyn_uv = lerp(src[i].xy, src[i].zw, s);

			if (dyn_uv.x >= src[i].x && dyn_uv.x <= src[i].z &&
				dyn_uv.y >= src[i].y && dyn_uv.y <= src[i].w)
			{
				// Sample the dynamic cockpit texture:
				hud_texelColor = texture1.Sample(sampler1, dyn_uv); // "ct" is for "cover_texture"
				float hud_alpha = hud_texelColor.w;
				// Add the background color to the dynamic cockpit display:
				hud_texelColor = lerp(uintColorToFloat4(getBGColor(i)), hud_texelColor, hud_alpha);
			}
		}
		// At this point hud_texelColor has the color from the offscreen HUD buffer blended with bgColor

		// Blend the offscreen buffer HUD texture with the cover texture and go shadeless where transparent.
		// Also go shadless where the cover texture is bright enough.
		if (bUseCoverTexture > 0) {
			// We don't have an alpha overlay texture anymore; but we can fake it by disabling shading
			// on areas with a high lightness value

			// texelColor is the cover_texture right now
			float3 HSV = RGBtoHSV(texelColor.xyz);
			float brightness = ct_brightness;
			if (HSV.z * alpha >= 0.8) {
				// The cover texture is bright enough, go shadeless and make it brighter
				//return float4(1, 0, 1, 1);
				diffuse = float3(1, 1, 1);
				// Increase the brightness:
				HSV = RGBtoHSV(texelColor.xyz);
				HSV.z *= 1.2;
				texelColor.xyz = HSVtoRGB(HSV);
				brightness = 1.0;
			}
			// Display the dynamic cockpit texture only where the texture cover is transparent:
			// In 32-bit mode, the cover textures appear brighter, we should probably dim them, that's what the 0.8 below is for:
			texelColor = lerp(hud_texelColor, brightness * texelColor, alpha);
			// The diffuse value will be 1 (shadeless) wherever the cover texture is transparent:
			diffuse = lerp(float3(1, 1, 1), diffuse, alpha);
		} else {
			texelColor = hud_texelColor;
			diffuse = float3(1, 1, 1);
		}
		return float4(diffuse * texelColor.xyz, texelColor.w);
	}
	
	return float4(brightness * diffuse * texelColor.xyz, texelColor.w);
}
