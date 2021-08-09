#pragma once
#include "Mesh.h"
#include <DirectXMath.h>
#include <d3d11.h>

class Model
{
private:
	Mesh mesh;
	XMMATRIX worldMatrix;

	// Water-effect
	XMFLOAT2 movementSpeedUv;
	XMFLOAT2 offsetUV;

	std::string name;

	struct Transform
	{
		XMVECTOR translation;
		XMVECTOR rotation;
		XMVECTOR scale;
		Transform(XMVECTOR translation, XMVECTOR  rotation, XMVECTOR scale) 
			    : translation(translation), rotation(rotation), scale(scale) {}

	}transform;

public:

	Model() = default;
	Model(ID3D11Device* device, std::string name, XMVECTOR position = { 0.0f,0.0f,0.0f }, 
		  XMVECTOR rotation = { 0.0f,0.0f,0.0f }, XMVECTOR scale = { 1.0f, 1.0f, 1.0f });
	~Model()
	{
		this->vertexBuffer->Release();

		/*if (waterBuffer)
			this->waterBuffer->Release();*/

		this->positionsBuffer->Release();
	}
	ID3D11Buffer* waterBuffer = nullptr;
	ID3D11Buffer* vertexBuffer = nullptr;
	ID3D11Buffer* positionsBuffer = nullptr;


	void Update();
	int GetVertexCount() { return this->mesh.vertexCount; }

	XMMATRIX GetWorldMatrix() { return this->worldMatrix; }
	// Getting the differentBuffers, Mtl and WaterBuffer mostly for for map,unmap and for setting the constant buffers used by the pixel shader pipeline stage
	void BindTextures(ID3D11DeviceContext* context, int startSlot = 0) { int slot = startSlot; for (auto& texture : mesh.material.diffuseTexures) { texture->BindTexture(context, slot); slot++; } }
	void BindDisplacementTexture(ID3D11DeviceContext* context, int startSlot = 0) { mesh.material.displacementTexture->BindTexture(context, startSlot, DS); }
	ID3D11Buffer** GetBuffer() { return &this->vertexBuffer; }
	ID3D11Buffer** GetMTLBuffer() { return &this->mesh.mtlBuffer; }
	ID3D11Buffer** GetWaterBuffer() { return &this->waterBuffer; }
	ID3D11Buffer** GetPositionsBuffer() { return &this->positionsBuffer; }
	XMFLOAT2* GetUVOffset() { return &this->offsetUV; }
	const Material::Data& GetMaterial() {return this->mesh.material.data;}
	// Blend
	// Terrain
	void AddTexture(ID3D11Device* device, std::string fileName) { this->mesh.material.diffuseTexures.emplace_back(new Texture(device, fileName)); }
	void SetDisplacementTexture(ID3D11Device* device, std::string path) { this->mesh.material.displacementTexture = new Texture(device, path); }
	// Setting Tranlation, Rotation and Scale
	void SetTranslation(XMVECTOR translation) {this->transform.translation = translation; }
	void SetRotation(XMVECTOR rotation) { this->transform.rotation = rotation; }
	void SetScale(XMVECTOR scale) {this->transform.scale = scale; }
	XMVECTOR GetRotation() { return this->transform.rotation; }
	// Water
	void WaterSettings(XMFLOAT2 movementSpeedUv, float deltatime);
};