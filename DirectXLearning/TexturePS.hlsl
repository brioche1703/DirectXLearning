Texture2D tex;

SamplerState samplerState;

float4 main(float2 tc : TexCoord) : SV_Target
{
	return tex.Sample(samplerState, tc);
}