Texture2D diffuseTex : register(t0);

SamplerState mySampler : register(s0);
SamplerState clampSampler : register(s1);

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD;
    float3 normal : NORMAL;
    float3 worldPos : WORLDPOS;
};

struct PixelOutput
{
    float4 position : SV_TARGET0;
    float3 normal : SV_TARGET1;
    float3 worldPos : SV_TARGET2;
    float4 diffuse : SV_TARGET3;
    float4 ambientMTL : SV_TARGET4;
    float4 diffuseMTL : SV_TARGET5;
    float4 specularMTL : SV_TARGET6;
    float4 lightClipPos : SV_TARGET7;
    
};

cbuffer mtlData : register(b0)
{
    float4 kA;
    float4 kD;
    float4 kS;
}

cbuffer LightMatrix : register(b1)
{
    float4x4 lightMatrix;
}

PixelOutput main(PixelInput input)
{
    PixelOutput output;
       
    output.position = input.position;
    output.normal = input.normal;
    output.worldPos = input.worldPos;
    output.diffuse = diffuseTex.Sample(mySampler, input.tex);
    output.ambientMTL = kA;
    output.diffuseMTL = kD;
    output.specularMTL = kS;
    
    output.lightClipPos = mul(float4(output.worldPos, 1.0f), lightMatrix);
    
    return output; 
}

