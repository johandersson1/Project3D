Texture2D textures[2] : register(t0);

SamplerState mySampler : register(s0);


struct PixelInput
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD;
    float3 normal : NORMAL;
    float3 worldPos : WORLDPOS;
};

float4 main(PixelInput input) : SV_TARGET
{
    float4 color1;
    float4 color2;
    float4 blend;
    
    color1 = textures[0].Sample(mySampler, input.tex);
    color2 = textures[1].Sample(mySampler, input.tex);
    blend = color1 * color2 * 2.0f;
    blend = saturate(blend);
    
    return blend;
}