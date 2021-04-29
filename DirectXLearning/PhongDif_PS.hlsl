#include "ShaderOps.hlsli"
#include "LightVectorData.hlsli"
#include "PointLight.hlsli"
#include "PSShadow.hlsli"

cbuffer ObjectCBuf : register(b1)
{
    float3 specularColor;
    float specularWeight;
    float specularGloss;
};

Texture2D tex : register(t0);
SamplerState samplerState : register(s0);

float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal, float2 tc : TexCoord, float4 spos : ShadowPosition) : SV_Target
{
    float3 diffuse;
    float3 specular;

    if (ShadowUnoccluded(spos))
    {
        viewNormal = normalize(viewNormal);
	// Fragment to light vector data
        const LightVectorData lv = CalculateLightVectorData(viewLightPos, viewFragPos);
	
	// Diffuse attenuation
        const float att = Attenuate(attConst, attLin, attQuad, lv.distToL);
	
	// Diffuse intensity
        diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lv.dirToL, viewNormal);
	
	// Specular
        specular = Speculate(diffuseColor * diffuseIntensity * specularColor, specularWeight, viewNormal, lv.vToL, viewFragPos, att, specularGloss);
    }
    else
    {
        diffuse = specular = 0.0f;
    }
	
	// Final color
    return float4(saturate((diffuse + ambient) * tex.Sample(samplerState, tc).rgb + specular), 1.0f);
}