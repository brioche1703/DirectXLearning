cbuffer LightCBuf
{
	float3 lightPos;
    float3 ambient;
    float3 diffuseColor;
    float diffuseIntensity;
    float attConst;
    float attLin;
    float attQuad;
};

Texture2D tex;
Texture2D spec;

SamplerState samplerState;

float4 main(float3 worldPos : Position, float3 n : Normal, float2 tc : TexCoord) : SV_Target
{
	// Fragment to light vector data
	const float3 vTol = lightPos - worldPos;
	const float distToL = length(vTol);
	const float3 dirToL = vTol / distToL;
	
	// Diffuse attenuation
    const float att = 1.0f / (attConst + attLin * distToL + attQuad * (distToL * distToL));
	
	// Diffuse intensity
	const float3 diffuse = diffuseColor * diffuseIntensity * att * max(0.0f, dot(dirToL, n));
	
	// Specular
	// reflected light vector
    const float3 w = n * dot(vTol, n);
    const float3 r = w * 2.0f - vTol;
	// calculate specular intensity based on angle between viewing vector and reflection vector, narrow with power function
    const float4 specularSample = spec.Sample(samplerState, tc);
    const float3 specularReflectionColor = specularSample.rgb;
    const float specularPower = pow(2.0f, specularSample.a * 13.0f);
    const float3 specular = att * (diffuseColor + diffuseIntensity) * pow(max(0.0f, dot(normalize(-r), normalize(worldPos))), specularPower);
	
	// Final color
    return float4(saturate((diffuse + ambient) * tex.Sample(samplerState, tc).rgb + specular * specularReflectionColor), 1.0f);
}
