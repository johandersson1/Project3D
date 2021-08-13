#pragma once
#include <string>
#include <d3d11.h>
#include <DirectXMath.h>
#include "D3D11Helper.h"


class Texture
{
private:
    ID3D11ShaderResourceView* texture = nullptr;
public:
    
    ID3D11ShaderResourceView** Get() { return &this->texture; }

    Texture() = default;
    Texture(ID3D11Device* device, std::string path);

	void Bind(ID3D11DeviceContext* context, int slot = 0, shaders shader = shaders::PS)
	{
		switch (shader)
		{
		case shaders::PS:
			context->PSSetShaderResources(slot, 1, &texture);
			break;
		case shaders::DS:
			context->DSSetShaderResources(slot, 1, &texture);
			break;
		}
	}


    ~Texture() {this->texture->Release(); }
};