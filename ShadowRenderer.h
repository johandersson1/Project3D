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

		CreateBuffer(device, matrixBuffer, sizeof(XMFLOAT4X4));

		std::string shaderData;
		std::ifstream reader;
		// VERTEX SHADER
		reader.open(vs_path, std::ios::binary | std::ios::ate);
		if (!reader.is_open())
		{
			std::cerr << "Could not open VS file!" << std::endl;
			return;
		}

		reader.seekg(0, std::ios::end);
		shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
		reader.seekg(0, std::ios::beg);

		shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());

		if (FAILED(device->CreateVertexShader(shaderData.c_str(), shaderData.length(), nullptr, &vertexShader)))
		{
			std::cerr << "Failed to create pixel shader!" << std::endl;
			return;
		}

		shaderData.clear();
		reader.close();

	}
	void Render(ID3D11DeviceContext* context, Model* model)
	{
		context->IASetInputLayout(ShaderData::positionOnly_layout);
		context->VSSetShader(vertexShader, NULL, 0);
		context->PSSetShader(NULL, NULL, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		XMFLOAT4X4 WVP1;
		XMMATRIX WVP = XMMatrixTranspose(model->GetWorldMatrix());
		XMStoreFloat4x4(&WVP1, WVP);
		UpdateBuffer(context, matrixBuffer, WVP1);

		context->IASetVertexBuffers(0, 1, model->GetPositionsBuffer(), &stride, &offset);
		context->Draw(model->GetVertexCount(), 0);
	}
};