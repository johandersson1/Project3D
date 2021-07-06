Texture2D albedoTex : register(t0);
Texture2D normalTex : register(t1);

SamplerState mySampler : register(s0);

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD;
    float3 normal : NORMAL;
    float3 worldPos : WORLDPOS;
};

struct PixelOutputType
{
    float4 position : SV_TARGET0;
    float3 normal : SV_TARGET1;
    float3 worldPos : SV_TARGET2;
    float4 diffuse : SV_TARGET3;
};

struct PointLight
{
    float3 colour;
    float3 ambient;
    float3 diffuse;
    float3 camPos;
    //float range;
    //float att0;
    //float att1;
    //float att2;
};

cbuffer cbPerFrame //: register(b1)
{
    PointLight gPointLight;
};

PixelOutputType main(PixelInputType input)
{
    PixelOutputType output;
    
    output.position = input.position;
    output.normal = normalTex.Sample(mySampler, input.tex).rgb;
    output.worldPos = input.worldPos;
    output.diffuse = albedoTex.Sample(mySampler, input.tex);
    
    return output;
    
}

