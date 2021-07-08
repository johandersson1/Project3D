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
}

Model::~Model()
{
	vertexBuffer->Release();
}

void Model::Update()
{
	XMMATRIX translation = XMMatrixTranslationFromVector(transform.translation);
	XMMATRIX rotation = XMMatrixRotationRollPitchYawFromVector(transform.rotation);
	XMMATRIX scale = XMMatrixScalingFromVector(transform.scale);
	worldMatrix = scale * rotation * translation;

}
