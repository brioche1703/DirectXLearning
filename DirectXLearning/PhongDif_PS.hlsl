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

cbuffer ObjectCBuf : register(b3)
{
    bool gammaCorrectionEnabled;
};
Texture2D tex : register(t0);
SamplerState samplerState : register(s0);

float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal, float2 tc : TexCoord, float4 spos : ShadowPosition) : SV_Target
{
    float3 diffuse;
    float3 specular;

    const float shadowLevel = Shadow(spos);
    if (shadowLevel != 0.0f)
    {
        viewNormal = normalize(viewNormal);
	// Fragment to light vector data
        const LightVectorData lv = CalculateLightVectorData(viewLightPos, viewFragPos);
	
	// Diffuse attenuation
        const float att = Attenuate(attConst, attLin, attQuad, lv.distToL, gammaCorrectionEnabled);
	
	// Diffuse intensity
        diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lv.dirToL, viewNormal);
	
	// Specular
        specular = Speculate(diffuseColor * diffuseIntensity * specularColor, specularWeight, viewNormal, lv.vToL, viewFragPos, att, specularGloss);

        diffuse *= shadowLevel;
        specular *= shadowLevel;
    }
    else
    {
        diffuse = specular = 0.0f;
    }
	
	// Final color
    float4 color = float4(saturate((GammaCorrection(diffuse) + ambient) * tex.Sample(samplerState, tc).rgb + specular), 1.0f);
    return gammaCorrectionEnabled ? float4(GammaCorrection(color.rgb), color.a) : color;
}