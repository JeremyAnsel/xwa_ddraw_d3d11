//static float2 SCREEN_SIZE = float2(1920, 1080);
static float2 SCREEN_SIZE = float2(3840, 2160);
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