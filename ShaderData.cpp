#include"ShaderData.h"
#include <fstream>

	std::string ShaderData::positionOnly_vs_path = "x64/Debug/ParticleVertexShader.cso";
	ID3D11VertexShader* ShaderData::positionOnly_vs; 
	
	XMFLOAT3 ShaderData::cameraPosition;
	XMMATRIX ShaderData::viewMatrix;
	XMMATRIX ShaderData::lightMatrix;
	XMMATRIX ShaderData::perspectiveMatrix;

	ID3D11InputLayout* ShaderData::positionOnly_layout;
	ID3D11InputLayout* ShaderData::model_layout;

	std::string ShaderData::gs_path = "x64/Debug/CullingGeometryShader.cso";
	ID3D11GeometryShader* ShaderData::geometryShader;

	ID3D11Buffer* ShaderData::cameraPos;

	ShadowMap* ShaderData::shadowmap;

	void ShaderData::Shutdown()
	{
		positionOnly_vs->Release();
		positionOnly_layout->Release();
		model_layout->Release();
		cameraPos->Release();
		geometryShader->Release();
		shadowmap->ShutDown();
		
	}

	void ShaderData::Initialize(ID3D11Device* device, std::string modelVSByteCode)
{
	shadowmap = new ShadowMap(device, 4096);
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

	//array av element med en beskrivning
	D3D11_INPUT_ELEMENT_DESC inputDesc[] =
	{
		//name, index, format, slot, byteOffset, inputSlotClass, stepRate
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}, //Input data is per-vertex data.
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	//Viktigt!! Allt behöver vara i rätt ordning för om det laddas in i fel ordning så funkar det ej
	hr = device->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), modelVSByteCode.c_str(), modelVSByteCode.length(), &model_layout);

	if FAILED(hr)
	{
		std::cerr << "ERROR INPUTLAYOUT" << std::endl;
		return;
	}

	reader.open(gs_path, std::ios::binary | std::ios::ate);
	if (!reader.is_open())
	{
		std::cout << "Could not open GS file!" << std::endl;
		return;
	}
	reader.seekg(0, std::ios::end);
	shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
	reader.seekg(0, std::ios::beg);
	shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());
	hr = device->CreateGeometryShader(shaderData.c_str(), shaderData.length(), nullptr, &geometryShader);

	if (FAILED(hr))
	{
		std::cout << "Failed to create position VERTEX shader!" << std::endl;
		return;
	}

	shaderData.clear();
	reader.close();

	CreateBuffer(device, cameraPos, sizeof(XMFLOAT4));
}


void ShaderData::Update(ID3D11DeviceContext*& context, Camera camera, DirectionalLight& dirLight)
{
	cameraPosition = camera.GetPosition();
	viewMatrix = camera.GetViewMatrix();
	perspectiveMatrix = camera.GetPerspectiveMatrix();
	lightMatrix = dirLight.GetMatrix();

	UpdateBuffer(context, cameraPos, camera.GetPosition());
	context->GSSetConstantBuffers(0, 1, &cameraPos);
	// CAM POS
	//std::cout << "X: " << cameraPosition.x << " Y: " << cameraPosition.y << " Z: " << cameraPosition.z << std::endl;
}
