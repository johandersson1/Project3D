struct GSOutput
{
    float4 pos : SV_POSITION;
	
};

float4 main(GSOutput input) : SV_TARGET
{
	return float4(0.0f, 0.5f, 0.0f, 1.0f);
}