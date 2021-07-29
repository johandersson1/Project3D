#pragma once
#include <string>
#include <d3d11.h>
#include <DirectXMath.h>
class Texture
{
private:
    ID3D11ShaderResourceView* texture;
public:

    ID3D11ShaderResourceView** Get() { return &this->texture; }

    Texture() = default;
    Texture(ID3D11Device* device, std::string path);
    void ShutDown() { texture->Release(); }
 /*   ~Texture() { if(texture)texture->Release(); }*/
};