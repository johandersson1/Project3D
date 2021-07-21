struct GSOutput
{
    float4 pos : SV_POSITION;
	
};

struct PixelOutput
{
    float4 position : SV_TARGET0;
    float4 diffuse : SV_TARGET3;
    float4 ambientMTL : SV_TARGET4;
    float4 diffuseMTL : SV_TARGET5;
    float4 specularMTL : SV_TARGET6;
};

PixelOutput main(GSOutput input) 
{
    PixelOutput output;
    
    output.position = input.pos;
    output.diffuse = float4(68.0f/255.0f, 85.0f/255.0f, 90.0f/255.0f, 1);
    output.ambientMTL = float4(0.0f, 0.0f, 0.0f, 0.0f);
    output.diffuseMTL = float4(-1.0f, 0.0f, 0.0f, 0.0f);
    output.specularMTL = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
	return output;
}