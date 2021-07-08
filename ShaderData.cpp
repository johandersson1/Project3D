#include"ShaderData.h"
#include <fstream>

	std::string ShaderData::positionOnly_vs_path = "x64/Debug/ParticleVertexShader.cso";
	ID3D11VertexShader* ShaderData::positionOnly_vs; 
	
	XMFLOAT3 ShaderData::cameraPosition;
	XMMATRIX ShaderData::viewMatrix;
	XMMATRIX ShaderData::perspectiveMatrix;
	
	ID3D11InputLayout* ShaderData::positionOnly_layout;


	void ShaderData::Shutdown()
	{
		positionOnly_vs->Release();
		positionOnly_layout->Release();
	}

	void ShaderData::Initialize(ID3D11Device* device)
{
	std::string byteCode;
	std::string shaderData;
	std::ifstream reader;
	//Clears the reader and shaderData and does the same thing for the PixelShader

	reader.open(positionOnly_vs_path, std::ios::binary | std::ios::ate);
	if (!reader.is_open())
	{
		std::cerr << "Could not open VS file!" << std::endl;
		return;
	}

	reader.seekg(0, std::ios::end);
	shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
	reader.seekg(0, std::ios::beg);

	shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());

	if (FAILED(device->CreateVertexShader(shaderData.c_str(), shaderData.length(), nullptr, &positionOnly_vs)))
	{
		std::cerr << "Failed to create position VERTEX shader!" << std::endl;
		return;
	}

	byteCode = shaderData;

	shaderData.clear();
	reader.close();
	D3D11_INPUT_ELEMENT_DESC positionDesc[1] = 
	{ 
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0} 
	};

	HRESULT hr = device->CreateInputLayout(positionDesc, 1, byteCode.c_str(), byteCode.length(), &positionOnly_layout);
	if (FAILED(hr))
	{
		std::cout << "FAILED TO CREATE POSITION INPUT LAYOUT" << std::endl;
	}
}

void ShaderData::Update(Camera camera)
{
	cameraPosition = camera.GetPosition();
	viewMatrix = camera.GetViewMatrix();
	perspectiveMatrix = camera.GetPerspectiveMatrix();
}
