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

PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput output;

	// Project 3D --> 2D
	input.pos.z = -input.pos.z;
	output.pos = mul(projEyeMatrix, float4(input.pos.xyz, 1));
	//output.pos = mul(input.pos, projEyeMatrix);
	
	output.pos.x *= vpScale.z;
	output.pos.y *= vpScale.z;
	output.pos.y = -output.pos.y;
	
	output.pos /= output.pos.w;
	output.pos.w = 1.0;
	// input.pos.w isn't really the W component: it's storing the original Z value
	// computed by the game
	if (z_override > -0.9f) {
		output.pos.z = z_override;
	} else {
		output.pos.z = input.pos.w;
	}
	
	// Halve the size of the screen; but scale according to vpScale.w, which is
	// set to g_global_scale or GUI_elem_scale depending on the type of element
	// being drawn
	output.pos.x *= vpScale.w * 0.5 * aspect_ratio;
	output.pos.y *= vpScale.w * 0.5;

	//output.pos /= input.pos.w;

	output.color = input.color.zyxw;
	output.tex = input.tex; // * input.pos.w;
	return output;
}
