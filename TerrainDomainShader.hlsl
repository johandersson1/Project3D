Texture2D displacementTexture : register(t0);

SamplerState mySampler : register(s0);

cbuffer matrices : register(b0)
{
    float4x4 viewPerspective;
    float4x4 worldSpace;
}
struct DS_OUTPUT
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD;
    float3 normal : NORMAL;
    float3 worldPos : WORLDPOS;
};

// Output control point
struct DS_INPUT
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD;
    float3 normal : NORMAL;
};

// Output patch constant data.
struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[3]			: SV_TessFactor; // e.g. would be [4] for a quad domain
	float InsideTessFactor			: SV_InsideTessFactor; // e.g. would be Inside[2] for a quad domain
	// TODO: change/add other stuff
};

#define NUM_CONTROL_POINTS 3

[domain("tri")]
DS_OUTPUT main(
	HS_CONSTANT_DATA_OUTPUT input,
	float3 domain : SV_DomainLocation,
	const OutputPatch<DS_INPUT, NUM_CONTROL_POINTS> patch)
{
    
    DS_OUTPUT output;
	output.position = patch[0].position * domain.x + patch[1].position * domain.y + patch[2].position * domain.z;
    output.tex = patch[0].tex * domain.x + patch[1].tex * domain.y + patch[2].tex * domain.z;
    output.normal = patch[0].normal * domain.x + patch[1].normal * domain.y + patch[2].normal * domain.z;
    
    output.normal = mul(float4(output.normal, 0), worldSpace);
    output.normal = normalize(output.normal);
    output.worldPos = mul(output.position, worldSpace);
    
    //Displacement
    const float disfactor = 0.5f;
    float h = displacementTexture.SampleLevel(mySampler, output.tex, 0).r;
    output.worldPos.xyz += h * disfactor * output.normal;
   
    output.position = mul(float4(output.worldPos, 1), viewPerspective);
   
    return output;
}
