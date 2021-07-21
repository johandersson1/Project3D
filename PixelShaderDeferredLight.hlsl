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
    float4 lightAmbient;
    float4 lightDiffuse;
    float4 lightSpecular;
    float4 lightPos;
    float4 camPos;
    float4 lightDirection;
    
    float lightRange;
    float lightStrength;
    float att0;
    float att1;
    float att2;
};

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD;
    //float3 normal : NORMAL;
    //float3 worldPosition : WORLDPOSITION;
};

float4 main(PixelInput input) : SV_Target
{
    //int3 sampleIndices = int3(input.position.xy, 0);
    
    //float3 diffuseAlbedo = diffuseAlbedoTexture.Sample(mySampler, input.tex).rgb;
    //float3 pos = posTexture.Sample(mySampler, input.tex).rgb;
    //float3 worldPos = worldPosTexture.Sample(mySampler, input.tex).rgb;
    //float3 normalTex = normalTexture.Sample(mySampler, input.tex);
    //float3 normal = normalTex.xyz;
    
    //return float4(diffuseAlbedo, 1.0f);
    
//// Sample the diffuse map 
//    float3 diffuseAlbedo = DiffuseMap.Sample(AnisoSampler, input.TexCoord).rgb;
//// Determine our indices for sampling the texture based on the current 
//// screen position 
//    int3 sampleIndices = int3(input.ScreenPos.xy, 0);
//// Sample the light target 
//    float4 lighting = LightTexture.Load(samplelndices);
//// Apply the diffuse and specular albedo to the lighting value 
//    float3 diffuse = lighting.xyz * diffuseAlbedo;
//    float3 specular = lighting.w * SpecularAlbedo;
//// Final output is the sum of diffuse + specular 
//    return float4(diffuse + specular, 1.0f);
    
    PixelInput output;
    
    float4 normal = normalTexture.Sample(mySampler, input.tex);
    float4 albedo = diffuseAlbedoTexture.Sample(mySampler, input.tex);
    float4 worldPos = worldPosTexture.Sample(mySampler, input.tex);
    //float4 shadowPosH = shadowMapTexture.Sample(mySampler, input.tex);
    
    
    normal = normalize(normal);
    float4 finalOutput = float4(1, 1, 1, 1);
    float3 lightVector = normalize(float4(lightPos.xyz, 1) - worldPos);
    lightVector = normalize(-1 * lightDirection.xyz);
    float lightDistance = length(lightVector);
    
    //Material properties
    float4 ambientMaterial = ambientMatTexture.Sample(mySampler, input.tex);
    float4 diffuseMaterial = diffuseMatTexture.Sample(mySampler, input.tex);
    float4 specularMaterial = specularMatTexture.Sample(mySampler, input.tex);
    
    float4 ambientComponent = lightAmbient * ambientMaterial;
    float diffuseFactor = dot(float4(lightVector, 0), normal);
    float4 diffuseComponent = float4(0, 0, 0, 1);
    
    float ShinynessFactor = 64;
    float specularFactor = 0;
    float att = 0;
    float4 specularComponent = float4(0, 0, 0, 1);
    
    if (diffuseFactor > 0)
    {
        float3 v = reflect(float4(-lightVector, 0), normal);
        float3 toEye = normalize(float4(camPos.xyz, 1) - worldPos);
        specularFactor = pow(max(dot(toEye, v), 0), ShinynessFactor);
        att = lightStrength / (att0 + att1 * lightDistance + att2 * pow(lightDistance, 2));
        specularComponent = (lightSpecular * specularMaterial) * specularFactor * att;
        diffuseComponent = lightDiffuse * diffuseMaterial * diffuseFactor * att;
    }
    
    //ambientComponent *= shadowMapTexture.Sample(testSampler, input.uv).x;
    
    //Output
    finalOutput = albedo * (ambientComponent + diffuseComponent) + specularComponent;
    //finalOutput = albedo; /** normal * worldPos;*/
    return finalOutput;
  
}