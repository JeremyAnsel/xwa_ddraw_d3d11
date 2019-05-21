// Copyright (c) 2014 Jérémy Ansel
// Licensed under the MIT license. See LICENSE.txt
// Extended for VR by Leo Reyes, 2019

cbuffer ConstantBuffer : register(b0)
{
	float4 vpScale;
	float aspect_ratio, parallax, z_override;
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

	output.pos.x = (input.pos.x * vpScale.x - 1.0f + parallax) * vpScale.z;
	output.pos.y = (input.pos.y * vpScale.y + 1.0f) * vpScale.z;
	if (z_override > -0.9f) {
		output.pos.z = z_override;
	} else {
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
