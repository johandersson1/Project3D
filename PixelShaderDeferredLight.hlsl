Texture2D worldPosTexture : register(t0);
Texture2D posTexture : register(t1);
Texture2D normalTexture : register(t2);
Texture2D diffuseAlbedoTex : register(t3);

SamplerState mySampler : register(s0);

//struct PixelOutput
//{
//    float4 lightOutput : SV_Target0;
//};

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
    
    
  
}