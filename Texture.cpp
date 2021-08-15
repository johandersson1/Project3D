#include "Texture.h"
#include <iostream>
#include <fstream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture(ID3D11Device* device, std::string path)
{
    int imgWidth, imgHeight;
    unsigned char* image = stbi_load((path).c_str(), &imgWidth, &imgHeight, nullptr, STBI_rgb_alpha);

	// Desc from https://docs.microsoft.com/en-us/windows/win32/direct3d11/overviews-direct3d-11-resources-textures-create
    D3D11_TEXTURE2D_DESC textureDesc = {};
    textureDesc.Width = imgWidth;
    textureDesc.Height = imgHeight;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.MiscFlags = 0;
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Usage = D3D11_USAGE_IMMUTABLE;	// GPU READ
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA data = {};
    data.pSysMem = image;
    data.SysMemPitch = imgWidth * STBI_rgb_alpha;

    ID3D11Texture2D* img;
    HRESULT hr = device->CreateTexture2D(&textureDesc, &data, &img);
    if (FAILED(hr))
    {
        std::cout << "FAILED TO CREATE TEXTURE 2D" << std::endl;
        return;
    }
    hr = device->CreateShaderResourceView(img, nullptr, &this->texture);

    if (FAILED(hr))
    {
        std::cout << "FAILED TO CREATE SHADER RESOURCE VIEW" << std::endl;
        return;
    }

    stbi_image_free(image);
    img->Release();
    img = nullptr;
}
