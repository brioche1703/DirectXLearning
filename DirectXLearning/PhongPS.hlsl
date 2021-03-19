#include "ShaderOps.hlsl"
#include "LightVectorData.hlsl"
#include "PointLight.hlsl"

cbuffer ObjectCBuf
{
    float specularIntensity;
    float specularPower;
    float padding[2];
};

Texture2D tex;
SamplerState samplerState;

float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal, float2 tc : TexCoord) : SV_Target
{
    viewNormal = normalize(viewNormal);
	// Fragment to light vector data
    const LightVectorData lv = CalculateLightVectorData(viewLightPos, viewFragPos);
	
	// Diffuse attenuation
    const float att = Attenuate(attConst, attLin, attQuad, lv.distToL);
	
	// Diffuse intensity
    const float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lv.dirToL, viewNormal);
	
	// Specular
    const float3 specular = Speculate(diffuseColor, diffuseIntensity, viewNormal, lv.vToL, viewFragPos, att, specularPower);
	
	// Final color
    return float4(saturate((diffuse + ambient) * tex.Sample(samplerState, tc).rgb + specular), 1.0f);
}