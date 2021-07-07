Texture2D testTexture : TEXTURE;
SamplerState testSampler : SAMPLER;

struct PixelShaderInput
{
    float4 position : SV_POSITION; //samma variabler f�r PixelShaderInput som f�r VertexShaderOutput, viktigt att det �r i samma ordning
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
    
    float3 ColourOfPixel = testTexture.Sample(testSampler, input.uv).xyz; //s�tter f�rg p� varje pixel
    
    //diffuse calculations

    float3 pixelToLight = normalize(light.lightCamPos - input.worldPos); //normaliserar vektorn mellan lightsource och worldpos f�r v�rt objekt
    float3 diffuse = light.lightColour * (max(0, dot(input.normal, pixelToLight))); // max tar in tv� stycken floats och ger oss max v�rdet, sedan j�mf�r 0  med * skal�r mellan input.norma och pixelToLight
  
    //specular calculations
    float3 normalSpecular = normalize(light.lightCamPos - input.worldPos);
    float3 reflection = reflect(-pixelToLight, input.normal); //reflect = r�knar ut reflectionen fr�n en vecktor baserat p� en normal, 
    float3 specular = pow(max(0, dot(reflection, pixelToLight)), lightStrength); //tar h�gsta v�rdet fr�n x,y och sedan kollar med skal�ren om v�rdet �r positivt,sen tilldelas specular det v�rdet
    
    //ambient calculations
    float3 ambientLight = light.lightColour * light.lightAmbient; //tilldelar f�rg till ambientLight genom att * colour med ambient 
   
   /* float3 finalColour = ColourOfPixel *//** (ambientLight + diffuse) + specular;*/ //tilldelar finalcolour genom att 
    return float4(ColourOfPixel, 1.0f); //en float4 d�r allt kombineras fr�n tidigare utr�kningar
}


  //anv�nds ej
  //float distance = length(pixelToLight); //utr�kning f�r attenuation
  //float Attenuation = 1.0f / dot(light.lightAttenuation, float3(1.0f, distance, distance*distance));
  //diffuse *= Attenuation;
  //specular *= Attenuation;