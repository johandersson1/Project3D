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

    bool hasDiffuseTexture = false;
    Texture diffuseTexture; 
    
    std::vector<Texture> diffuseTexures;

    bool hasDisplacementTexture = false;
    Texture displacementTexture;

    ID3D11ShaderResourceView** GetDiffuseTextures(int count)
    {
        ID3D11ShaderResourceView** srvs = new ID3D11ShaderResourceView* [diffuseTexures.size()];

        for (int i = 0; i < count; i++)
        {
            if (i > diffuseTexures.size())
                break;
            srvs[i] = *diffuseTexures[i].Get();
        }
        return srvs;
		for (int i = 0; i < count; i++)
		{
			srvs[i]->Release();
		}
    }

    Material() = default;

	void Shutdown()
	{
		diffuseTexures.clear();
		//displacementTexture.ShutDown();
		//diffuseTexture.ShutDown();
	}
};