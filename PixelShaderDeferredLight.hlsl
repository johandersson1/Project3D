Texture2D posTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D worldPosTexture : register(t2);
Texture2D diffuseAlbedoTexture : register(t3);

Texture2D ambientMatTexture : register(t4);
Texture2D diffuseMatTexture : register(t5);
Texture2D specularMatTexture : register(t6);
Texture2D lightClipPos : register(t7);

Texture2D shadowMapTex : register(t8);

SamplerState wrapSampler : register(s0);
SamplerState clampSampler : register(s1);

cbuffer DirectionalLight : register(b0)
{
    float4 color;
    float3 direction;
    float lightRange;
};

cbuffer pos : register(b1)
{
    float3 cameraPosition;
}

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD;
};

struct LightResult
{
    float4 diffuse;
    float4 specular;
    float4 color;
};

LightResult LightCalculation(float4 P, float3 N, float4 D, float4 S)
{
    LightResult result = { { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } };
    float4 V = { 1, 1, 1, 1 };		// safeguard

    float3 E = normalize(cameraPosition - P.xyz);	//	camera to pixel vector

    float diffuse = saturate(dot(N, direction));	// dot product of normal and direction

    if (diffuse < 0.0f) // IF BACKFACED
        continue;

    if (D.x > 0)
        result.diffuse += diffuse * D;
    else
        result.diffuse += diffuse * V;

    float3 R = normalize(reflect(-direction, N));
    float specular = pow(saturate(dot(R, E)), 1);

    if (S.x > 0)
        result.specular += specular * S;
    else
        result.specular += specular * V;

    return result;
}


float4 main(PixelInput input) : SV_Target
{
    float3 normal = normalTexture.Sample(wrapSampler, input.tex).xyz;
    float4 albedo = diffuseAlbedoTexture.Sample(wrapSampler, input.tex);
    float4 worldPos = worldPosTexture.Sample(wrapSampler, input.tex);
   // float4 shadowPosH = shadowMapTexture.Sample(testSampler, input.tex);
    normal = normalize(normal);

    //Material properties
    float4 ambientMaterial = ambientMatTexture.Sample(wrapSampler, input.tex);
    float4 diffuseMaterial = diffuseMatTexture.Sample(wrapSampler, input.tex);
    float4 specularMaterial = specularMatTexture.Sample(wrapSampler, input.tex);
    
    float4 lightClip = lightClipPos.Sample(wrapSampler, input.tex);
    
    //SHADOWS
    lightClip.xyz /= lightClip.w;													 // Complete projection of textureCoords by doing division by w.  --- Frank Luna 21.4.5 The Shadow Factor
    float depth = lightClip.z;														 // Depth in the NDC-Space --- Frank Luna 21.4.5 The Shadow Factor --- Normalized Device Coordinates - Figure out what obscures what
    float2 tx = float2(0.5f * lightClip.x + 0.5f, -0.5f * lightClip.y + 0.5);        // [-1,1] => [0, 1] -> in d3d11 the y-axis is positive when pointing in clip-space. 
																				     // when working with UVs or texcoords the y-axis positive in HLSL y-axis is pointing downwards. invert here

    float sm = shadowMapTex.Sample(wrapSampler, tx).r;
    float shadow = (sm + 0.005f < depth) ? sm : 1.0f;								 // If sm + bias (closest) < clip-depth there is a primitive castings shadow.
																					 // else - no shadows
    if (diffuseMaterial.x < 0)														 // For the particles
    {
        return albedo;
    }
    
    LightResult lResult = LightCalculation(worldPos, normal, diffuseMaterial, specularMaterial);  // The light calculations
    
    float4 lighting = (lResult.diffuse + lResult.specular) * shadow;
        
    float4 globalAmbient = { 0.4f, 0.4f, 0.4f, 1.0f };
    float4 A = ambientMaterial;
    
    float4 finalColor = albedo * (lighting + A) * globalAmbient;
    return finalColor;
    //return float4(shadow, shadow, shadow, 1);
}