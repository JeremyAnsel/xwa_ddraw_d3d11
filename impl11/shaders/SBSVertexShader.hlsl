// Copyright (c) 2014 J�r�my Ansel
// Licensed under the MIT license. See LICENSE.txt
// Extended for VR by Leo Reyes, 2019

cbuffer ConstantBuffer : register(b0)
{
	float4 vpScale;
	float aspect_ratio, restoreZ, z_override, sz_override;
	float mult_z_override, z_flip;
};

cbuffer ConstantBuffer : register(b1)
{
	matrix projEyeMatrix;
	matrix viewMatrix;
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
static float METRIC_SCALE_FACTOR = 25.0;
//float C = 1.0f, Z_FAR = 1.0f;
//static float LOG_K = 1.0;
static float g_fFocalDist = 0.5;
//static float C = 8;
//static float LOG_K = log(C * METRIC_SCALE_FACTOR * 65535);

float3 back_project_exp(float3 p)
{
	float3 Q;
	float invz = 1.0 - p.z;
	Q.z = (exp(invz) - 1.0) / p.z;
	Q.xy = Q.z * p.xy / g_fFocalDist;
	Q *= GAME_SCALE_FACTOR;
	//Q.z *= GAME_SCALE_FACTOR_Z;
	return Q;
}

PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput output;
	float sz = input.pos.z;
	float w = 1.0 / input.pos.w;

	float3 temp = input.pos.xyz;
	// Apply the scale in 2D coordinates before back-projecting. This is
	// either g_fGlobalScale or g_fGUIElemScale (used to zoom-out the HUD
	// so that it's readable)
	//temp.xy *= 0.5 * vpScale.w * vpScale.z * float2(aspect_ratio, 1);
	temp.xy *= vpScale.w * vpScale.z * float2(aspect_ratio, 1);

	// Override the depth of this element if z_override is set
	temp.z = METRIC_SCALE_FACTOR * w; // This value was determined empirically so that the X-Wing cockpit had a reasonably metric size
	// temp.z = w; // This setting provides a really nice depth for distant objects.
	if (mult_z_override > -0.1)
		temp.z *= mult_z_override;
	if (z_override > -0.1)
		temp.z = z_override;

	// The back-projection into 3D is now very simple:
	float3 P = float3(temp.z * temp.xy, temp.z);
	// Adjust the coordinate system for SteamVR:
	P.y = -P.y;
	P.z = -P.z;
	// Apply head position and project 3D --> 2D
	output.pos = mul(viewMatrix, float4(P, 1));
	output.pos = mul(projEyeMatrix, output.pos);

	// Compute the new sz:
	////output.pos.z = (1 - log(C*output.pos.w + 1) / LOG_K) * output.pos.w;
	//output.pos.z = (1 - log(output.pos.w + 1) / LOG_K) * output.pos.w;

	// For some weird reason the following line also provides perspective-correct texturing;
	// but it's using the old sz and w:
	//output.pos.z = sz * w;

	// Use the original sz; but compensate with w so that it stays perspective-correct:
	output.pos.z = sz * output.pos.w;
	if (sz_override > -0.1)
		output.pos.z = sz_override;
	//if (restoreZ > 0.5)
	//	output.pos.z = sz;

	output.color = input.color.zyxw;
	output.tex = input.tex;
	return output;
}

PixelShaderInput old_main_0_9_8(VertexShaderInput input)
{
	PixelShaderInput output;
	float sz = input.pos.z;

	float3 temp = input.pos.xyz;
	// Apply the scale in 2D coordinates before back-projecting. This is
	// either g_fGlobalScale or g_fGUIElemScale (used to zoom-out the HUD
	// so that it's readable)
	temp.xy *= 0.5 * vpScale.w * vpScale.z * float2(aspect_ratio, 1);
	//temp.xy *= vpScale.w * vpScale.z * float2(aspect_ratio, 1);

	// Override the depth of this element if z_override is set
	if (z_override > -0.1)
		temp.z = z_override;

	// input.pos is in 2D; but normalized to -1..1, back-project into 3D:
	float3 P = back_project_exp(temp.xyz);
	// and then project into 2D; but using the right projEje matrix:
	P.y = -P.y;
	P.z = -P.z;
	// Project 3D --> 2D
	output.pos = mul(projEyeMatrix, float4(P, 1));
	// Normalize:
	output.pos /= output.pos.w;
	output.pos.w = 1.0f;

	// We have normalized 2D again, continue processing as before:
	output.pos.z = sz;
	if (z_override > -0.1)
		output.pos.z = z_override;
	if (restoreZ > 0.5)
		output.pos.z = sz;
	/*
	if (restoreZ < 0.5)
		output.pos.z = input.pos.z;
	else
		output.pos.z = sz;	
	*/
	output.pos /= input.pos.w;
	output.color = input.color.zyxw;
	output.tex = input.tex;
	return output;
}

/*
PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput output;
	float sz = input.pos.z;

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
	if (restoreZ < 0.5)
		output.pos.z = input.pos.z;
	else
		output.pos.z = sz;
	
	output.pos.w = 1.0f;
	
	// Halve the size of the screen
	output.pos.xy *= 0.5 * float2(aspect_ratio, 1);
	output.pos /= input.pos.w;

	output.color = input.color.zyxw;
	output.tex = input.tex;
	return output;
}
*/

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