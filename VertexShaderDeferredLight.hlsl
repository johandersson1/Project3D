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

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    output.position = input.position;
    output.tex = input.tex;
	//output.normal = input.normal;
	//output.colour = input.colour;
    
    return output;
}