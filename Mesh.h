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
	bool LoadBlendMaterials(ID3D11Device* device, std::string name);
public:
	Mesh() = default;
	Mesh(ID3D11Device* device, std::string name)
	{
		if (!LoadModel(name) || !LoadMaterial(device, name) || !LoadBlendMaterials(device, name))
			std::cout << "FAILED TO LOAD MESH DATA" << std::endl;
	}

	Material material;

	int vertexCount = 0;
	std::vector<Face> faces;
	std::vector<XMFLOAT3> positions;
};