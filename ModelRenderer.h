#pragma once
#include "Model.h"
#include "D3D11Helper.h"
#include "ShaderData.h"
#include <fstream>

class ModelRenderer
{
private:
	// Struct in Geometry
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
	ID3D11Buffer* lightBuffer;

	struct Matrices 
	{ 
		XMFLOAT4X4 WVP; 
		XMFLOAT4X4 worldSpace; 
	}matrices;

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
	
	void Render(ID3D11Device* device, ID3D11DeviceContext* context, Model* model, bool water, bool rotation)
	{
		// Get the WorldMatrix, store and update the buffer for the model ( Transposed WorldMatrix HLSL is columnmayor )
		XMStoreFloat4x4(&matrices.worldSpace,XMMatrixTranspose( model->GetWorldMatrix()));
		XMMATRIX WVP = XMMatrixTranspose(model->GetWorldMatrix() * ShaderData::viewMatrix * ShaderData::perspectiveMatrix); // Local to clipspace (camera)
		XMStoreFloat4x4(&matrices.WVP, WVP);
		UpdateBuffer(context, matricesBuffer, matrices);

		// Set the inputlayout, VS and topology
		context->IASetInputLayout(ShaderData::model_layout);
		context->VSSetShader(vertexShader, NULL, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); 
		
		// Water specific stuff
		if (water == true)
		{
			// Water uses its own PS because of the UV animation
			context->PSSetShader(pixelShaderWater, NULL, 0);

			D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};

			HRESULT hr = context->Map(*model->GetWaterBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
			if FAILED(hr)
			{
				std::cout << "FAILED TO UPDATE WATER BUFFER" << std::endl;
				return;
			}

			memcpy(mappedBuffer.pData, model->GetUVOffset(), sizeof(model->GetUVOffset()));
			context->Unmap(*model->GetWaterBuffer(), 0);

			// The CB that contains the information that is used to move the UVs
			context->PSSetConstantBuffers(2, 1, model->GetWaterBuffer());
		}
		// If the model isnt water, use the "regular" PS for each model
		else
		{
			context->PSSetShader(pixelShader, NULL, 0);
		}
		
		// Update the lightbuffer for each model, used for the shadows (sent to the clipspace target in the PS)
		UpdateBuffer(context, lightBuffer, ShaderData::lightMatrix);
		context->PSSetConstantBuffers(1, 1, &lightBuffer); // set the CB containing light info
		model->BindTextures(context);	// Set the shader resource with the specific texture used for the modelm
		context->VSSetConstantBuffers(1, 1, &matricesBuffer); // set the CB for the vs with the WVP and worldspace matrices
		// DS and HS not used for regular models
		context->HSSetShader(NULL, NULL, 0);
		context->DSSetShader(NULL, NULL, 0);
		// GS already set outside the renderer

		// Get, update and set the MTL data for each model
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
		context->IASetVertexBuffers(0, 1, model->GetBuffer(), &stride, &offset);				// Sends vertex buffer, per model, to the input assembly 
																								// ( first stage of the graphics pipeline ) that tells the vertex 
																								// shader how to read the data
		context->Draw(model->GetVertexCount(), 0);

	}
	std::string GetByteCode() const { return this->byteCode; }

	void ShutDown()
	{
		pixelShader->Release();
		vertexShader->Release();
		pixelShaderWater->Release();
		matricesBuffer->Release();
		lightBuffer->Release();
	}
};