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
};


PixelOutput main(PixelInput input)
{
    PixelOutput output;
    
    output.position = input.position;
    output.normal = input.normal;
    output.worldPos = input.worldPos;
    output.diffuse = diffuseTex.Sample(mySampler, input.tex);
    
    return output;
    
}

