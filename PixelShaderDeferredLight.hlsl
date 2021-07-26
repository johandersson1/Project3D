Texture2D posTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D worldPosTexture : register(t2);
Texture2D diffuseAlbedoTexture : register(t3);

Texture2D ambientMatTexture : register(t4);
Texture2D diffuseMatTexture : register(t5);
Texture2D specularMatTexture : register(t6);
Texture2D shadowMapTexture : register(t7);

SamplerState mySampler : register(s0);

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
    float4 V = { 1, 1, 1, 1 };

    float3 E = normalize(cameraPosition - P.xyz);

   // float3 lPos = { 10, 10, 10 };
    
   
    float diffuse = saturate(dot(N, direction));

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
    
    float3 normal = normalTexture.Sample(mySampler, input.tex).xyz;
    float4 albedo = diffuseAlbedoTexture.Sample(mySampler, input.tex);
    float4 worldPos = worldPosTexture.Sample(mySampler, input.tex);
   // float4 shadowPosH = shadowMapTexture.Sample(testSampler, input.tex);
    normal = normalize(normal);

    //Material properties
    float4 ambientMaterial = ambientMatTexture.Sample(mySampler, input.tex);
    float4 diffuseMaterial = diffuseMatTexture.Sample(mySampler, input.tex);
    float4 specularMaterial = specularMatTexture.Sample(mySampler, input.tex);
    
    float4 shadowMap = shadowMapTexture.Sample(mySampler, input.tex);
    
    if (diffuseMaterial.x < 0)
    {
        return albedo;
    }
    
    LightResult lResult = LightCalculation(worldPos, normal, diffuseMaterial, specularMaterial);
    
    
    
    
    float4 globalAmbient = { 0.6f, 0.6f, 0.6f, 1.0f };
    float4 A = ambientMaterial;
    
    float4 finalColor = albedo * (lResult.diffuse + lResult.specular + A) * globalAmbient * shadowMap.r;
    return finalColor;
}