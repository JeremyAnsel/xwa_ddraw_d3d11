// Copyright (c) 2014 Jérémy Ansel
// Licensed under the MIT license. See LICENSE.txt
// Extended for VR by Leo Reyes, 2019

cbuffer ConstantBuffer : register(b0)
{
	float4 vpScale;
	float aspect_ratio, cockpit_threshold, z_override, sz_override;
	float mult_z_override, bPreventTransform, bFullTransform;
};

cbuffer ConstantBuffer : register(b1)
{
	matrix projEyeMatrix;
	matrix viewMatrix;
	matrix fullViewMatrix;
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

static float METRIC_SCALE_FACTOR = 25.0;

PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput output;
	float sz = input.pos.z;
	float pz = 1.0 - sz;
	float w = 1.0 / input.pos.w;

	float3 temp = input.pos.xyz;
	// Normalize into the -0.5..0.5 range
	temp.xy *= vpScale.xy;
	temp.xy -= 0.5;
	// Apply the scale in 2D coordinates before back-projecting. This is
	// either g_fGlobalScale or g_fGUIElemScale (used to zoom-out the HUD
	// so that it's readable)
	temp.xy *= vpScale.w * vpScale.z * float2(aspect_ratio, 1);

	temp.z = METRIC_SCALE_FACTOR * w; // This value was determined empirically
	// temp.z = w; // This setting provides a really nice depth for distant objects; but the cockpit is messed up
	// Override the depth of this element if z_override is set
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
	if (bPreventTransform < 0.5f) {
		if (bFullTransform < 0.5f)
			output.pos = mul(viewMatrix, float4(P, 1));
		else
			output.pos = mul(fullViewMatrix, float4(P, 1));
	} else {
		// The HUD should not be transformed so that it's possible to aim properly
		// This case is specifically to keep the aiming HUD centered so that it can still be used
		// to aim the lasers. Here, we ignore all translations to keep the HUD in the right spot.
		float4x4 compViewMatrix = viewMatrix;
		compViewMatrix._m03_m13_m23 = 0;
		output.pos = mul(compViewMatrix, float4(P, 1));
	}
	output.pos = mul(projEyeMatrix, output.pos);

	/*
	// Stereoscopy boost -- probably not worth it, needs to be able to distinguish the skybox and the HUD is amplified too
	if (pz > cockpit_threshold) {
		float w_orig = output.pos.w;
		metric_scale = 1;
		temp.xy *= vpScale.w * vpScale.z * float2(aspect_ratio, 1);
		temp.z = metric_scale * w; // This value was determined empirically so that the X-Wing cockpit had a reasonably metric size
		// temp.z = w; // This setting provides a really nice depth for distant objects.
		if (mult_z_override > -0.1)
			temp.z *= mult_z_override;
		if (z_override > -0.1)
			temp.z = z_override;
		float3 P = float3(temp.z * temp.xy, temp.z);
		// Adjust the coordinate system for SteamVR:
		P.y = -P.y;
		P.z = -P.z;
		// Apply head position and project 3D --> 2D
		output.pos = mul(viewMatrix, float4(P, 1));
		output.pos = mul(projEyeMatrix, output.pos);
		output.pos *= w_orig / output.pos.w;
	}
	*/

	// For some weird reason the following line also provides perspective-correct texturing;
	// but it's using the old sz and w:
	//output.pos.z = sz * w;

	// Use the original sz; but compensate with the new w so that it stays perspective-correct:
	output.pos.z = sz * output.pos.w;
	// NOTE: The use of this w coming from the perspective matrix may have fixed the ghosting effect
	//		 in the Pimax. Better not to use the original w coming from the game.
	if (sz_override > -0.1)
		output.pos.z = sz_override;

	output.color = input.color.zyxw;
	output.tex = input.tex;
	return output;
}
