/*=============================================================================

	Adapted by Leo Reyes with permission from:

	ReShade 4 effect file
	github.com/martymcmodding

	Support Pascal Gilcher at:
		paypal.me/mcflypg
		patreon.com/mcflypg

	Simple Bloom
	by Marty McFly / P.Gilcher
	part of qUINT shader library for ReShade 3

	CC BY-NC-ND 3.0 licensed.

=============================================================================*/

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

static float2 SCREEN_SIZE = float2(1920, 1080);
static float BLOOM_INTENSITY = 1.2;
static float BLOOM_CURVE = 1.5;
static float BLOOM_SAT = 2.0;
static float BLOOM_LAYER_MULT_1 = 0.05;
static float BLOOM_LAYER_MULT_2 = 0.05;
static float BLOOM_LAYER_MULT_3 = 0.05;
static float BLOOM_LAYER_MULT_4 = 0.1;
static float BLOOM_LAYER_MULT_5 = 0.5;
static float BLOOM_LAYER_MULT_6 = 0.01;
static float BLOOM_LAYER_MULT_7 = 0.01;
static float BLOOM_ADAPT_STRENGTH = 0.5;
static float BLOOM_ADAPT_EXPOSURE = 0.0;
static float BLOOM_ADAPT_SPEED = 2.0;
static float BLOOM_TONEMAP_COMPRESSION = 4.0;

struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD;
};

float4 downsample(Texture2D tex, SamplerState s, float2 tex_size, float2 uv)
{
	float2 offset_uv = 0;
	float2 kernel_small_offsets = float2(2.0, 2.0) / tex_size;
	float4 kernel_center = tex.Sample(s, uv);
	float4 kernel_small = 0;

	offset_uv.xy  = uv + kernel_small_offsets;
	kernel_small += tex.Sample(s, offset_uv); //++
	offset_uv.x   = uv.x - kernel_small_offsets.x;
	kernel_small += tex.Sample(s, offset_uv); //-+
	offset_uv.y   = uv.y - kernel_small_offsets.y;
	kernel_small += tex.Sample(s, offset_uv); //--
	offset_uv.x   = uv.x + kernel_small_offsets.x;
	kernel_small += tex.Sample(s, offset_uv); //+-

	// This looks like it's taking the average
	return (kernel_center + kernel_small) / 5.0;
}

//void PS_BloomPrepass(in float4 pos : SV_Position, in float2 uv : TEXCOORD0, out float4 color : SV_Target0)
float4 main(PixelShaderInput input) : SV_TARGET
{
	float4 color = downsample(texture0, sampler0, SCREEN_SIZE, input.uv);
	//color.w = saturate(dot(color.rgb, 0.333)); // Original value
	color.w = saturate(dot(color.rgb, 0.9));     // Looks like this is the threshold for the bloom effect: higher --> smaller bloom

	color.rgb = lerp(color.w, color.rgb, BLOOM_SAT);
	color.rgb *= (pow(color.w, BLOOM_CURVE) * BLOOM_INTENSITY * BLOOM_INTENSITY * BLOOM_INTENSITY) / (color.w + 1e-3);
	return color;
}
