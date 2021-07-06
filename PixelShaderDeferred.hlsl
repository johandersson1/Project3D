Texture2D gDiffuseTexture : register(t0);
Texture2D gNormalTexture : register(t1);

SamplerState mySampler : register(s0);

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD;
    float3 normal : NORMAL;
    float3 worldPos : WORLDPOS;
};

struct PixelOutputType
{
    float4 position : SV_TARGET0;
    float3 normal : SV_TARGET1;
    float3 worldPos : SV_TARGET2;
    float4 diffuse : SV_TARGET3;
};

struct PointLight
{
    float3 colour;
    float3 ambient;
    float3 diffuse;
    float3 camPos;
    //float range;
    //float att0;
    //float att1;
    //float att2;
};

cbuffer cbPerFrame //: register(b1)
{
    PointLight gPointLight;
};

PixelOutputType main(PixelInputType input)
{
    PixelOutputType output;
    
    output.position = input.position;
    output.normal = gNormalTexture.Sample(mySampler, input.tex).rgb;
    output.worldPos = input.worldPos;
    output.diffuse = gDiffuseTexture.Sample(mySampler, input.tex);
    
    return output;
    
    //float3 finalColour = { 0.0f, 0.0f, 0.0f };
    //input.normal = normalize(input.normal);

    //float lightIntensity = 200;
    //float3 pixelColour = gDiffuseTexture.Sample(mySampler, input.tex).xyz;
    
    ////Ambient
    //float3 ambientLight = gPointLight.colour * gPointLight.ambient;
    
    ////Diffuse
    //float3 pixelToLight = normalize(gPointLight.camPos - input.worldPos);
    //float3 diffuse = gPointLight.colour * (max(0, dot(input.normal, pixelToLight)));
    
    ////Distance from the lightsource to the surface
    ////float d = length(pixelToLight);

    ////Specular
    //float3 reflection = reflect(-pixelToLight, input.normal);
    //float3 specular = pow(max(0, dot(reflection, pixelToLight)), lightIntensity);
   
    ////float att = lightIntensity / gPointLight.att0 + (gPointLight.att1 * d) + (gPointLight.range * pow(d, 2));
      
    //finalColour = pixelColour * (ambientLight + diffuse) + specular /*/ att*/;
    //return float4(finalColour, 1.0f);

}

