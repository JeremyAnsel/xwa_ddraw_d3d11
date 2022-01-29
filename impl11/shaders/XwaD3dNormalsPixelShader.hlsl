
#include "XwaD3dCommon.hlsl"

Texture2D texture0 : register(t0);
Texture2D texture1 : register(t1);
SamplerState sampler0 : register(s0);

struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float4 position : COLOR2;
    float4 normal : COLOR1;
    float2 tex : TEXCOORD;
    float4 color : COLOR0;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
    input.normal = normalize(input.normal);

    float4 color = float4(input.normal.xyz * 0.5f + 0.5f, 1.0f);

    return color;
}
