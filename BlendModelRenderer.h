#pragma once
#include "Model.h"
#include "D3D11Help.h"
#include "D3D11Helper.h"
#include "ShaderData.h"
#include <fstream>

class BlendModelRenderer
{
private:
	const unsigned int stride = sizeof(Vertex);
	const unsigned int offset = 0;
	std::string byteCode;
	//Deferred
	const std::string ps_path = "x64/Debug/BlendPixelShader.cso";
	ID3D11PixelShader* pixelShader;
	const std::string vs_path = "x64/Debug/BlendVertexShader.cso";
	ID3D11VertexShader* vertexShader;

	ID3D11Buffer* matricesBuffer;
	struct Matrices { XMFLOAT4X4 WVP; XMFLOAT4X4 worldSpace; }matrices;
public:

	BlendModelRenderer(ID3D11Device* device)
		:matrices(), pixelShader(nullptr), vertexShader(nullptr)
	{

	}

};
