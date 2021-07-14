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
   
    float3 p0 = input[0].position.xyz;
    float3 p1 = input[1].position.xyz;
    float3 p2 = input[2].position.xyz;

    float3 faceNormal = normalize(cross(p1 - p0, p2 - p0));
    float dotVal = dot(faceNormal, -p0);

    if (dotVal >= 0.0f)
    {
        for (uint i = 0; i < 3; i++)
        {
            GSOOUTPUT vertex = input[i];
            output.Append(vertex);
        }
    }
 
    //float3 camVector = cameraPos.xyz - input[2].worldPos.xyz;
   
    //float3 v1 = input[0].worldPos - input[2].worldPos;
    //float3 v2 = input[0].worldPos - input[1].worldPos;
    //float3 v3 = cross(v2, v1);
    
    //if (dot(camVector, v3) <= 0)
    //{
    //    GSOOUTPUT element;
    //    for (int i = 0; i < 3; i++)
    //    {
            
    //        element.position = input[i].position;
    //        element.tex = input[i].tex;
    //        element.normal = input[i].normal;
    //        element.normal = v3;
    //        element.worldPos = input[i].worldPos;
    //        output.Append(element);
    //    }
    //}
    
}