Texture2D smap : register(t3);
SamplerComparisonState ssamHW : register(s1);
SamplerState ssam : register(s2);

cbuffer ShadowControl : register(b2)
{
	int pcfLevel;
	float depthBias;
	bool hardwarePcf;
}

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

float2 GetSampleOffsetY(float2 coord, float yOffset)
{
	const float sixth = 1.0f / 6.0f;

	float yOffsetNormalized = yOffset * sixth;

	float2 returnValue;
	float2 output = coord + float2(0, yOffsetNormalized);

	if (trunc(output.y * 6) == trunc(coord.y * 6) && output.y > 0)
	{
		return output;
	}
	else 
	{
		return coord;
	}
}

float2 GetSampleOffsetX(float2 coord, float xOffset)
{
	float2 returnValue;
	float2 output = coord + float2(xOffset, 0);

	return output.x == saturate(output.x) ? output : coord;
}

float2 GetSampleOffset(float2 coord, float2 offset)
{
	coord = GetSampleOffsetX(coord, offset.x);
	coord = GetSampleOffsetY(coord, offset.y);

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


float ShadowLoop_(const in float4 spos, uniform int range)
{
	float shadowLevel = 0.0f;
	float texelSize = 1.0f / 1000;
	float2 p = GetSampleCoordinate(spos.xyz);

	[unroll]
	for (int x = -range; x <= range; x++)
	{
		[unroll]
		for (int y = -range; y <= range; y++)
		{
			float3 sampleShadow = 0.0f;
			const float2 sOffset = GetSampleOffset(p.xy, float2(x, y) * texelSize);
			if (hardwarePcf)
			{
				sampleShadow = smap.SampleCmpLevelZero(ssamHW, sOffset, CalculateShadowDepth(spos) - depthBias);
			}
			else
			{
				sampleShadow = smap.Sample(ssam, sOffset).r >= CalculateShadowDepth(spos) - depthBias ? 1.0f : 0.0f;
			}
			shadowLevel += sampleShadow.r;
		}
	}
	return shadowLevel / ((range * 2 + 1) * (range * 2 + 1));
}

float Shadow(const in float4 shadowHomoPos)
{
	float shadowLevel = 0.0f;
	const float3 spos = shadowHomoPos.xyz / shadowHomoPos.z;

	if (spos.z > 1.0f + depthBias || spos.z < 0.0f - depthBias)
	{
		shadowLevel = 0.0f;
	}
	else
	{

	[unroll]
	for (int level = 0; level <= 4; level++)
	{
		if (level == pcfLevel)
		{
			shadowLevel = ShadowLoop_(shadowHomoPos, level);
		}
	}

	}
	return shadowLevel;
}
