// Copyright (c) 2014 Jérémy Ansel
// Licensed under the MIT license. See LICENSE.txt


cbuffer ConstantBuffer : register(b0)
{
	// Only vpScale and bFullTransform are used in this shader
	float3 vpScale;
	//float aspect_ratio, cockpit_threshold, z_override, sz_override;
	//float mult_z_override, bPreventTransform, bFullTransform;
};

/*
cbuffer ConstantBuffer : register(b1)
{
	matrix projEyeMatrix;
	matrix viewMatrix;
	matrix fullViewMatrix;
};
*/

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

	output.pos.x = (input.pos.x * vpScale.x - 1.0f) * vpScale.z;
	output.pos.y = (input.pos.y * vpScale.y + 1.0f) * vpScale.z;
	output.pos.z = input.pos.z;
	output.pos.w = 1.0f;

	output.pos *= 1.0f / input.pos.w;

	output.color = input.color.zyxw;

	output.tex = input.tex;

	return output;
}
