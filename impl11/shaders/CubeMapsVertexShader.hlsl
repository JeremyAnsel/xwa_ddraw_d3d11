
cbuffer ConstantBuffer : register(b0)
{
    float4 vpScale;
    float projectionValue1;
    float projectionValue2;
    float projectionDeltaX;
    float projectionDeltaY;
    float4 projectionParameters;
    matrix transformView;
    matrix transformWorld;
}

struct VertexShaderInput
{
    float4 pos : POSITION;
};

struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float4 position : COLOR0;
};

float4 TransformProjection(float3 input)
{
    float4 pos;
    float st0 = projectionValue1 / input.z;
    pos.x = input.x * st0 + projectionDeltaX;
    pos.y = input.y * st0 + projectionDeltaY;

    pos.z = (st0 * projectionValue2 / projectionParameters.x) / (abs(st0) * projectionValue2 / projectionParameters.y + projectionValue1 * projectionParameters.z);

	//pos.w = 1.0f;
    pos.x = (pos.x * vpScale.x - 1.0f) * vpScale.z;
    pos.y = (pos.y * vpScale.y + 1.0f) * vpScale.z;
	//pos *= 1.0f / st0;
    pos.xyz = pos.xyz * input.z / projectionValue1;
    pos.w = input.z / projectionValue1;

    return pos;
}

PixelShaderInput main(VertexShaderInput input)
{
    PixelShaderInput output;

    output.pos = input.pos;
    output.pos = mul(output.pos, transformWorld);
    output.pos = mul(output.pos, transformView);
    output.pos = TransformProjection(output.pos.xyz);
    output.position = input.pos;
    
    return output;
}
