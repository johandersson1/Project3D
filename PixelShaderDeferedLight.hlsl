Texture2D albedoTex : register(t0);
Texture2D normalTex : register(t1);
Texture2D worldPosTex : register(t2);

SamplerState mySampler : register(s0);

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 uv : UV;
    float3 normal : NORMAL;
    float3 worldPosition : WORLDPOSITION;
};

struct PixelShaderOutput
{
    float4 lightOutput : SV_Target0;
};

cbuffer DirectionalLight : register(b0)
{
    float4 lightAmbient;
    float4 lightDiffuse;
    float4 lightSpecular;
    float4 lightPos;
    float4 camPos;
    float4 lightDirection;
    
    float lightRange;
    float lightStrength;
    float att0;
    float att1;
    float att2;
};

PixelShaderOutput main(PixelInputType input) : SV_Target
{
    PixelShaderOutput output;
    
    float4 normal = normalTex.Sample(mySampler, input.uv);
    float4 albedo = albedoTex.Sample(mySampler, input.uv);
    float4 worldPos = worldPosTex.Sample(mySampler, input.uv);
    //float4 shadowPosH = shadowMapTexture.Sample(testSampler, input.uv);
    
    normal = normalize(normal);
    float4 finalOutput = float4(1, 1, 1, 1);
    float3 lightVector = normalize(float4(lightPos.xyz, 1) - worldPos);
    lightVector = normalize(-1 * lightDirection.xyz);
    float lightDistance = length(lightVector);
    
    ////Material properties
    //float4 ambientMaterial = ambientMatlTexture.Sample(testSampler, input.uv);
    //float4 diffuseMaterial = diffuseMtlTexture.Sample(testSampler, input.uv);
    //float4 specularMaterial = specularMtlTexture.Sample(testSampler, input.uv);
    
    //float4 ambientComponent = lightAmbient * ambientMaterial;
    float diffuseFactor = dot(float4(lightVector, 0), normal);
    float4 diffuseComponent = float4(0, 0, 0, 1);
    
    float ShinynessFactor = 64;
    float specularFactor = 0;
    float att = 0;
    float4 specularComponent = float4(0, 0, 0, 1);
    
    if (diffuseFactor > 0)
    {
        float3 v = reflect(float4(-lightVector, 0), normal);
        float3 toEye = normalize(float4(camPos.xyz, 1) - worldPos);
        specularFactor = pow(max(dot(toEye, v), 0), ShinynessFactor);
        att = lightStrength / (att0 + att1 * lightDistance + att2 * pow(lightDistance, 2));
        specularComponent = (lightSpecular /** specularMaterial*/) * specularFactor * att;
        diffuseComponent = lightDiffuse /** diffuseMaterial*/ * diffuseFactor * att;
    }
    //ambientComponent *= shadowMapTexture.Sample(testSampler, input.uv).x;
    //diffuseComponent *= shadowMapTexture.Sample(testSampler, input.uv).x;
    //specularComponent *= shadowMapTexture.Sample(testSampler, input.uv).x;
    
    //Output
    finalOutput = albedo * (diffuseComponent) + specularComponent;
    output.lightOutput = finalOutput;
    return output;
    
    
    
}