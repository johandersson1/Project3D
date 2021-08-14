// Array of textures since we are also blending based on the height value of the displacement
Texture2D textures[2] : register(t0);
// The displacement texture that the blending is based upon
Texture2D blendTexture : register(t2);

sampler wrapSampler : register(s0);
sampler clampSampler : register(s1);

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
    float4 lightClipPos : SV_TARGET7;
};

cbuffer LightMatrix : register(b0)
{
    float4x4 lightMatrix;
}

PSOutput main(PSInput input)
{
    PSOutput output;
    
    output.worldPos = input.worldPos;
    output.normal = input.normal;
    output.position = input.position;
    output.ambientMTL = float4(0.5f, 0.5f, 0.5f, 1);
    output.diffuseMTL = float4(0.5f, 0.5f, 0.5f, 1);
    output.specularMTL = float4(0.2f, 0.2f, 0.2f, 1);
    //output.shadowMap = shadowTexture.Sample(mySampler, input.tex);
  
    // Sample the textures used for the terrain, [0] = base, [1] = blended texture
    float4 lowColour = textures[0].Sample(wrapSampler, input.tex);
    float4 hiColour = textures[1].Sample(wrapSampler, input.tex);
    float blendValue = blendTexture.Sample(clampSampler, input.tex).r;
        
    float highAmount = blendValue;
    float lowAmount = 1.0f - blendValue;
    
    output.lightClipPos = mul(float4(output.worldPos, 1.0f), lightMatrix);
   
    output.diffuse = highAmount * hiColour + lowAmount * lowColour;
    return output;
}