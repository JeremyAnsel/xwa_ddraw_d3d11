// Copyright (c) 2014 Jérémy Ansel
// Licensed under the MIT license. See LICENSE.txt
// Extended for VR/Dynamic Cockpit by Leo Reyes, 2019.

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

	output.pos.x =  (input.pos.x - 0.5) * 2.0;
	output.pos.y = -(input.pos.y - 0.5) * 2.0;
	output.pos.z = input.pos.z;
	output.pos.w = 1.0f;
	output.color = input.color.zyxw;
	output.tex = input.tex;
	return output;
}
