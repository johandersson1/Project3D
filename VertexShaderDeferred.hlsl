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

cbuffer worldViewProjectionMatrixBuffer : register(b0)
{
    matrix viewProjectionMatrix;
};

cbuffer viewProjectionMatrixBuffer : register(b1)
{
    matrix worldMatrix;
};

PixelInput main(VertexInput input)
{
    PixelInput output;
    
    output.position = float4(input.position, 1);
    output.position = mul(output.position, worldMatrix);
    //output.position = mul(output.position, WV);
    output.position = mul(output.position, viewProjectionMatrix);
    output.tex = input.tex;
    output.normal = mul(float4(input.normal, 0), worldMatrix);
    output.normal = normalize(output.normal);
    output.worldPos = mul(float4(input.position, 1), worldMatrix);

    return output;
}
