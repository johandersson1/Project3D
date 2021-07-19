struct GSInput
{    
	float4 position : SV_POSITION;
    float2 tex : TEXCOORD;
    float3 normal : NORMAL;
    float3 worldPos : WORLDPOS;
};

struct GSOutput
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD;
    float3 normal : NORMAL;
    float3 worldPos : WORLDPOS;
};

[maxvertexcount(3)]
void main(
	triangle GSInput input[3] : SV_POSITION, 
	inout TriangleStream< GSOutput > output
)
{
    float3 p0 = input[0].position.xyz;
    float3 p1 = input[1].position.xyz;
    float3 p2 = input[2].position.xyz;

    //Calc normal from vertices
    float3 faceNormal = normalize(cross(p1 - p0, p2 - p0));

    //Get dot value, if value >= 0.0f the triangle is frontfaced. (p0 = (0,0,0) - (p0), a vector from pov to point).
    float dotVal = dot(faceNormal, -p0);

    if (dotVal >= 0.0f)
    {
        for (uint i = 0; i < 3; i++)
        {
            GSOutput vertex = input[i];
            vertex.normal = normalize(cross(input[1].worldPos - input[0].worldPos, input[2].worldPos - input[0].worldPos).xyz);
            output.Append(vertex);
        }
    }
      
}