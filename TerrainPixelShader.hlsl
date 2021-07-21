Texture2D textures[2] : register(t0);
Texture2D blendTexture : register(t2);
sampler mySampler : register(s0);

struct PSInput
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD;
    float3 normal : NORMAL;
    float3 worldPos : WORLDPOS;
};

struct PSOutput
{
    float4 position : SV_TARGET0;
    float3 normal : SV_TARGET1;
    float3 worldPos : SV_TARGET2;
    float4 diffuse : SV_TARGET3;
    float4 ambientMTL : SV_TARGET4;
    float4 diffuseMTL : SV_TARGET5;
    float4 specularMTL : SV_TARGET6;
};

PSOutput main(PSInput input)
{
    PSOutput output;
    
    output.worldPos = input.worldPos;
    output.normal = input.normal;
    output.position = input.position;
    output.ambientMTL = float4(0.5f, 0.5f, 0.5f, 1);
    output.diffuseMTL = float4(0.5f, 0.5f, 0.5f, 1);
    output.specularMTL = float4(0.2f, 0.2f, 0.2f, 1);
    static const float TEX_LOW_BOUND = 0.4f;
    static const float TEX_HIGH_BOUND = 0.7f;

    float4 texColour;
    float4 lowColour = textures[0].Sample(mySampler, input.tex);
    float4 hiColour = textures[1].Sample(mySampler, input.tex);
    float blendValue = blendTexture.Sample(mySampler, input.tex).r;
        
    float highAmount = blendValue;
    float lowAmount = 1.0f - blendValue;
        
    texColour = saturate(texColour);
   
    output.diffuse = highAmount * hiColour + lowAmount * lowColour;
    return output;
}