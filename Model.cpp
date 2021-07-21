#include "Model.h"


Model::Model(ID3D11Device* device, std::string name, XMVECTOR position, XMVECTOR rotation, XMVECTOR scale)
	:transform(position, rotation, scale), name(name)
{
	Update();
	this->mesh = Mesh(device, name);
	
	/*for (auto& face : mesh.faces)
	{
		for (int i = 0; i < 3; ++i)
		{
			
		}
	}*/

	D3D11_BUFFER_DESC desc = {};
	desc.ByteWidth = (int)mesh.faces.size() * sizeof(Face);
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = sizeof(Face);

	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = mesh.faces.data();

	HRESULT hr = device->CreateBuffer(&desc, &data, &vertexBuffer);
	if (FAILED(hr))
	{
		std::cout << "FAILED TO CREATE MESH VERTEXBUFFER" << std::endl;
	}
	desc.ByteWidth = mesh.vertexCount * sizeof(XMFLOAT3);
	desc.StructureByteStride = sizeof(XMFLOAT3);

	data.pSysMem = mesh.positions.data();
	hr = device->CreateBuffer(&desc, &data, &positionsBuffer);
	if (FAILED(hr))
	{
		std::cout << "FAILED TO CREATE MESH VERTEXBUFFER" << std::endl;
	}


	//// MTL BUFFER
	//D3D11_BUFFER_DESC mtlDesc = {};
	//desc.ByteWidth = sizeof(Material.)




}

Model::~Model()
{
	vertexBuffer->Release();
	positionsBuffer->Release();
	
}

void Model::Update()
{
	XMMATRIX translation = XMMatrixTranslationFromVector(transform.translation);
	XMMATRIX rotation = XMMatrixRotationRollPitchYawFromVector(transform.rotation);
	XMMATRIX scale = XMMatrixScalingFromVector(transform.scale);
	worldMatrix = scale * rotation * translation;

}
