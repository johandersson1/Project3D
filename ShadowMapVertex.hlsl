cbuffer constantBuffer : register(b0)
{
    float4x4 WVP; 
    float4x4 worldSpace;
}
struct VertexShaderInput
{
    float3 position : POSITION;
 
};
struct VertexShaderOutput
{
    float4 position : SV_POSITION;
    float2 uv : UV;
    float3 normal : NORMAL;
};

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    output.position = mul(float4(input.position, 1), mul(worldSpace, WVP));
    
    return output;
}