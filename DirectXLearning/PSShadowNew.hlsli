Texture2D smap : register(t3);
SamplerComparisonState ssam : register(s1);

#define PCF_RANGE 1

//vec3 doesn’t have to be normalized,
//Translates from world space vector to a coordinate inside our 6xsize shadow map
float2 GetSampleCoordinate(float3 vec3) {
	float2 coord;
	float slice;

	if (abs(vec3.x) >= abs(vec3.y) && abs(vec3.x) >= abs(vec3.z))
	{
		vec3.z = -vec3.z;
		if (vec3.x > 0) //Positive X
		{
			vec3.y = -vec3.y;
			slice = 0;
			vec3 /= vec3.x;
			coord = vec3.zy;
		}
		else
		{
			slice = 1; //Negative X
			vec3 /= vec3.x;
			coord = vec3.zy;
		}
	}
	else if (abs(vec3.y) >= abs(vec3.x) && abs(vec3.y) >= abs(vec3.z))
	{
		if (vec3.y > 0)
		{
			slice = 2; // PositiveY;
			vec3 /= vec3.y;
			coord = vec3.xz;
		}
		else
		{
			vec3.x = -vec3.x;
			slice = 3; // NegativeY;
			vec3 /= vec3.y;
			coord = vec3.xz;
		}
	}
	else
	{
		if (vec3.z > 0) //Positive Z
		{
			vec3.y = -vec3.y;
			slice = 4;
			vec3 /= vec3.z;
			coord = vec3.xy;
		}
		else
		{
			slice = 5; //Negative Z
			vec3 /= vec3.z;
			coord = vec3.xy;
		}
	}

	const float sixth = 1.0f / 6.0f;

	coord = (coord + float2(1, 1)) * 0.5f;
	coord.y = coord.y * sixth + slice * sixth;
	return coord;
}

static const float zf = 100.0f;
static const float zn = 0.5f;
static const float c1 = zf / (zf - zn);
static const float c0 = -zn * zf / (zf - zn);

float CalculateShadowDepth(const in float4 shadowPos)
{
	const float3 m = abs(shadowPos).xyz;
	const float major = max(m.x, max(m.y, m.z));
	return (c1 * major + c0) / major;
}


float ShadowLoop_(const in float3 spos)
{
	float shadowLevel = 0.0f;
	[unroll]
	for (int x = -PCF_RANGE; x <= PCF_RANGE; x++)
	{
		[unroll]
		for (int y = -PCF_RANGE; y <= PCF_RANGE; y++)
		{
			float2 p = GetSampleCoordinate(spos.xyz);
			shadowLevel += smap.SampleCmpLevelZero(ssam, p.xy, spos.b, int2(x, y));
		}
	}
	return shadowLevel / ((PCF_RANGE * 2 + 1) * (PCF_RANGE * 2 + 1));
}

float Shadow(const in float4 shadowHomoPos)
{
	float shadowLevel = 0.0f;

	//float3 spos = shadowHomoPos.xyz / shadowHomoPos.w;
	//if (spos.z > 1.0f || spos.z < 0.0f)
	//{
	//	shadowLevel = 1.0f;
	//}
	//else
	//{
		//shadowLevel = ShadowLoop_(spos);
	float2 p = GetSampleCoordinate(shadowHomoPos.xyz);
	shadowLevel = smap.SampleCmpLevelZero(ssam, p.xy, CalculateShadowDepth(shadowHomoPos));
	//}
	return shadowLevel;
}