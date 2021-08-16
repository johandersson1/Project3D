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
void main(point float4 input[1] : SV_POSITION, inout TriangleStream< GSOutput > output) 
{
	
    float3 up = float3(0.0f, 1.0f, 0.0f);									// Up-vector
    float3 lookAt = cameraPosition - input[0].xyz;						    // Vector from the point to camera
    lookAt.y = 0.0f;
    lookAt = normalize(lookAt);											    // Set y to 0 since we are using the Up-vector to determine which way is up
    float3 right = cross(up, lookAt);										// Get the right-vector used to determine where the vertices of the quad will be placed
    
	// The quad itself and the positions of the vertices of the quad
    float4 corners[4];
    corners[0] = float4(input[0].xyz + 0.3f * right + 0.3f * up, 1.0f);		// Top Left
    corners[1] = float4(input[0].xyz - 0.3f * right + 0.3f * up, 1.0f);		// Top Right
    corners[2] = float4(input[0].xyz + 0.3f * right - 0.3f * up, 1.0f);		// Bottom Left
    corners[3] = float4(input[0].xyz - 0.3f * right - 0.3f * up, 1.0f);		// Bottom Right
	
	// Set the position of the quad in the viewPerspective space and send off to the PS
	// Unroll can be used if you know that the loop will be the same each iteration and therefore optimize the program slightly ( i think )
    GSOutput gOut;
    [unroll]  
	for (uint i = 0; i < 4; i++)
	{
        gOut.pos = mul(corners[i], viewPerspecive);
		output.Append(gOut);
	}
}