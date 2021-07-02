Texture2D myTexture : TEXTURE : register(t0);
SamplerState mySampler : SAMPLER : register(s0);

struct PixelShaderInput
{
	float4 position : SV_POSITION;
	float2 uv : UV;
	float3 normal : NORMAL;
    float3 colour : COLOUR;
    float3 worldPos : WORLDPOS;
};


float4 main(PixelShaderInput input) : SV_TARGET
{
    float3 finalColour = { 0.0f, 0.0f, 0.0f };
    input.normal = normalize(input.normal);

    float3 pixelColour = myTexture.Sample(mySampler, input.uv).xyz;
    
    return float4(pixelColour, 1.0f);
    
}