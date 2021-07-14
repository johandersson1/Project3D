struct GSINPUT
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD;
    float3 normal : NORMAL;
    float3 worldPos : WORLDPOS;
};

struct GSOOUTPUT
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD;
    float3 normal : NORMAL;
    float3 worldPos : WORLDPOS;
};

cbuffer CAMERAPOS : register(b0)
{
    float4 cameraPos;
}

[maxvertexcount(3)]
void main(triangle GSINPUT input[3] : SV_POSITION, inout TriangleStream<GSOOUTPUT> output)
{
    float3 camVector = cameraPos.xyz - input[2].worldPos.xyz;
   
    float3 v1 = input[2].worldPos - input[0].worldPos;
    float3 v2 = input[1].worldPos - input[0].worldPos;
    float3 v3 = cross(v1, v2);

    
    if (dot(camVector, v3) >= 0)
    {
        GSOOUTPUT element;
        for (int i = 0; i < 3; i++)
        {
            
            element.position = input[i].position;
            element.tex = input[i].tex;
            element.normal = input[i].normal;
            element.normal = v3;
            element.worldPos = input[i].worldPos;
            output.Append(element);
        }
    }
}