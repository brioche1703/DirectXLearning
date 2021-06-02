Texture2D tex : register(t3);
SamplerState ss : register(s0);

static const float far = 100.0f;
static const float near = 0.5f;

float LinearizeDepth(float depth) {
	float z = depth * 2.0f - 1.0f;
	return (2.0f * near * far) / (far + near - z * (far - near));
}

float4 main(float2 tc : Texcoord) : SV_Target
{
	float depth = tex.Sample(ss, tc).r;
	float linDepth = LinearizeDepth(depth) / far;
	return float4(linDepth, linDepth, linDepth, 1.0f);
}
