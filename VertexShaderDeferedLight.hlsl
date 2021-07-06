struct VertexShaderInput
{
    float3 position : POSITION;
    float2 uv : UV;
    float3 normal : NORMAL;
};

struct VertexShaderOutput
{
    float4 position : SV_POSITION; // Kamerans clipspace
    float2 uv : UV;
    float3 normal : NORMAL;
    float3 worldPosition : WORLDPOSITION;
};

cbuffer worldViewProjectionMatrixBuffer : register(b0)
{
    matrix viewProjectionMatrix;
};

cbuffer viewProjectionMatrixBuffer : register(b1)
{
    matrix worldMatrix;
};

cbuffer lightProjection : register(b2)
{
    matrix lightProjectionMatrix;
};
