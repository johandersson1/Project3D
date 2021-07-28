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
	const std::string ps_water_path = "x64/Debug/PixelShaderWater.cso";
	ID3D11PixelShader* pixelShaderWater;

	ID3D11Buffer* matricesBuffer;
	ID3D11Buffer* mtlBuffer;
	ID3D11Buffer* lightBuffer;
	struct Matrices { XMFLOAT4X4 WVP; XMFLOAT4X4 worldSpace; }matrices;
public: 
	
	ModelRenderer(ID3D11Device* device)
				  :matrices(), pixelShader(nullptr), vertexShader(nullptr)
	{
		CreateBuffer(device, matricesBuffer, sizeof(Matrices));	
		CreateBuffer(device, lightBuffer, sizeof(XMMATRIX));

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

		// WATER PIXEL SHADER
		reader.open(ps_water_path, std::ios::binary | std::ios::beg);

		if (!reader.is_open())
		{
			std::cout << "COULD NOT OPEN FILE: " + ps_water_path << std::endl;
			return;
		}

		reader.seekg(0, std::ios::end);
		shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
		reader.seekg(0, std::ios::beg);
		shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());

		hr = device->CreatePixelShader(shaderData.c_str(), shaderData.length(), nullptr, &pixelShaderWater);
		if FAILED(hr)
		{
			std::cout << "FAILED TO CREATE PIXEL SHADER" << std::endl;
			return;
		}

		shaderData.clear();
		reader.close();
	}

	void Render(ID3D11Device* device, ID3D11DeviceContext* context, Model* model, bool water = false)
	{
		XMStoreFloat4x4(&matrices.worldSpace, model->GetWorldMatrix());
		XMMATRIX WVP = XMMatrixTranspose(model->GetWorldMatrix() * ShaderData::viewMatrix * ShaderData::perspectiveMatrix);
		XMStoreFloat4x4(&matrices.WVP, WVP);

		UpdateBuffer(context, matricesBuffer, matrices);
		context->IASetInputLayout(ShaderData::model_layout);
		context->VSSetShader(vertexShader, NULL, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		if (water == true)
		{
			context->PSSetShader(pixelShaderWater, NULL, 0);

			D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};

			HRESULT hr = context->Map(*model->GetWaterBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
			if FAILED(hr)
			{
				std::cout << "FAILED TO UPDATE MTL BUFFER" << std::endl;
				return;
			}

			memcpy(mappedBuffer.pData, model->GetUVOffset(), sizeof(model->GetUVOffset()));
			context->Unmap(*model->GetWaterBuffer(), 0);
			context->PSSetConstantBuffers(2, 1, model->GetWaterBuffer());
		}

		else
		{
			context->PSSetShader(pixelShader, NULL, 0);
		}

		UpdateBuffer(context, lightBuffer, ShaderData::lightMatrix);
		context->PSSetConstantBuffers(1, 1, &lightBuffer);
		context->PSSetShaderResources(0, 1, model->GetTextures(1));
		context->PSSetShaderResources(1, 1, ShaderData::shadowmap->GetSRV());
	
		context->VSSetConstantBuffers(1, 1, &matricesBuffer);
	
		
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