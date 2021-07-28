#pragma once
#include "Model.h"
#include "D3D11Help.h"
#include "D3D11Helper.h"
#include "ShaderData.h"
#include <fstream>

class TerrainRenderer
{
private:
	const unsigned int stride = sizeof(Vertex);
	const unsigned int offset = 0;
	//Deferred
	const std::string vs_path = "x64/Debug/TerrainVertexShader.cso";
	ID3D11VertexShader* vertexShader;
	const std::string hs_path = "x64/Debug/TerrainHullShader.cso";
	ID3D11HullShader* hullShader;
	const std::string ds_path = "x64/Debug/TerrainDomainShader.cso";
	ID3D11DomainShader* domainShader;
	const std::string ps_path = "x64/Debug/TerrainPixelShader.cso";
	ID3D11PixelShader* pixelShader;
	const std::string gs_path = "x64/Debug/TerrainGeometryShader.cso";
	ID3D11GeometryShader* geometryShader;

	ID3D11Buffer* matricesBuffer;
	ID3D11Buffer* lightBuffer;

	struct Matrices { XMFLOAT4X4 viewPerspective; XMFLOAT4X4 worldSpace; }matrices;
public:

	TerrainRenderer(ID3D11Device* device) :matrices(), hullShader(nullptr), domainShader(nullptr), vertexShader(nullptr)
	{
		CreateBuffer(device, matricesBuffer, sizeof(Matrices));
		CreateBuffer(device, lightBuffer, sizeof(XMMATRIX));

		std::string shaderData;
		std::ifstream reader;
		std::string shaderByteCode;

		// HULL SHADER
		reader.open(hs_path, std::ios::binary | std::ios::beg);

		if (!reader.is_open())
		{
			std::cout << "COULD NOT OPEN FILE: " + hs_path << std::endl;
			return;
		}

		reader.seekg(0, std::ios::end);
		shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
		reader.seekg(0, std::ios::beg);
		shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());

		HRESULT hr = device->CreateHullShader(shaderData.c_str(), shaderData.length(), nullptr, &hullShader);
		if FAILED(hr)
		{
			std::cout << "FAILED TO CREATE HULL SHADER" << std::endl;
			return;
		}


		shaderData.clear();
		reader.close();

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

		hr = device->CreatePixelShader(shaderData.c_str(), shaderData.length(), nullptr, &pixelShader);
		if FAILED(hr)
		{
			std::cout << "FAILED TO CREATE PIXEL SHADER" << std::endl;
			return;
		}


		shaderData.clear();
		reader.close();

		// DOMAIN SHADER
		reader.open(ds_path, std::ios::binary | std::ios::beg);

		if (!reader.is_open())
		{
			std::cout << "COULD NOT OPEN FILE: " + ds_path << std::endl;
			return;
		}

		reader.seekg(0, std::ios::end);
		shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
		reader.seekg(0, std::ios::beg);
		shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());

		hr = device->CreateDomainShader(shaderData.c_str(), shaderData.length(), nullptr, &domainShader);
		if FAILED(hr)
		{
			std::cout << "FAILED TO CREATE DOMAIN SHADER" << std::endl;
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
			std::cerr << "FAILED TO CREATE VERTEX SHADER!" << std::endl;
			return;
		}

		shaderData.clear();
		reader.close();

		reader.open(gs_path, std::ios::binary | std::ios::beg);
		if (!reader.is_open())
		{
			std::cerr << "Could not open GS file!" << std::endl;
			return;
		}

		reader.seekg(0, std::ios::end);
		shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
		reader.seekg(0, std::ios::beg);

		shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());

		if (FAILED(device->CreateGeometryShader(shaderData.c_str(), shaderData.length(), nullptr, &geometryShader)))
		{
			std::cerr << "FAILED TO CREATE GEOMETRY SHADER!" << std::endl;
			return;

		}
		shaderByteCode = shaderData;

		shaderData.clear();
		reader.close();

	}

	void Render(ID3D11DeviceContext* context, Model* model)
	{
		context->IASetInputLayout(ShaderData::model_layout);
		context->VSSetShader(vertexShader, NULL, 0);
		context->PSSetShader(pixelShader, NULL, 0);
		context->HSSetShader(hullShader, NULL, 0);
		context->DSSetShader(domainShader, NULL, 0);
		context->GSSetShader(geometryShader, NULL, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);

		XMStoreFloat4x4(&matrices.worldSpace, XMMatrixTranspose(model->GetWorldMatrix()));
		XMMATRIX viewPerspective = XMMatrixTranspose(ShaderData::viewMatrix *ShaderData::perspectiveMatrix);
		XMStoreFloat4x4(&matrices.viewPerspective, viewPerspective);
		UpdateBuffer(context, matricesBuffer, matrices);
		context->DSSetConstantBuffers(0, 1, &matricesBuffer);


		UpdateBuffer(context, lightBuffer, ShaderData::lightMatrix);
		context->PSSetConstantBuffers(0, 1, &lightBuffer);
		context->PSSetShaderResources(0, 2, model->GetTextures(2));
		context->PSSetShaderResources(2, 1, model->GetDisplacementTexture());
		context->DSSetShaderResources(0, 1, model->GetDisplacementTexture());
		
		context->IASetVertexBuffers(0, 1, model->GetBuffer(), &stride, &offset);
		context->Draw(model->GetVertexCount(), 0);
		context->HSSetShader(NULL, NULL, 0);
		context->DSSetShader(NULL, NULL, 0);
		context->GSSetShader(NULL, NULL, 0);


	}
};
