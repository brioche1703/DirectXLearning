#include "ShaderOps.hlsli"
#include "LightVectorData.hlsli"
#include "MultipleLights.hlsli"
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
    float4 color;

    // normalize the mesh normal
    viewNormal = normalize(viewNormal);

    const float shadowLevel = Shadow(spos);
    for (int i = 0; i < numLights; i++)
    {
        diffuse = specular = 0.0f;
        if (shadowLevel != 0.0f)
        {

	        // fragment to light vector data
            const LightVectorData lv = CalculateLightVectorData(lights[i].viewLightPos, viewFragPos);
	        // attenuation
            const float att = Attenuate(lights[i].attConst, lights[i].attLin, lights[i].attQuad, lv.distToL, gammaCorrectionEnabled);
	        // diffuse
            diffuse = Diffuse(lights[i].diffuseColor, lights[i].diffuseIntensity, att, lv.dirToL, viewNormal);
            // specular
            specular = Speculate(lights[i].diffuseColor * lights[i].diffuseIntensity * specularColor, 
                specularWeight, viewNormal, lv.vToL, viewFragPos, att, specularGloss);

            diffuse *= shadowLevel;
            specular *= shadowLevel;
        }

        color += float4(saturate((diffuse + lights[i].ambient) * materialColor + specular), 1.0f);
    }

	// final color
    return gammaCorrectionEnabled ? float4(GammaCorrection(color.rgb), color.a) : color;
}