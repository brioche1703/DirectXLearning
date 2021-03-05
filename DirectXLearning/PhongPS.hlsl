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

cbuffer ObjectCBuf
{
    float3 materialColor;
    float specularIntensity;
    float specularPower;
};

float4 main(float3 worldPos : Position, float3 n : Normal) : SV_Target
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
    const float3 specular = att * (diffuseColor * diffuseIntensity) * specularIntensity * pow(max(0.0f, dot(normalize(-r), normalize(worldPos))), specularPower);
	
	// Final color
	return float4(saturate(diffuse + ambient + specular) * materialColor, 1.0f);
}