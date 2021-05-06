TextureCube tex : register(t0);
SamplerState ss : register(s0);

float4 main(float3 worldPos : Position) : SV_TARGET
{
    return tex.Sample(ss, worldPos);
}