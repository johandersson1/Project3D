cbuffer constantBuffer
{
    float4x4 worldViewProj;
    float4x4 worldSpace;
};

struct VertexShaderInput
{
    float3 position : POSITION;
    float2 uv : UV;
    float3 normal : NORMAL;
    float3 colour : COLOUR;
};

struct VertexShaderOutput
{
    float4 position : SV_POSITION; //matchar PixelShaderInput 
    float2 uv : UV;
    float3 normal : NORMAL;
    float3 colour : COLOUR;
    float4 worldPos : WORLDPOS;
};

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;

    output.position = mul(float4(input.position, 1.0f), worldViewProj);
    output.uv = input.uv;
    output.normal = mul(float4(input.normal, 0.0f), worldSpace);
    output.worldPos = mul(float4(input.position, 1.0f), worldSpace);
    output.colour = input.colour.xyz;
    return output;
}