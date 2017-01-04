// Copyright (c) 2014 Jérémy Ansel
// Licensed under the MIT license. See LICENSE.txt


Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
	float2 tex : TEXCOORD;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
	float4 texelColor = texture0.Sample(sampler0, input.tex);
	texelColor *= input.color;
	if (texelColor.a == 0) discard;
	// Otherwise stars will be visible around light source textures in TIE Fighter
	// TODO: possibly should depended on alpha-set-to-one?? Can't find that setting though.
	// XvT seems to look a bit better without this, maybe depend on game?
	texelColor.a = 1.0f;
	return texelColor;
}
