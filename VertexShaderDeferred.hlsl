cbuffer constantBuffer : register(b1)
{
    float4x4 WVP; //projectionMatrix
    float4x4 worldSpace; // worldMatrix
}

struct VertexInput
{
    float3 position : POSITION; //deferred
    float2 tex : TEXCOORD;
    float3 normal : NORMAL;
};

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD;
    float3 normal : NORMAL;
    float3 worldPos : WORLDPOS;

};

PixelInput main(VertexInput input)
{
    PixelInput output;
    
    output.position = float4(input.position, 1);
    output.worldPos = mul(output.position, worldSpace);
    output.position = mul(output.position, WVP);
    output.tex = input.tex;
    output.normal = mul(float4(input.normal, 0), worldSpace);
    output.normal = normalize(output.normal);


    return output;
}
