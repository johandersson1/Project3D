Texture2D albedoTex : register(t0);
Texture2D normalTex : register(t1);
Texture2D worldPosTex : register(t2);

SamplerState mySampler : register(s0);

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD;
    //float3 normal : NORMAL;
    //float3 worldPosition : WORLDPOSITION;
};

struct PixelOutput
{
    float4 lightOutput : SV_Target0;
};

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

PixelOutput main(PixelInput input) : SV_Target
{
    PixelOutput output;
    
    int3 sampleIndices = int3(input.position.xy, 0);
    
    float3 diffuseAlbedo = albedoTex.Sample(mySampler, input.tex).rgb;
    float3 worldPos = worldPosTex.Sample(mySampler, input.tex).rgb;
    float3 normaltex = normalTex.Sample(mySampler, input.tex).rgb;
 
        
    //float4 lighting = LightTexture.Load(sampleIndices);

    //float3 diffuse = lighting.xyz * diffuseAlbedo;
    //float3 specular = lighting.w * gPointLight.SpecularAlbedo;

    return float4(diffuseAlbedo + worldPos + normalTex, 1.0f);
    
    
  
}