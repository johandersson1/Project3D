Texture2D posTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D worldPosTexture : register(t2);
Texture2D diffuseAlbedoTexture : register(t3);

Texture2D ambientMatTexture : register(t4);
Texture2D diffuseMatTexture : register(t5);
Texture2D specularMatTexture : register(t6);

SamplerState mySampler : register(s0);

cbuffer DirectionalLight : register(b0)
{
    float4 color;
    float3 lightPos;
    float lightRange;
    float att0;
    float att1;
    float att2;
    float padding;
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

    float3 E = normalize(cameraPosition - P.xyz);

   // float3 lPos = { 10, 10, 10 };
    
    for (int i = 0; i < 1; ++i)
    {
        float3 L = normalize(lightPos - P.xyz);
        float dist = length(L);
        L /= dist;

        if (dist > lightRange) // IF TOO FAR AWAY
            continue;

        float diffuse = saturate(dot(N, L));

        if (diffuse < 0.0f) // IF BACKFACED
            continue;

        float attenuation = 1.0f / (att0 + dist * att1 + dist * dist * att2);

        if (D.x > 0)
            result.diffuse += diffuse * attenuation * D;
        else
            result.diffuse += diffuse * attenuation;

        float3 R = normalize(reflect(-L, N));
        float specular = pow(saturate(dot(R, E)), 1);

        if (S.x > 0)
            result.specular += specular * attenuation * S;
        else
            result.specular += specular * attenuation;
    }

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
        
    if (diffuseMaterial.x < 0)
    {
        return albedo;
    }
    
    LightResult lResult = LightCalculation(worldPos, normal, diffuseMaterial, specularMaterial);
    float4 globalAmbient = { 0.3f, 0.3f, 0.3f, 1.0f };
    float4 A = ambientMaterial * globalAmbient;
    
    float4 finalColor = albedo * (lResult.diffuse + lResult.specular + A);
    return finalColor;
    
    
   // float ShinynessFactor = 32;
   // float specularFactor = 0;
   // float att = 0;
   // float4 specularComponent = float4(0, 0, 0, 1);
    
   // if (length(specularMaterial) == 0)
   // {
   //     return albedo;
   // }
    
   // if (diffuseFactor > 0)
   // {
   //     float3 v = reflect(float4(-lightVector, 0), normal);
   //     float3 toEye = normalize(float4(camPos.xyz, 1) - worldPos);
   //     specularFactor = pow(max(dot(toEye, v), 0), ShinynessFactor);
   //     att = lightStrength / (att0 + att1 * lightDistance + att2 * pow(lightDistance, 2));
   //     specularComponent = (lightSpecular * specularMaterial) * specularFactor * att;
   //     diffuseComponent = lightDiffuse * diffuseMaterial * diffuseFactor * att;
   // }
    
   // //ambientComponent *= shadowMapTexture.Sample(testSampler, input.uv).x;
   // //diffuseComponent *= shadowMapTexture.Sample(testSampler, input.uv).x;
   // //specularComponent *= shadowMapTexture.Sample(testSampler, input.uv).x;
    
   // //Output
   // finalOutput = albedo * (ambientComponent + diffuseComponent) + specularComponent;
   // //output.lightPassOutput = finalOutput;
   // return finalOutput;
    
}