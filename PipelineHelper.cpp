#define STB_IMAGE_IMPLEMENTATION
#include "PipelineHelper.h"
#include <fstream>
#include <string>
#include <iostream>
#include <vector>

bool LoadShaders(ID3D11Device* device, ID3D11VertexShader*& vShader, ID3D11PixelShader*& pShader, std::string& vShaderByteCode)
{
	std::string shaderData;
	std::ifstream reader;
	reader.open("Debug/VertexShader.cso", std::ios::binary | std::ios::ate);
	if (!reader.is_open())
	{
		std::cerr << "Could not open VS file!" << std::endl;
		return false;
	}

	reader.seekg(0, std::ios::end);
	shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
	reader.seekg(0, std::ios::beg);

	shaderData.assign((std::istreambuf_iterator<char>(reader)),
		std::istreambuf_iterator<char>());

	if (FAILED(device->CreateVertexShader(shaderData.c_str(), shaderData.length(), nullptr, &vShader)))
	{
		std::cerr << "Failed to create vertex shader!" << std::endl;
		return false;
	}

	vShaderByteCode = shaderData;
	shaderData.clear();
	reader.close();
	reader.open("Debug/PixelShader.cso", std::ios::binary | std::ios::ate);
	if (!reader.is_open())
	{
		std::cerr << "Could not open PS file!" << std::endl;
		return false;
	}

	reader.seekg(0, std::ios::end);
	shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
	reader.seekg(0, std::ios::beg);

	shaderData.assign((std::istreambuf_iterator<char>(reader)),
		std::istreambuf_iterator<char>());

	if (FAILED(device->CreatePixelShader(shaderData.c_str(), shaderData.length(), nullptr, &pShader)))
	{
		std::cerr << "Failed to create pixel shader!" << std::endl;
		return false;
	}

	return true;
}

//CreateInputLayout function to describe the input-buffer data
bool CreateInputLayout(ID3D11Device* device, ID3D11InputLayout*& inputLayout, const std::string& vShaderByteCode)
{
	//Array of elements with description
	D3D11_INPUT_ELEMENT_DESC inputDesc[] =
	{
		//Name, Index, Format, Slot, ByteOffset, InputSlotClass, StepRate
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOUR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}

		//These need to be in the right order for the data to be placed at the right location
	};

	HRESULT hr = device->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), vShaderByteCode.c_str(), vShaderByteCode.length(), &inputLayout);

	return !FAILED(hr);
}

bool CreateVertexBuffer(ID3D11Device* device, ID3D11Buffer*& vertexBuffer)
{
	Vertex quad[] =
	{
		//Top Right
		{ {0.5f, 0.5f, 0.0f}, {1.0f, 0.0f}, {1, 1, 1}, {0, 0, -1} },
		//Bottom Right
		{ {0.5f, -0.5f, 0.0f}, {1.0f, 1.0f}, {0, 1, 0}, {0, 0, -1}  },
		//Top Left
		{ {-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f}, {1, 0, 0}, {0, 0, -1}  },
		//Bottom Left
		{ {-0.5f, -0.5f, 0.0f}, {0.0f, 1.0f}, {1, 1, 1}, {0, 0, -1}  }
		//(X, Y, Z), UVs, Colour, Normals
	};

	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.ByteWidth = sizeof(quad);
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = quad;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	HRESULT hr = device->CreateBuffer(&bufferDesc, &data, &vertexBuffer);
	return !FAILED(hr);
}

//CreateTexture function
bool CreateTexture(ID3D11Device* device, ID3D11Texture2D*& texture, ID3D11ShaderResourceView*& textureSRV)
{
	//Width, Height and channel values
	int textureWidth = 512;
	int textureHeight = 512;
	int channels = 3; //RGB

	//Loads the image from the Debug folder
	unsigned char* imageData = stbi_load("3D Project/wiz.jpg", &textureWidth, &textureHeight, &channels, 4);

	std::vector<unsigned char> textureData;
	textureData.resize(textureWidth * textureHeight * 4);

	//Loops for each pixel and assigns the colour depending on the image loaded into imageData
	for (int h = 0; h < textureHeight; ++h)
	{
		for (int w = 0; w < textureWidth; ++w)
		{
			unsigned int pos0 = w * 4 + textureWidth * 4 * h;
			textureData[pos0 + 0] = imageData[pos0 + 0];
			textureData[pos0 + 1] = imageData[pos0 + 1];
			textureData[pos0 + 2] = imageData[pos0 + 2];
			textureData[pos0 + 3] = imageData[pos0 + 3];
		}
	}

	//Description of the Texture
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = textureWidth; //Width
	desc.Height = textureHeight; //Height
	desc.MipLevels = 1; //Miplevels are used for optimizing images ,speeding up rendering times. Dont need more than 1
	desc.ArraySize = 1; //Amount of textures
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //RGBA
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0; //Lowest quality, no need for more
	desc.Usage = D3D11_USAGE_IMMUTABLE; //No CPU access
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE; //Binds as Shader
	desc.CPUAccessFlags = 0; //No CPU access
	desc.MiscFlags = 0; //No additional flags

	//The subresource data 
	D3D11_SUBRESOURCE_DATA  data; //Pointer to the initialization data
	data.pSysMem = &textureData[0];
	data.SysMemPitch = textureWidth * 4; //Distance (bytes) between beginning of a row in a texture to the next one 
	data.SysMemSlicePitch = 0; // --Only used for 3D texture data-- Distance (bytes) from the beginning of depth level to the next

	if (FAILED(device->CreateTexture2D(&desc, &data, &texture)))
	{
		std::cerr << "Failed to create texture!" << std::endl;
		return false;
	}

	HRESULT hr = device->CreateShaderResourceView(texture, nullptr, &textureSRV);
	return !FAILED(hr);
}
bool CreateSamplerState(ID3D11Device* device, ID3D11SamplerState*& sampler)
{
	D3D11_SAMPLER_DESC desc;
	desc.Filter = D3D11_FILTER_ANISOTROPIC; //Anistropic filter -- different properties depending on the material
	desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP; //Wraps UVs that are outside of the boundries of a texture. 0-3 U, V, W 
	desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.MipLODBias = 0; //No offset from the mipmap
	desc.MaxAnisotropy = 16; //Value of the Anistropic, works between 1-16
	desc.BorderColor[0] = desc.BorderColor[1] = desc.BorderColor[2] = desc.BorderColor[3] = 0; //Sets the bordercolour to black
	desc.MinLOD = 0; //Clamp to 0, largest and most detailed
	desc.MaxLOD = D3D11_FLOAT32_MAX; //No upper limit, using D3D11_FLOAT32_MAX

	HRESULT hr = device->CreateSamplerState(&desc, &sampler);
	return !FAILED(hr);
}

bool SetupPipeline(ID3D11Device* device, ID3D11Buffer*& vertexBuffer, ID3D11VertexShader*& vShader, ID3D11PixelShader*& pShader,
	ID3D11InputLayout*& inputLayout, ID3D11Texture2D*& texture, ID3D11ShaderResourceView*& textureSRV, ID3D11SamplerState*& sampler)
{
	std::string vShaderByteCode;
	if (!LoadShaders(device, vShader, pShader, vShaderByteCode))
	{
		std::cerr << "Error loading shaders!" << std::endl;
		return false;
	}

	if (!CreateInputLayout(device, inputLayout, vShaderByteCode))
	{
		std::cerr << "Error creating input layout!" << std::endl;
		return false;
	}

	if (!CreateVertexBuffer(device, vertexBuffer))
	{
		std::cerr << "Error creating vertex buffer!" << std::endl;
		return false;
	}
	if (!CreateTexture(device, texture, textureSRV))
	{
		std::cerr << "Error creating texture or srv!" << std::endl;
		return false;
	}
	if (!CreateSamplerState(device, sampler))
	{
		std::cerr << "Error creating sampler state!" << std::endl;
		return false;
	}
	return true;
}