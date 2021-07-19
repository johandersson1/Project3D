#pragma once
#include <iostream>
#include "Geometry.h"
#include "Material.h"
#include <vector>

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
	void AddDiffuseTexture(ID3D11Device* device, std::string fileName) { material.diffuseTexures.push_back(Texture(device, "Models/" + name + "/" + fileName)); }
		
	void AddDisplacementTexture(ID3D11Device* device, std::string fileName) { material.displacementTexture = Texture(device, "Models/" + name + "/" + fileName); }

	Material material;

	int vertexCount = 0;
	std::vector<Face> faces;
	std::vector<XMFLOAT3> positions;
};