// Copyright (c) 2014 Jérémy Ansel
// Licensed under the MIT license. See LICENSE.txt
// Extended for VR by Leo Reyes, 2019

cbuffer ConstantBuffer : register(b0)
{
	float4 vpScale;
	float aspect_ratio, parallax, z_override;
};

cbuffer ConstantBuffer : register(b1)
{
	matrix projEyeMatrix;
};

struct VertexShaderInput
{
	float4 pos : POSITION;
	float4 color : COLOR0;
	float4 specular : COLOR1;
	float2 tex : TEXCOORD;
};

struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
	float2 tex : TEXCOORD;
};

static float GAME_SCALE_FACTOR = 60.0;
static float GAME_SCALE_FACTOR_Z = 60.0;
//float C = 1.0f, Z_FAR = 1.0f;
static float LOG_K = 1.0;
static float g_fFocalDist = 0.5;

float3 back_project(float3 p)
{
	float3 Q;
	float invz = 1.0 - p.z;
	Q.z = (exp(invz * LOG_K) - 1.0) / p.z;
	Q.xy = Q.z * p.xy / g_fFocalDist;
	Q *= GAME_SCALE_FACTOR;
	//Q.z *= GAME_SCALE_FACTOR_Z;
	return Q;
}

PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput output;

	// Override the depth of this element if z_override is set
	if (z_override > -0.1)
		input.pos.z = z_override;
	// Apply the scale in 2D coordinates before back-projecting. This is
	// either g_fGlobalScale or g_fGUIElemScale (used to zoom-out the HUD
	// so that it's readable
	input.pos.xy *= vpScale.w;

	// input.pos is in 2D; but normalized to -1..1, back-project into 3D:
	float3 P = back_project(input.pos.xyz);
	// and then project into 2D; but using the right projEje matrix:
	P.y = -P.y;
	P.z = -P.z;
	// Project 3D --> 2D
	output.pos = mul(projEyeMatrix, float4(P, 1));
	// Normalize:
	output.pos /= output.pos.w;

	// We have normalized 2D again, continue processing as before:
	output.pos.xy = output.pos.xy * vpScale.z;
	output.pos.z = input.pos.z; // Restore the original Z to avoid Z-fighting
	output.pos.w = 1.0f;
	
	// Halve the size of the screen
	output.pos.xy *= 0.5 * float2(aspect_ratio, 1);
	output.pos /= input.pos.w;

	output.color = input.color.zyxw;
	output.tex = input.tex;
	return output;
}


/*
PixelShaderInput main_old(VertexShaderInput input) // This was the original DirectSBS shader
{
	PixelShaderInput output;

	output.pos.x = (input.pos.x * vpScale.x - 1.0f + parallax) * vpScale.z;
	output.pos.y = (input.pos.y * vpScale.y + 1.0f) * vpScale.z;
	if (z_override > -0.9f) {
		output.pos.z = z_override;
	}
	else {
		output.pos.z = input.pos.z;
	}
	output.pos.w = 1.0f;

	// Halve the size of the screen; but scale according to vpScale.w, which is
	// set to g_global_scale or GUI_elem_scale depending on the type of element
	// being drawn
	output.pos.x *= vpScale.w * 0.5 * aspect_ratio;
	output.pos.y *= vpScale.w * 0.5;

	output.pos /= input.pos.w;

	output.color = input.color.zyxw;
	output.tex = input.tex;
	return output;
}
*/