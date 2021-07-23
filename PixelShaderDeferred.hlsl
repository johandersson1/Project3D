Texture2D diffuseTex : register(t0);

SamplerState mySampler : register(s0);

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
    
};

cbuffer mtlData : register(b0)
{
    float4 kA;
    float4 kD;
    float4 kS;
}


PixelOutput main(PixelInput input)
{
    PixelOutput output;
    
    //input.tex.x += uCord;
    //input.tex.y += vCord;
    
    output.position = input.position;
    output.normal = input.normal;
    output.worldPos = input.worldPos;
    output.diffuse = diffuseTex.Sample(mySampler, input.tex);
    output.ambientMTL = kA;
    output.diffuseMTL = kD;
    output.specularMTL = kS;
    
    return output; 
}

