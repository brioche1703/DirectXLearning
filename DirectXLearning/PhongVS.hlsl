#include "Transform.hlsl"

struct VSOut
{
	float3 viewPos : Position;
	float3 viewNormal : Normal;
    float2 tc : TexCoord;
	float4 pos : SV_Position;
};

VSOut main(float3 pos : Position, float3 n : Normal, float2 tc : TexCoord)
{
	VSOut vso;
	vso.viewPos = (float3)mul(float4(pos, 1.0f), modelView);
	vso.viewNormal = mul(n, (float3x3)modelView); // use 3x3 to get rid of translation which we do not want!
	vso.pos = mul(float4(pos, 1.0f), modelViewProj);
    vso.tc = tc;
	
	return vso;
}