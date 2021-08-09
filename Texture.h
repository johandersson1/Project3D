#pragma once
#include <string>
#include <d3d11.h>
#include <DirectXMath.h>

enum Shader { PS, DS };

class Texture
{
private:
    ID3D11ShaderResourceView* texture;
public:

   // ID3D11ShaderResourceView** Get() { return &this->texture; }


    Texture() = default;
    Texture(ID3D11Device* device, std::string path);
	void BindTexture(ID3D11DeviceContext* context, int slot = 0, Shader shader = Shader::PS)
	{ 
		switch (shader)
		{
			case Shader::PS:
				context->PSSetShaderResources(slot, 1, &texture);
				break;
				
			case Shader::DS:
				context->DSSetShaderResources(slot, 1, &texture);
				break;
		}
	}
    ~Texture() { this->texture->Release(); }
};