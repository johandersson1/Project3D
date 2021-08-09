#pragma once
#include "Texture.h"
#include <DirectXMath.h>
#include <vector>
using namespace DirectX;

struct Material
{
    struct Data
    {
        XMFLOAT4 diffuse = { 0.0f, 0.0f, 0.0f, 1.0f };
        XMFLOAT4 ambient = { 0.0f, 0.0f, 0.0f, 1.0f };
        XMFLOAT4 specular = { 0.0f, 0.0f, 0.0f, 1.0f };
        float specularPower = 0.0f;
        XMFLOAT3 padding = { 0.0f, 0.0f, 0.0f };
    } data;
   
    std::vector<Texture*> diffuseTexures;

    bool hasDisplacementTexture = false;
    Texture* displacementTexture;
	~Material() 
	{
		if (displacementTexture)
			delete displacementTexture;
		for (auto& texture : diffuseTexures)
			delete texture;
	}
    Material() = default;

	//void Shutdown()
	//{
	//	diffuseTexures.clear();
	//	//displacementTexture.ShutDown();
	//	//diffuseTexture.ShutDown();
	//}
};