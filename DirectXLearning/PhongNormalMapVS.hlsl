cbuffer CBuf
{
    matrix modelView;
    matrix modelViewProj;
};

struct VSOut
{
    float3 viewPos : Position;
    float3 viewNormal : Normal;
    float3 viewTan : Tangent;
    float3 viewBitan : Bitangent;
    float2 tc : Texcoord;
    float4 pos : SV_Position;
};

VSOut main(float3 viewPos : Position, float3 viewNormal : Normal, float3 viewTan : Tangent, float3 viewBitan : Bitangent, float2 tc : Texcoord)
{
    VSOut vso;
    vso.viewPos = (float3) mul(float4(viewPos, 1.0f), modelView);
    vso.viewNormal = mul(viewNormal, (float3x3) modelView);
    vso.viewTan = mul(viewTan, (float3x3) modelView);
    vso.viewBitan = mul(viewBitan, (float3x3) modelView);
    vso.pos = mul(float4(viewPos, 1.0f), modelViewProj);
    vso.tc = tc;
    return vso;
}