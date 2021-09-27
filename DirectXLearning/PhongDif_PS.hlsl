#include "ShaderOps.hlsli"
#include "LightVectorData.hlsli"
#include "MultipleLights.hlsli"
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
    float4 color;

    viewNormal = normalize(viewNormal);

    const float shadowLevel = Shadow(spos);
    for (int i = 0; i < numLights; i++)
    {
        diffuse = specular = 0.0f;
        if (shadowLevel != 0.0f)
        {
	        // Fragment to light vector data
            const LightVectorData lv = CalculateLightVectorData(lights[i].viewLightPos, viewFragPos);
	
	        // Diffuse attenuation
            const float att = Attenuate(lights[i].attConst, lights[i].attLin, lights[i].attQuad, lv.distToL);
	
	        // Diffuse intensity
            diffuse = Diffuse(lights[i].diffuseColor, lights[i].diffuseIntensity, att, lv.dirToL, viewNormal);
	
	        // Specular
            specular = Speculate(lights[i].diffuseColor * lights[i].diffuseIntensity * specularColor, specularWeight, viewNormal, lv.vToL, viewFragPos, att, specularGloss);

            diffuse *= shadowLevel;
            specular *= shadowLevel;
        }

        color += float4(saturate((diffuse + lights[i].ambient) * tex.Sample(samplerState, tc).rgb + specular), 1.0f);
    }
	
	// Final color
    return gammaCorrectionEnabled ? float4(GammaCorrection(color.rgb), color.a) : color;
}