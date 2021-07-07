Texture2D worldPosTexture : register(t0);
Texture2D posTexture : register(t1);
Texture2D normalTexture : register(t2);
Texture2D diffuseAlbedoTex : register(t3);

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
    
    float3 diffuseAlbedo = diffuseAlbedoTex.Sample(mySampler, input.tex).rgb;
    float3 pos = posTexture.Sample(mySampler, input.tex).rgb;
    float3 worldPos = worldPosTexture.Sample(mySampler, input.tex).rgb;
    float3 normalTex = normalTexture.Sample(mySampler, input.tex);
    float3 normal = normalTex.xyz;
    
    return float4(diffuseAlbedo, 1.0f);
    
    
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
  
}