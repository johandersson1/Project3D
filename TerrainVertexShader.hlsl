struct VertexInput
{
    float3 position : POSITION;
    float2 tex : TEXCOORD;
    float3 normal : NORMAL;
};

struct VertexOutput
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD;
    float3 normal : NORMAL;
};

VertexOutput main(VertexInput input)
{
    VertexOutput output;
    // Pass the information on, no need to do anything at this stage
    output.position = float4(input.position, 1); // Waiting with the transformation until the domain shader
    output.tex = input.tex;
    output.normal = input.normal;

    return output;
}
