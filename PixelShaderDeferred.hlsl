Texture2D diffuseTex : register(t0);
Texture2D shadowTex : register(t1);

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
    float4 shadowMap : SV_TARGET7;
    
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
    
    float4 lightClip = mul(float4(output.worldPos, 1.0f), lightMatrix);
    
    //SHADOWS
    lightClip.xyz /= lightClip.w;
    float2 tx = float2(0.5f * lightClip.x + 0.5f, -0.5f * lightClip.y + 0.5);
    float sm = shadowTex.Sample(mySampler, tx).r;

    float shadow = (sm + 0.005f < lightClip.z) ? sm : 1.0f;
    
    output.shadowMap = float4(shadow, 0, 0, 1);
    
    return output; 
}

