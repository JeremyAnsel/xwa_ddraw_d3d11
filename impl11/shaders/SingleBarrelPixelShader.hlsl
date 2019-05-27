/*

 Based on BlueSkyDefender's Polynomial Barrel Distortion shader from Depth3D, which can be seen here:

 https://github.com/BlueSkyDefender/Depth3D/blob/master/Shaders/Polynomial_Barrel_Distortion_for_HMDs.fx

 https://github.com/crosire/reshade-shaders/blob/master/Shaders/Depth3D.fx

 Used with permission from BlueSkyDefender.

 This version applies the barrel distortion effect to the whole image. Used for the SteamVR version where
 the left and right images are independent.
 */
Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

cbuffer ConstantBuffer : register(b0)
{
	float k1, k2, k3;
	// TODO: Maybe add one constant for barrel_scale (see main() below) and
	//       one constant to enable/disable the vignette or change its size
};

struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
};

float3 genericDistortion(float2 p, float2 LC)
{
	// Lens distortion value, positive values of k1 & k2 give barrel distortion, negative give pincushion.
	float2 q = p - LC;
	float r = length(q);
	float r2 = pow(r, 2.0f);

	float newRadius = (1.0f + r2 * k1) + (k2 * pow(r, 4.0f)) + (k3 * pow(r, 6.0f));
	return float3(newRadius * q + LC, r);
}

float4 main(PixelShaderInput input) : SV_TARGET
{
	float2 texPos = input.tex;
	float barrel_scale = 0.9; // Values lower than 1 increase the size of the barrel viewport.
	float3 distortData;
	float2 LC = { 0.5, 0.5 };

	texPos = (texPos - LC) * barrel_scale + LC;
	distortData = genericDistortion(texPos, LC);
	texPos = distortData.xy;
	// Cut to 0 as texPos approaches the limits of the viewport
	if (texPos.x < 0.0 || texPos.x > 1.0 ||
		texPos.y < 0.0 || texPos.y > 1.0)
		return float4(0, 0, 0, 1);

	// We don't need a vignette for the SteamVR version.
	/*
	if (radius > 0.45f)
	// Regular linear interpolation:
	// vignette = 1.0f * (0.5 - radius) / (0.5 - 0.45) + 0.0 * (radius - 0.45) / (0.5 - 0.45)
	// This formula becomes:
	vignette = (0.5 - radius) / 0.05f;
	*/

	return texture0.Sample(sampler0, texPos);
}
