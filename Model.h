#pragma once
#include "Mesh.h"
#include <DirectXMath.h>
#include <d3d11.h>



class Model
{
private:
	Mesh mesh;
	XMMATRIX worldMatrix;
	ID3D11Buffer* vertexBuffer;
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

	void Update();
	int GetVertexCount() { return this->mesh.vertexCount; }
	XMMATRIX GetWorldMatrix() { return this->worldMatrix; }
	ID3D11Buffer** GetBuffer() { return &this->vertexBuffer; }
	Material::Data GetMaterial() {return this->mesh.material.data;}
	ID3D11ShaderResourceView** GetTexture() { return this->mesh.material.diffuseTexture.Get(); }
	void SetTranslation(XMVECTOR translation) {this->transform.translation = translation; }
	void SetRotation(XMVECTOR rotation) { this->transform.rotation = rotation; }
	void SetScale(XMVECTOR scale) {this->transform.scale = scale; }
};