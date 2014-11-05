
struct VertexShaderInput
{
	float2 pos : POSITION;
	float2 tex : TEXCOORD;
};

struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
};

PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput output;

	output.pos = float4(input.pos.x, input.pos.y, 0.5f, 1.0f);
	output.tex = input.tex;

	return output;
}
