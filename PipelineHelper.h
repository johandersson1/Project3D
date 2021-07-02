#pragma once
#include <array>
#include <d3d11.h>
#include "stb_image.h"
struct Vertex
{
	float pos[3];
	float uv[2];
	float clr[3];
	float nor[3];

	Vertex(const std::array<float, 3>& position, const std::array<float, 2>& uvCoords, const std::array<float, 3>& colour, const std::array<float, 3>& normal)
	{
		for (int i = 0; i < 3; ++i)
		{
			pos[i] = position[i];
			clr[i] = colour[i];
		}
		uv[0] = uvCoords[0];
		uv[1] = uvCoords[1];
		for (int k = 0; k < 3; ++k)
		{
			nor[k] = normal[k];
		}
	}
};

bool SetupPipeline(ID3D11Device* device, ID3D11Buffer*& vertexBuffer, ID3D11VertexShader*& vShader, ID3D11PixelShader*& pShader,
					ID3D11InputLayout*& inputLayout, ID3D11Texture2D*& texture, ID3D11ShaderResourceView*& textureSRV, ID3D11SamplerState*& sampler);
