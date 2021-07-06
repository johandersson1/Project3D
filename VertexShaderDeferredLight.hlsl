struct VertexShaderInput
{
    float4 position : POSITION;
    float2 tex : TEXCOORD;
};

struct VertexShaderOutput
{
    float4 position : SV_POSITION; // Kamerans clipspace
    float2 tex : TEXCOORD;
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
VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    output.position = input.position;
    output.tex = input.tex;
	//output.normal = input.normal;
	//output.colour = input.colour;
    
    return output;
}