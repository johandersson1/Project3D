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

[maxvertexcount(4)]
void main(
	point float4 input[1] : SV_POSITION, 
	inout TriangleStream< GSOutput > output
)
{
    float3 up = float3(0.0f, 1.0f, 0.0f);
    float3 lookAt = cameraPosition - input[0].xyz;
    lookAt.y = 0.0f;
    lookAt = normalize(lookAt);
    float3 right = cross(up, lookAt);
    
    float4 corners[4];
    corners[0] = float4(input[0].xyz + 1.0f * right + 0.1f * up, 1.0f);
    corners[1] = float4(input[0].xyz - 1.0f * right + 0.1f * up, 1.0f);
    corners[2] = float4(input[0].xyz + 1.0f * right - 0.1f * up, 1.0f);
    corners[3] = float4(input[0].xyz - 1.0f * right - 0.1f * up, 1.0f);
	
    GSOutput gOut;
    [unroll]
	for (uint i = 0; i < 4; i++)
	{
        gOut.pos = mul(corners[i], viewPerspecive);
		output.Append(gOut);
	}
}