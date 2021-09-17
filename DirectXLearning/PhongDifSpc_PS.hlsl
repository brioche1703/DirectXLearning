#include "ShaderOps.hlsli"
#include "LightVectorData.hlsli"
//#include "PointLight.hlsli"
#include "MultipleLights.hlsli"
#include "PSShadow.hlsli"

cbuffer ObjectCBuf : register(b1)
{
    bool useGlossAlpha;
    bool useSpecularMap;
    float3 specularColor;
    float specularWeight;
    float specularGloss;
};

cbuffer ObjectCBuf : register(b3)
{
    bool gammaCorrectionEnabled;
};

Texture2D tex : register(t0);
Texture2D spec : register(t1);

SamplerState samplerState : register(s0);


float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal, float2 tc : Texcoord, float4 spos : ShadowPosition) : SV_Target
{
    float3 diffuse;
    float3 specularReflected;
    float4 color;

    viewNormal = normalize(viewNormal);

    // specular parameters
    float specularPowerLoaded = specularGloss;
    const float4 specularSample = spec.Sample(samplerState, tc);
    float3 specularReflectionColor;
    if (useSpecularMap)
    {
        specularReflectionColor = specularSample.rgb;
    }
    else
    {
        specularReflectionColor = specularColor;
    }
    if (useGlossAlpha)
    {
        specularPowerLoaded = pow(2.0f, specularSample.a * 13.0f);
    }

    const float shadowLevel = Shadow(spos);
    for (int i = 0; i < numLights; i++)
    {
        diffuse = specularReflected = 0.0f;
        if (shadowLevel != 0.0f)
        {
            const LightVectorData lv = CalculateLightVectorData(lights[i].viewLightPos, viewFragPos);
            const float att = Attenuate(lights[i].attConst, lights[i].attLin, lights[i].attQuad, lv.distToL, gammaCorrectionEnabled);
            diffuse = Diffuse(lights[i].diffuseColor, lights[i].diffuseIntensity, att, lv.dirToL, viewNormal);
            specularReflected = Speculate(lights[i].diffuseColor * specularReflectionColor, specularWeight, viewNormal,
                lv.vToL, viewFragPos, att, specularPowerLoaded);
        
            diffuse *= shadowLevel;
            specularReflected *= shadowLevel;
        }

        color += float4(saturate((diffuse + lights[i].ambient) * tex.Sample(samplerState, tc).rgb + specularReflected), 1.0f);
    }
	// final color = attenuate diffuse & ambient by diffuse texture color and add specular reflected
    return gammaCorrectionEnabled ? float4(GammaCorrection(color.rgb), color.a) : color;
}