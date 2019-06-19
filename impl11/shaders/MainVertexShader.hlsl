// Copyright (c) 2014 Jérémy Ansel
// Licensed under the MIT license. See LICENSE.txt
// Extended for VR by Leo Reyes, 2019

cbuffer ConstantBuffer : register(b0)
{
	float scale;
	float aspect_ratio;
	float parallax;
	float brightness; // This isn't used in the vertex shader
	float use_3D; // Use the 3D projection matrix or not
};

cbuffer ConstantBuffer : register(b1)
{
	matrix projEyeMatrix;
};

struct VertexShaderInput
{
	float2 pos : POSITION;
	float2 tex : TEXCOORD;
};

struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
};

/* This shader is used to render the concourse, menu and other 2D elements. */
PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput output;

	// Use the following in SteamVR and DirectSBS modes
	if (use_3D > 0.5) {
		// Upgrade to 3D:
		float4 P = float4(
			scale * aspect_ratio * input.pos.x,
			scale * input.pos.y,
			-(30.0 + parallax), // The Concourse is placed 30m away: enough space for the Tech Library
			1);
		// Project to 2D
		output.pos = mul(projEyeMatrix, P);
	} else { // Use this for the original 2D version of the game:
		output.pos = float4((input.pos.x + parallax) * scale * aspect_ratio, input.pos.y * scale, 0.5f, 1.0f);
	}
	output.tex = input.tex;

	return output;
}

/*
PixelShaderInput main_old(VertexShaderInput input)
{
	PixelShaderInput output;

	// This shader is used to render the concourse, menu and other 2D elements.
	output.pos = float4((input.pos.x + parallax) * scale * aspect_ratio, input.pos.y * scale, 0.5f, 1.0f);
	//output.pos = float4(input.pos.x, input.pos.y, 0.5f, 1.0f);
	output.tex = input.tex;

	return output;
}
*/