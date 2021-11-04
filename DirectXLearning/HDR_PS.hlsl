Texture2D tex;
SamplerState splr;

float4 main(float2 uv : Texcoord) : SV_Target
{
    const float gamma = 2.2f;
    float exposure = 1.0f;
    
    float3 hdrColor = tex.Sample(splr, uv).rgb;

    float3 color = 1.0f;
    color = color - exp(-hdrColor * exposure);
    color = pow(color, 1.0f / gamma);

    return float4(color, 1.0f);
}