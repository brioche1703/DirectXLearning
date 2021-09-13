#include "ShaderOps.hlsli"
#include "LightVectorData.hlsli"
#include "PointLight.hlsli"
#include "PSShadow.hlsli"

cbuffer ObjectCBuf : register(b1)
{
    float3 materialColor;
    float3 specularColor;
    float specularWeight;
    float specularGloss;
};

cbuffer ObjectCBuf : register(b3)
{
    bool gammaCorrectionEnabled;
};

float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal, float4 spos : ShadowPosition) : SV_Target
{    
    float3 diffuse;
    float3 specular;

    const float shadowLevel = Shadow(spos);
    if (shadowLevel != 0.0f)
    {
    // normalize the mesh normal
        viewNormal = normalize(viewNormal);
	// fragment to light vector data
        const LightVectorData lv = CalculateLightVectorData(viewLightPos, viewFragPos);
	// attenuation
        const float att = Attenuate(attConst, attLin, attQuad, lv.distToL, gammaCorrectionEnabled);
	// diffuse
        diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lv.dirToL, viewNormal);
    // specular
        specular = Speculate(
            diffuseColor * diffuseIntensity * specularColor, specularWeight, viewNormal,
            lv.vToL, viewFragPos, att, specularGloss
        );
        diffuse *= shadowLevel;
        specular *= shadowLevel;
    }
    else
    {
        diffuse = specular = 0.0f;
    }
	// final color
    float4 color = float4(saturate((diffuse + ambient) * materialColor + specular), 1.0f);
    return gammaCorrectionEnabled ? float4(GammaCorrection(color.rgb), color.a) : color;
}