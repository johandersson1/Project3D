// Input control point
struct HS_INPUT
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD;
    float3 normal : NORMAL;
};

// Output control point
struct HS_OUTPUT
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD;
    float3 normal : NORMAL;
};

// Output patch constant data.
struct HS_CONSTANT_DATA_OUTPUT
{
	// Minumum amount possible

	float EdgeTessFactor[3]			: SV_TessFactor;				// e.g. would be [4] for a quad domain
	float InsideTessFactor			: SV_InsideTessFactor;			// e.g. would be Inside[2] for a quad domain

};

#define NUM_CONTROL_POINTS 3

// Patch Constant Function
HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants(
	InputPatch<HS_INPUT, NUM_CONTROL_POINTS> ip,
	uint PatchID : SV_PrimitiveID)
{
	HS_CONSTANT_DATA_OUTPUT Output;

	// Insert code to compute Output here
	Output.EdgeTessFactor[0] = 
		Output.EdgeTessFactor[1] = 
		Output.EdgeTessFactor[2] = 
		Output.InsideTessFactor = 10; // e.g. could calculate dynamic tessellation factors instead

	return Output;
}
// Patch Control point Function

[domain("tri")]									// triangle
[partitioning("fractional_odd")]				// Partitioning fractional_odd = smooth transition when	increasing or decreasing the tesselation amount, rounds to odds numbers
[outputtopology("triangle_cw")]					// clocwise triangles
[outputcontrolpoints(3)]						// three vertices
[patchconstantfunc("CalcHSPatchConstants")]		
HS_OUTPUT main( 
	InputPatch<HS_INPUT, NUM_CONTROL_POINTS> input,
	uint i : SV_OutputControlPointID,
	uint PatchID : SV_PrimitiveID )
{
    HS_OUTPUT output;
	
    output.position = input[i].position;
    output.tex = input[i].tex;
    output.normal = input[i].normal;
    
	return output;
}
