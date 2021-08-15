#pragma once
#include "ParticleSystem.h"
#include "D3D11Helper.h"
#include "ShaderData.h"
#include <fstream>
class ParticleRenderer
{
private:
	unsigned int stride = sizeof(XMFLOAT3);
	unsigned int offset = 0;

	ID3D11Buffer* cameraBuffer;
	ID3D11Buffer* viewPersPectiveBuffer;
	const std::string ps_path = "x64/Debug/ParticlePixelShader.cso";
	ID3D11PixelShader* pixelShader;
	const std::string gs_path = "x64/Debug/ParticleGeometryShader.cso";
	ID3D11GeometryShader* geometryShader;

public:

	ParticleRenderer(ID3D11Device* device):pixelShader(nullptr),geometryShader(nullptr)
	{
		CreateBuffer(device, viewPersPectiveBuffer, sizeof(XMFLOAT4X4));
		CreateBuffer(device, cameraBuffer, sizeof(XMFLOAT4));

		std::string shaderData;
		std::ifstream reader;

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

		// GEOMETRY SHADER
		reader.open(gs_path, std::ios::binary | std::ios::beg);

		if (!reader.is_open())
		{
			std::cout << "COULD NOT OPEN FILE: " + gs_path << std::endl;
			return;
		}

		reader.seekg(0, std::ios::end);
		shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
		reader.seekg(0, std::ios::beg);
		shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());

		hr = device->CreateGeometryShader(shaderData.c_str(), shaderData.length(), nullptr, &geometryShader);
		if FAILED(hr)
		{
			std::cout << "FAILED TO CREATE GEOMETRY SHADER" << std::endl;
			return;
		}

		shaderData.clear();
		reader.close();

	}
	void ShutDown()
	{
		cameraBuffer->Release();
		viewPersPectiveBuffer->Release();
		geometryShader->Release();
		pixelShader->Release();
	}

	void Render(ID3D11DeviceContext* context, ParticleSystem* particlesystem)
	{
		UpdateBuffer(context, cameraBuffer, ShaderData::cameraPosition);								// Update the buffer containing the users position ( used for billboarding )
		XMMATRIX viewPerspectiveMatrix = ShaderData::viewMatrix * ShaderData::perspectiveMatrix; 		// Get the viewPerspectiveMatrix
		XMFLOAT4X4 viewPersp;
		XMStoreFloat4x4(&viewPersp, XMMatrixTranspose(viewPerspectiveMatrix));
		UpdateBuffer(context, viewPersPectiveBuffer, viewPersp);

		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);		// The particles are made of points that are then used in the GS to create a quad
		context->IASetInputLayout(ShaderData::positionOnly_layout); 				// The points only uses a position
		context->VSSetShader(ShaderData::positionOnly_vs, NULL, 0);
		context->PSSetShader(pixelShader, NULL, 0);									// Used to set the color of the particles ( no texturing ) 
		context->GSSetShader(geometryShader, NULL, 0);							    // Used to create and billboard the particle itself

		// Set the buffers for the GS, VS, Draw and reset the geometryshader for the next pass
		context->GSSetConstantBuffers(1, 1, &cameraBuffer); 
		context->GSSetConstantBuffers(2, 1, &viewPersPectiveBuffer);
		context->IASetVertexBuffers(0, 1, &particlesystem->vertexBuffer, &stride, &offset);
		context->Draw(particlesystem->particleCount, 0);

		context->GSSetShader(NULL, NULL, 0);
	}
};