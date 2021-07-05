Texture2D testTexture : TEXTURE;
SamplerState testSampler : SAMPLER;

struct PixelShaderInput
{
    float4 position : SV_POSITION; //samma variabler för PixelShaderInput som för VertexShaderOutput, viktigt att det är i samma ordning
    float2 uv : UV;
    float3 normal : NORMAL;
    float3 colour : COLOUR;
    float3 worldPos : WORLDPOS; 
};

struct Light
{
    float3 lightColour;
    float3 lightAmbient;
    float3 lightDiffuse;
    float3 lightCamPos;
    float3 lightAttenuation;
};

cbuffer constantBufferLight
{
    Light light;
};

float4 main(PixelShaderInput input) : SV_TARGET
{  
    input.normal = normalize(input.normal); //normalise normalen

    float lightStrength = 200; //ljuset i scenen
    
    float3 ColourOfPixel = testTexture.Sample(testSampler, input.uv).xyz; //sätter färg på varje pixel
    
    //diffuse calculations

    float3 pixelToLight = normalize(light.lightCamPos - input.worldPos); //normaliserar vektorn mellan lightsource och worldpos för vårt objekt
    float3 diffuse = light.lightColour * (max(0, dot(input.normal, pixelToLight))); // max tar in två stycken floats och ger oss max värdet, sedan jämför 0  med * skalär mellan input.norma och pixelToLight
  
    //specular calculations
    float3 normalSpecular = normalize(light.lightCamPos - input.worldPos);
    float3 reflection = reflect(-pixelToLight, input.normal); //reflect = räknar ut reflectionen från en vecktor baserat på en normal, 
    float3 specular = pow(max(0, dot(reflection, pixelToLight)), lightStrength); //tar högsta värdet från x,y och sedan kollar med skalären om värdet är positivt,sen tilldelas specular det värdet
    
    //ambient calculations
    float3 ambientLight = light.lightColour * light.lightAmbient; //tilldelar färg till ambientLight genom att * colour med ambient 
   
   /* float3 finalColour = ColourOfPixel *//** (ambientLight + diffuse) + specular;*/ //tilldelar finalcolour genom att 
    return float4(ColourOfPixel, 1.0f); //en float4 där allt kombineras från tidigare uträkningar
}


  //används ej
  //float distance = length(pixelToLight); //uträkning för attenuation
  //float Attenuation = 1.0f / dot(light.lightAttenuation, float3(1.0f, distance, distance*distance));
  //diffuse *= Attenuation;
  //specular *= Attenuation;