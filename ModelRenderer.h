#pragma once
#include "Model.h"
#include "D3D11Help.h"
#include "D3D11Helper.h"
#include "ShaderData.h"
#include <fstream>

class ModelRenderer
{
private:
	const unsigned int stride = sizeof(Vertex);
	const unsigned int offset = 0;
	std::string byteCode;
	//Deferred
	const std::string ps_path = "x64/Debug/PixelShaderDeferred.cso";
	ID3D11PixelShader* pixelShader;
	const std::string vs_path = "x64/Debug/VertexShaderDeferred.cso";
	ID3D11VertexShader* vertexShader;

	ID3D11Buffer* matricesBuffer;
	ID3D11Buffer* mtlBuffer;
	struct Matrices { XMFLOAT4X4 WVP; XMFLOAT4X4 worldSpace; }matrices;
public: 
	
	ModelRenderer(ID3D11Device* device)
				  :matrices(), pixelShader(nullptr), vertexShader(nullptr)
	{
		CreateBuffer(device, matricesBuffer, sizeof(Matrices));	

		std::string shaderData;
		std::ifstream reader;
		std::string shaderByteCode;
		// PIXEL SHADER
		reader.open(ps_path, std::ios::binary | std::ios::beg);

		if (!reader.is_open())
		{
			std::cout << "COULD NOT OPEN FILE: " + ps_path << std::endl;
			return;
		}

		reader.seekg(0, std::ios::end);
		shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
		reader.seekg(0, std::ios::beg);
		shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());

		HRESULT hr = device->CreatePixelShader(shaderData.c_str(), shaderData.length(), nullptr, &pixelShader);
		if FAILED(hr)
		{
			std::cout << "FAILED TO CREATE PIXEL SHADER" << std::endl;
			return;
		}

		shaderData.clear();
		reader.close();

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

		byteCode = shaderData;

		shaderData.clear();
		reader.close();
	
	}

	void Render(ID3D11DeviceContext* context, Model* model)
	{
		context->IASetInputLayout(ShaderData::model_layout);
		context->VSSetShader(vertexShader, NULL, 0);
		context->PSSetShader(pixelShader, NULL, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		XMStoreFloat4x4(&matrices.worldSpace, model->GetWorldMatrix());
		XMMATRIX WVP = XMMatrixTranspose(model->GetWorldMatrix() * ShaderData::viewMatrix * ShaderData::perspectiveMatrix);
		XMStoreFloat4x4(&matrices.WVP, WVP);
		UpdateBuffer(context, matricesBuffer, matrices);
		context->VSSetConstantBuffers(1, 1, &matricesBuffer);
		
		context->PSSetShaderResources(0, 1, model->GetTextures(1));
		
		D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};

		HRESULT hr = context->Map(*model->GetMTLBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
		if FAILED(hr)
		{
			std::cout << "FAILED TO UPDATE MTL BUFFER" << std::endl;
			return;
		}
		memcpy(mappedBuffer.pData, &model->GetMaterial(), sizeof(model->GetMaterial()));
		context->Unmap(*model->GetMTLBuffer(), 0);

		context->PSSetConstantBuffers(0, 1, model->GetMTLBuffer());

		context->IASetVertexBuffers(0, 1, model->GetBuffer(), &stride, &offset);
		context->Draw(model->GetVertexCount(), 0);
		
	}
	std::string GetByteCode() const { return this->byteCode; }
};