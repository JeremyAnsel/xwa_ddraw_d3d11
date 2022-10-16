// Copyright (c) 2014 Jérémy Ansel
// Licensed under the MIT license. See LICENSE.txt


cbuffer ConstantBuffer : register(b0)
{
	float4 vpScale;
	float s_V0x08B94CC;
	float s_V0x05B46B4;
	float unused1;
	float unused2;
	float4 projectionParameters;
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

	//output.pos.x = (input.pos.x * vpScale.x - 1.0f) * vpScale.z;
	//output.pos.y = (input.pos.y * vpScale.y + 1.0f) * vpScale.z;
	//output.pos.z = input.pos.z;
	//output.pos.w = 1.0f;
	//output.pos *= 1.0f / input.pos.w;

	float st0 = input.pos.w;

	if (input.pos.z == input.pos.w)
	{
		//float z = s_V0x05B46B4 / input.pos.w - s_V0x05B46B4;
		//st0 = s_V0x08B94CC / z;

		float z = s_V0x05B46B4 / input.pos.w;
		st0 = s_V0x08B94CC / z;
    }

	output.pos.z = (st0 * s_V0x05B46B4 / projectionParameters.x) / (abs(st0) * s_V0x05B46B4 / projectionParameters.y + s_V0x08B94CC * projectionParameters.z);

	output.pos.w = 1.0f;
	output.pos.x = (input.pos.x * vpScale.x - 1.0f) * vpScale.z;
	output.pos.y = (input.pos.y * vpScale.y + 1.0f) * vpScale.z;
	output.pos *= 1.0f / input.pos.w;

	output.color = input.color.zyxw;

	output.tex = input.tex;

	return output;
}
