Texture2D displacementTexture : register(t0);

SamplerState mySampler : register(s0);
SamplerState clampSampler : register(s1);

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
    float blendValue : BLENDVALUE;
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
	float EdgeTessFactor[3]			: SV_TessFactor;			// e.g. would be [4] for a quad domain
	float InsideTessFactor			: SV_InsideTessFactor;		// e.g. would be Inside[2] for a quad domain

};

#define NUM_CONTROL_POINTS 3

[domain("tri")]
DS_OUTPUT main(
	HS_CONSTANT_DATA_OUTPUT input, float3 UVW : SV_DomainLocation, const OutputPatch<DS_INPUT, NUM_CONTROL_POINTS> patch)
{
	// The domain shader creates new primitives based on the weights it recieves from the tesselator stage
	// since we use triangles, the data per patch is UVW barycentric coordinates and create vertex positions

	// Set the output based on the new data for the patch based on the weights
    DS_OUTPUT output;
	output.position = patch[0].position * UVW.x + patch[1].position * UVW.y + patch[2].position * UVW.z;
    output.tex = patch[0].tex * UVW.x + patch[1].tex * UVW.y + patch[2].tex * UVW.z;
    output.normal = patch[0].normal * UVW.x + patch[1].normal * UVW.y + patch[2].normal * UVW.z;
    
    output.normal = mul(float4(output.normal, 0), worldSpace);
    output.normal = normalize(output.normal);
    output.worldPos = mul(output.position, worldSpace);
    
    //Displacement done here
    const float disfactor = 2.0f;													// amount of displacement
    float h = displacementTexture.SampleLevel(clampSampler, output.tex, 0).r;		// sample the displacement map, using one channel since the texture is 0->1 (grayscale)
    output.blendValue = h;															// set the blendvalue to the displacementmap ( used for the texture blending in the pixelshader)
    output.worldPos.xyz += h * disfactor * output.normal;							// displace the vertices based on the displacementmap, the amount and in the direction of their respective normal
   
	// no need to do this here, could do it in the geometry shader aswell
    output.position = mul(float4(output.worldPos, 1), viewPerspective);
   
    return output;
}
