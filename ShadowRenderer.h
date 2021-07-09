#pragma once
#include "D3D11Help.h"
#include "D3D11Helper.h"
#include "ShaderData.h"
#include "Model.h"
#include <fstream>

class ShadowRenderer
{
private:
	const unsigned int stride = sizeof(XMFLOAT3);
	const unsigned int offset = 0;
	const std::string vs_path = "x64/Debug/ShadowMapVertex.cso";
	ID3D11VertexShader* vertexShader;
	ID3D11Buffer* matrixBuffer;

public:

	ShadowRenderer(ID3D11Device* device) :matrixBuffer(nullptr), vertexShader(nullptr)
	{



	}


};