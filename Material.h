#pragma once
#include "Texture.h"
#include <DirectXMath.h>

using namespace DirectX;

struct Material
{
private:
    //std::string defaultTexture = "Models/Default/Missing_Texture.jpg";
public:
    struct Data
    {
        XMFLOAT4 diffuse = { 0.0f, 0.0f, 0.0f, 1.0f };
        XMFLOAT4 ambient = { 0.0f, 0.0f, 0.0f, 1.0f };
        XMFLOAT4 specular = { 0.0f, 0.0f, 0.0f, 1.0f };
        float specularPower = 0.0f;
        XMFLOAT3 padding = { 0.0f, 0.0f, 0.0f };
    } data;

    bool hasDiffuseTexture = false;
    Texture diffuseTexture; // = Texture(defaultTexture);

    bool hasDisplacementTexture = false;
    Texture displacementTexture; // = Texture(defaultTexture);

    Material() = default;
};