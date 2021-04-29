Texture2D smap : register(t3);
SamplerState shadowSampler : register(s1);

bool ShadowUnoccluded(const in float4 shadowHomoPos)
{
    const float3 spos = shadowHomoPos.xyz / shadowHomoPos.w;
    return spos.z > 1.0f ? true : smap.Sample(shadowSampler, spos.xy).r > spos.z - 0.005f;

}