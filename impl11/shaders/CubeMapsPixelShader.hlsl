
TextureCube texture0 : register(t0);
SamplerState sampler0 : register(s0);

struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float4 position : COLOR0;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
    float4 color = texture0.Sample(sampler0, input.position.xzy);

    if (color.a < 0.5)
    {
        discard;
    }
    
    return color;
}
