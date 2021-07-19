Texture2D textures[2];
sampler mySampler : register(s0);

struct PSInput
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD;
    float3 normal : NORMAL;
    float3 worldPos : WORLDPOS;
    float blendValue : BLENDVALUE;
};

struct PSOutput
{
    float4 position : SV_TARGET0;
    float3 normal : SV_TARGET1;
    float3 worldPos : SV_TARGET2;
    float4 diffuse : SV_TARGET3;
};

PSOutput main(PSInput input)
{
    PSOutput output;
    
    output.worldPos = input.worldPos;
    output.normal = input.normal;
    output.position = input.position;
    
    static const float TEX_LOW_BOUND = 0.4f;
    static const float TEX_HIGH_BOUND = 0.7f;
    
    float4 texColour;
    float4 lowColour = textures[0].Sample(mySampler, input.tex);
    //float4 midColour = middleTex.Sample(SWrap, pin.Tex);
    float4 hiColour = textures[1].Sample(mySampler, input.tex);
    
    if (input.blendValue < TEX_LOW_BOUND)
    {
        texColour = lowColour;
    }
    else if (input.blendValue > TEX_LOW_BOUND)
    {
        texColour = hiColour;
    }
    else
    {
        texColour = lerp(lowColour, hiColour, (input.blendValue - TEX_LOW_BOUND) * (1.0f / (TEX_HIGH_BOUND - TEX_LOW_BOUND)));
    }
    
    //float4 lower = textures[0].Sample(mySampler, input.tex);
    output.diffuse = texColour;
	return output;
}