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
    } data;
   
    // std::vector is a sequence container that encapsulates dynamic size arrays
    std::vector<Texture*> diffuseTexures;

    Texture *displacementTexture = nullptr;

	~Material()
	{
		if (displacementTexture)
			delete displacementTexture;
		for (auto& texture : diffuseTexures)
			delete texture;
	}

    Material() = default;
};