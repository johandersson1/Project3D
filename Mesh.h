#pragma once
#include <iostream>
#include "Geometry.h"
#include "Material.h"
#include <vector>
#include "D3D11Help.h"

using namespace DirectX;

struct Mesh
{
private:
	bool LoadModel(std::string name);
	bool LoadMaterial(ID3D11Device* device, std::string name);
	std::string name;
public:
	Mesh() = default;
	Mesh(ID3D11Device* device, std::string name)
		:name(name)
	{
		if (!LoadModel(name) || !LoadMaterial(device, name))
			std::cout << "FAILED TO LOAD MESH DATA" << std::endl;
	}
	~Mesh();

	Material material;

	ID3D11Buffer* mtlBuffer;

	void AddDiffuseTexture(ID3D11Device* device, std::string fileName) { material.diffuseTexures.push_back(Texture(device, "Models/" + name + "/" + fileName)); }
		
	void AddDisplacementTexture(ID3D11Device* device, std::string fileName) { material.displacementTexture = Texture(device, "Models/" + name + "/" + fileName); }

	ID3D11Buffer* GetMTLBuffer() { return this->mtlBuffer; }

	int vertexCount = 0;
	std::vector<Face> faces;
	std::vector<XMFLOAT3> positions;
};