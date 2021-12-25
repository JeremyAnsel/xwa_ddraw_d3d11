
struct D3dGlobalLight
{
	float4 direction;
	float4 color;
};

struct D3dLocalLight
{
	float4 position;
	float4 color;
};

cbuffer ConstantBuffer : register(b0)
{
	float4 vpScale;
	float projectionValue1;
	float projectionValue2;
	float projectionDeltaX;
	float projectionDeltaY;
	float floorLevel;
	float cameraPositionX;
	float cameraPositionY;
	float hangarShadowAccStartEnd;
	float s_V0x0662814;
	float s_V0x064D1A4;
	float s_V0x06626F4;
	float s_V0x063D194;
	matrix transformView;
	matrix transformWorldView;
	int globalLightsCount;
	int localLightsCount;
	int renderType;
	int renderTypeIllum;
	D3dGlobalLight globalLights[8];
	D3dLocalLight localLights[8];
};

float4 TransformProjection(float3 input)
{
	float4 pos;
	float st0 = projectionValue1 / input.z;
	pos.x = input.x * st0 + projectionDeltaX;
	pos.y = input.y * st0 + projectionDeltaY;
	pos.z = (st0 * projectionValue2 / 32) / (abs(st0) * projectionValue2 / 32 + projectionValue1 / 3) * 0.5f;
	pos.w = 1.0f;
	pos.x = (pos.x * vpScale.x - 1.0f) * vpScale.z;
	pos.y = (pos.y * vpScale.y + 1.0f) * vpScale.z;
	pos *= 1.0f / st0;
	return pos;
}

float4 ComputeColorLight(float3 v, float3 n)
{
	float3 normal = normalize(n);
	float3 color = float3(0.4f, 0.4f, 0.4f);

	for (int globalLightIndex = 0; globalLightIndex < globalLightsCount; globalLightIndex++)
	{
		float d = dot(globalLights[globalLightIndex].direction.xyz, normal);

		if (d > 0.0f)
		{
			color += globalLights[globalLightIndex].color.xyz * d * globalLights[globalLightIndex].color.w;
		}
	}

	for (int localLightIndex = 0; localLightIndex < localLightsCount; localLightIndex++)
	{
		float3 esp2C = localLights[localLightIndex].position.xyz - v;
		float esp18 = dot(esp2C, n.xyz);

		float st1;
		float3 esp20 = abs(esp2C);

		if (esp20.x >= esp20.y && esp20.x >= esp20.z)
		{
			st1 = esp20.x + (esp20.y + esp20.z) * 0.2941f;
		}
		else if (esp20.y >= esp20.x && esp20.y >= esp20.z)
		{
			st1 = esp20.y + (esp20.x + esp20.z) * 0.2941f;
		}
		else
		{
			st1 = esp20.z + (esp20.x + esp20.y) * 0.2941f;
		}

		float esp2Ca = 1.0f / st1;

		if (esp2Ca * esp18 >= -0.3f && esp2Ca * 0.5f > 0.0f)
		{
			float st2 = esp2Ca * 0.5f * localLights[localLightIndex].color.w;
			color.xyz += localLights[localLightIndex].color.xyz * st2;
		}
	}

	float3 delta = max(color - 1.0f, 0.0f);
	color.x += (delta.y + delta.z) * 0.1f;
	color.y += (delta.x + delta.z) * 0.1f;
	color.z += (delta.x + delta.y) * 0.1f;

	color = min(color, float3(1.0f, 1.0f, 1.0f));

	return float4(color, 0.0f);
}
