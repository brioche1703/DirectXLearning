Texture2D tex;
SamplerState splr;

cbuffer ObjectCBuf : register(b4)
{
    bool hdrEnable;
    float exposure;
};

float4 main(float2 uv : Texcoord) : SV_Target
{
    const float gamma = 2.2f;
    
    float3 color = tex.Sample(splr, uv).rgb;

    float3 hdrColor = 1.0f;
    hdrColor = hdrColor - exp(-color * exposure);
    hdrColor = pow(hdrColor, 1.0f / gamma);

    return hdrEnable ? float4(hdrColor, 1.0f) : float4(color, 1.0f);
}