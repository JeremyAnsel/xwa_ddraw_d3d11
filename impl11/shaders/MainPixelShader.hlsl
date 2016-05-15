// Copyright (c) 2014 J�r�my Ansel
// Licensed under the MIT license. See LICENSE.txt


Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
	float4 texelColor = texture0.Sample(sampler0, input.tex);

	if (texelColor.a > 0.4f)
	{
		discard;
	}
	texelColor.xyz *= 1.0f / (1.0f - texelColor.a);

	return float4(texelColor.xyz, 1.0f);
}
