struct GSOutput
{
    float4 pos : SV_POSITION;
};

struct PixelOutput
{
	float4 position : SV_TARGET0;
	float4 normal : SV_TARGET1;
	float4 worldPos : SV_TARGET2;
	float4 diffuse : SV_TARGET3;
	float4 ambientMTL : SV_TARGET4;
	float4 diffuseMTL : SV_TARGET5;
	float4 specularMTL : SV_TARGET6;
};

PixelOutput main(GSOutput input) 
{
    PixelOutput output;
    
    output.position = input.pos;
    //output.diffuse = float4(68.0f / 255.0f, 85.0f / 255.0f, 90.0f / 255.0f, 1);
    output.diffuse = float4(0.5, 0.5, 0.5, 1);			// color of the particles
	
	// Material Data --- Set to -1 in diffuse so we dont du light calc on the particles
    output.ambientMTL = float4(0.0f, 0.0f, 0.0f, 0.0f);	
    output.diffuseMTL = float4(-1.0f, 0.0f, 0.0f, 0.0f);
    output.specularMTL = float4(0.0f, 0.0f, 0.0f, 0.0f);
	output.normal = float4(1, 1, 1, 1);
	output.worldPos = float4(1, 1, 1, 1);
    
	return output;
}