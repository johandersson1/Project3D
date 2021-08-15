#include "Model.h"


Model::Model(ID3D11Device* device, std::string name, XMVECTOR position, XMVECTOR rotation, XMVECTOR scale)
	:transform(position, rotation, scale), name(name), mesh(device, name)
{
	Update();
	
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
		std::cout << "FAILED TO CREATE POSITION VERTEXBUFFER" << std::endl;
	}
}

Model::~Model()
{
	
}

// Function to update the models matrices after changing either of them
void Model::Update()
{
	XMMATRIX translation = XMMatrixTranslationFromVector(transform.translation);
	XMMATRIX rotation = XMMatrixRotationRollPitchYawFromVector(transform.rotation);
	XMMATRIX scale = XMMatrixScalingFromVector(transform.scale);
	worldMatrix = scale * rotation * translation;
}

void Model::WaterSettings(XMFLOAT2 movementSpeedUv, float deltatime)
{	
	// Offset the UVs on both X and Y axis with the variable from the parameter * deltatime
	// Reset the the offset down below 1 so the value doesnt get too large

	offsetUV.x += movementSpeedUv.x * deltatime;
	if (offsetUV.x > 1)
		offsetUV.x = 1 - offsetUV.x;
	offsetUV.y += movementSpeedUv.y * deltatime;
	if (offsetUV.y > 1)
		offsetUV.y = 1 - offsetUV.y;
}
