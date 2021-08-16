
// Geometry Pass Vertex Shader

cbuffer constantBuffer : register(b1)
{
    float4x4 WVP;			//projectionMatrix
    float4x4 worldSpace;    // worldMatrix
}

struct VertexInput
{
	// The vertices of the model has a position, texcoord (UV), and a normal

    float3 position : POSITION; 
    float2 tex : TEXCOORD;
    float3 normal : NORMAL;
};

struct VertexOutput
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD;
    float3 normal : NORMAL;
    float3 worldPos : WORLDPOS;
};

VertexOutput main(VertexInput input)
{
	VertexOutput output;
    
    output.position = float4(input.position, 1);
    output.worldPos = mul(output.position, worldSpace);
    output.position = mul(output.position, WVP); // clipspace
    output.tex = input.tex;
    output.normal = mul(float4(input.normal, 0), worldSpace);
	output.normal = normalize(output.normal);

    return output;
}
