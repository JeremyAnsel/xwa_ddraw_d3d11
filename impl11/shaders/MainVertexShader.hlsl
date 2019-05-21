// Copyright (c) 2014 Jérémy Ansel
// Licensed under the MIT license. See LICENSE.txt
// Extended for VR by Leo Reyes, 2019

cbuffer ConstantBuffer : register(b0)
{
	float scale;
	float aspect_ratio;
	float parallax;
	float brightness; // Unused in the vertex shader
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

PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput output;

	/* This shader is used to render the concourse, menu and other 2D elements. */
	output.pos = float4((input.pos.x + parallax) * scale * aspect_ratio, input.pos.y * scale, 0.5f, 1.0f);
	//output.pos = float4(input.pos.x, input.pos.y, 0.5f, 1.0f);
	output.tex = input.tex;

	return output;
}
