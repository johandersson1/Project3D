cbuffer CAMERA : register(b1)
{
    float3 cameraPosition;
}

cbuffer MATRIX : register(b2)
{
    float4x4 viewPerspecive;
}

struct GSOutput
{
	float4 pos : SV_POSITION;
};

[maxvertexcount(4)] // Four total vertices for each particle
void main(
	point float4 input[1] : SV_POSITION
	inout TriangleStream< GSOutput > output
) // point input[i] =  one single point used per particle
{
    float3 up = float3(0.0f, 1.0f, 0.0f); // Up-vector
    float3 lookAt = cameraPosition - input[0].xyz; // Vector from the point to camera
    lookAt.y = 0.0f;
    lookAt = normalize(lookAt); // Set y to 0 since we are using the Up-vector to determine which way is up
    float3 right = cross(up, lookAt); // Get the right-vector used to determine where the vertices of the quad will be placed
    
	// The quad itself and the positions of the vertices of the quad
    float4 corners[4];
    corners[0] = float4(input[0].xyz + 0.1f * right + 0.1f * up, 1.0f);
    corners[1] = float4(input[0].xyz - 0.1f * right + 0.1f * up, 1.0f);
    corners[2] = float4(input[0].xyz + 0.1f * right - 0.1f * up, 1.0f);
    corners[3] = float4(input[0].xyz - 0.1f * right - 0.1f * up, 1.0f);
	
	// Set the position of the quad in the viewPerspective space and send off to the PS
    GSOutput gOut;
    [unroll] // Unroll can be used if you know that the loop will be the same each iteration and therefore optimize the program slightly ( i think ) 
	for (uint i = 0; i < 4; i++)
	{
        gOut.pos = mul(corners[i], viewPerspecive);
		output.Append(gOut);
	}
}