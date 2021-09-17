#define MAX_NUM_LIGHTS 10

struct PointLight
{
    float3 viewLightPos;
    float3 ambient;
    float3 diffuseColor;
    float diffuseIntensity;
    float attConst;
    float attLin;
    float attQuad;
};

cbuffer PointLightCBuf : register(b0)
{
    int numLights;
    PointLight lights[MAX_NUM_LIGHTS];
};
