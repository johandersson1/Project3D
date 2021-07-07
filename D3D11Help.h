#pragma once
#include <d3d11.h>
#include <iostream>

template<typename T>
inline void UpdateBuffer(ID3D11DeviceContext* context, ID3D11Buffer*& buffer, const T& data)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource = {};

	HRESULT hr = context->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if FAILED(hr)
	{
		std::cout << "FAILED TO UPDATE BUFFER" << std::endl;
		return;
	}

	memcpy(mappedResource.pData, &data, sizeof(data));
	context->Unmap(buffer, 0);
}

inline void CreateBuffer(ID3D11Device* device, ID3D11Buffer*& buffer, unsigned int size)
{
    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = 0;
    bufferDesc.ByteWidth = size;

    HRESULT hr = device->CreateBuffer(&bufferDesc, nullptr, &buffer);
    if FAILED(hr)
    {
        std::cout << "FAILED TO CREATE BUFFER" << std::endl;
    }
}
