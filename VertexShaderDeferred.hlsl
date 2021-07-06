cbuffer constantBuffer
{
    float4x4 WVP; //projectionMatrix
    //float4x4 WV; // viewMatrix
    float4x4 worldSpace; // worldMatrix
    //float3 lightPos;
}

//cbuffer CameraBuffer
//{
//    float3 cameraPosition;
//};

struct VertexInput
{
    float3 position : POSITION; //deferred
    float2 tex : TEXCOORD;
    float3 normal : NORMAL;
};

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD;
    float3 normal : NORMAL;
    float3 worldPos : WORLDPOS;
    //float3 lightPos : TEXCOORD1;
    //float3 viewDirection : TEXCOORD2;
};

PixelInput main(VertexInput input)
{
    PixelInput output;
    //float4 worldPosition;
    //input.position.w = 1.0f;
    
    output.position = float4(input.position, 1);
    output.position = mul(output.position, worldSpace);
    //output.position = mul(output.position, WV);
    output.position = mul(output.position, WVP);
    output.tex = input.tex;
    output.normal = mul(float4(input.normal, 0), worldSpace);
    output.normal = normalize(output.normal);
    output.worldPos = mul(float4(input.position, 1), worldSpace);
    //output.lightPos = lightPos.xyz - lightPos.xyz;
    //output.lightPos = normalize(output.lightPos);

    //output.viewDirection = cameraPosition.xyz - worldPosition.xyz;
    //output.viewDirection = normalize(output.viewDirection);

    return output;
}
