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
    bool useNormalMap;
    float normalMapWeight;
};

cbuffer ObjectCBuf : register(b3)
{
    bool gammaCorrectionEnabled;
};

Texture2D tex : register(t0);
Texture2D spec : register(t1);
Texture2D nmap : register(t2);

SamplerState splr : register(s0);

float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal, float3 viewTan : Tangent, float3 viewBitan : Bitangent, float2 tc : Texcoord, float4 spos : ShadowPosition) : SV_Target
{
    float3 diffuse;
    float3 specularReflected;
    float4 color;

    // alpha testing
    const float4 dtex = tex.Sample(splr, tc);
    
#ifdef MASK_BOI
    clip(dtex.a < 0.1f ? -1 : 1);
    // flip normal when backface
    if (dot(viewNormal, viewFragPos) >= 0.0f)
    {
        viewNormal = -viewNormal;
    }
#endif

    viewNormal = normalize(viewNormal);
    // replace normal with mapped if normal mapping enabled
    if (useNormalMap)
    {
        const float3 mappedNormal = MapNormal(normalize(viewTan), normalize(viewBitan), viewNormal, tc, nmap, splr);
        viewNormal = lerp(viewNormal, mappedNormal, normalMapWeight);
    }

    // specular parameter determination (mapped or uniform)
    float specularPower = specularGloss;
    const float4 specularSample = spec.Sample(splr, tc);
    
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
        specularPower = pow(2.0f, specularSample.a * 13.0f);
    }
    
    const float shadowLevel = Shadow(spos);
    for (int i = 0; i < numLights; i++)
    {
        diffuse = specularReflected = 0.0f;
        if (shadowLevel != 0.0f)
        {
            // fragment to light vector data
            const LightVectorData lv = CalculateLightVectorData(lights[i].viewLightPos, viewFragPos);
    
            // attenuation
            const float att = Attenuate(lights[i].attConst, lights[i].attLin, lights[i].attQuad, lv.distToL, gammaCorrectionEnabled);
    	    // diffuse light
            diffuse = Diffuse(lights[i].diffuseColor, lights[i].diffuseIntensity, att, lv.dirToL, viewNormal);

            // specular reflected
            specularReflected = Speculate(lights[i].diffuseColor * lights[i].diffuseIntensity * specularReflectionColor,
            specularWeight, viewNormal, lv.vToL, viewFragPos, att, specularPower);

            diffuse *= shadowLevel;
            specularReflected *= shadowLevel;
        }

	    // final color = attenuate diffuse & ambient by diffuse texture color and add specular reflected
        color += float4(saturate((diffuse + lights[i].ambient) * dtex.rgb + specularReflected), 1.0f);
    }
    return gammaCorrectionEnabled ? float4(GammaCorrection(color.rgb), color.a) : color;

}