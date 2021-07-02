#pragma once

#include "stb_image.h"
#include <array>
#include <d3d11.h>
#include <DirectXMath.h>

//struct för cbVertex, med två matriser för worldViewProj och worldspace
struct cbVertex
{
    DirectX::XMMATRIX worldViewProj;
    DirectX::XMMATRIX worldSpace;
};
//struct för Light
struct Light
{
    //flera paddings variabler för att det ska inte bli någon offset med variablerna
    DirectX::XMFLOAT3 lightColour;
    float padding1;
    DirectX::XMFLOAT3 lightAmbient;
    float padding2;
    DirectX::XMFLOAT3 lightDiffuse;
    float padding3;
    DirectX::XMFLOAT3 lightCamPos;
    float padding4;
    DirectX::XMFLOAT3 lightAttenuation;
    float padding5;
};
//struct för SimpleVertex
struct SimpleVertex
{
    float pos[3];
    float uv[2];
    float clr[3];
    float norm[3];

    SimpleVertex(const std::array<float, 3>& position, const std::array<float, 2>& uvCoords, const std::array<float, 3>& colour, const std::array<float, 3>& normal)
    {
        for (int i = 0; i < 3; ++i)
        {
            pos[i] = position[i];
            clr[i] = colour[i];
        }

        uv[0] = uvCoords[0];
        uv[1] = uvCoords[1];

        for (int j = 0; j < 3; ++j)
        {
            norm[j] = normal[j];
        }
    }
};

bool SetupPipeline(ID3D11Device* device, ID3D11Buffer*& vertexBuffer, 
                   ID3D11VertexShader*& vShader,ID3D11PixelShader*& pShader, 
                   ID3D11InputLayout*& inputLayout, ID3D11Buffer*& constantBuffer,
                   ID3D11Texture2D*& texture, ID3D11ShaderResourceView*& textureSRV, 
                   ID3D11SamplerState*& sampler);

