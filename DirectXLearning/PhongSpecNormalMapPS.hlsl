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
    bool normalMapEnabled;
    bool specularMapEnabled;
    bool hasAplhaGloss;
    float specularPowerConst;
    float3 specularColor;
    float specularMapWeight;
};

Texture2D tex;
Texture2D spec;
Texture2D nmap;

SamplerState samplerState;

float4 main(float3 viewPos : Position, float3 n : Normal, float3 tan : Tangent, float3 bitan : Bitangent, float2 tc : TexCoord) : SV_Target
{
    if (normalMapEnabled)
    {
        const float3x3 tanToView = float3x3(normalize(tan), normalize(bitan), normalize(n));

        const float3 normalSample = nmap.Sample(samplerState, tc).xyz;
        n = normalSample * 2.0f - 1.0f;
        n = mul(n, tanToView);
    }
    
	// Fragment to light vector data
        const float3 vTol = lightPos - viewPos;
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
    float3 specularReflectionColor;
    float specularPower = specularPowerConst;
    if (specularMapEnabled)
    {
        const float4 specularSample = spec.Sample(samplerState, tc);
        specularReflectionColor = specularSample.rgb * specularMapWeight;
        if (hasAplhaGloss)
        {
            specularPower = pow(2.0f, specularSample.a * 13.0f);
        }
    }
    else
    {
        specularReflectionColor = specularColor;
    }
        const float3 specular = att * (diffuseColor + diffuseIntensity) * pow(max(0.0f, dot(normalize(-r), normalize(viewPos))), specularPower);
	
	// Final color
        return float4(saturate((diffuse + ambient) * tex.Sample(samplerState, tc).rgb + specular * specularReflectionColor), 1.0f);
    }
