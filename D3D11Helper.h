#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <iostream>
#include <DirectXMath.h>

enum class shaders{ DS, PS};

// The G-Buffer with NROFBUFFERS and the views & textures
// Geometry buffer, the information about our geometry is stored in a gBuffer
struct GeometryBuffer 
{
    static const int NROFBUFFERS = 8; // Numbers of buffers that we want
    int screenWidth;
    int screenHeight;

	// Create Texture, RTV, SRV arrays
    ID3D11Texture2D* gBufferTexture[NROFBUFFERS]; 
    ID3D11RenderTargetView* gBuffergBufferRtv[NROFBUFFERS];
    ID3D11ShaderResourceView* gBufferSrv[NROFBUFFERS];
};
// Default UpdateBuffer with map,unmap and specify the buffer and the data that we need 
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
// Default CreateBuffer, specify which buffer and size we want for it
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

// Setup all things D3D11
bool SetupD3D11( UINT width, UINT height, HWND window, ID3D11Device*& device, 
	             ID3D11DeviceContext*& immidateContext, IDXGISwapChain*& swapChain,
	             ID3D11RenderTargetView*& rtv, ID3D11Texture2D*& dsTexture, 
	             ID3D11DepthStencilView*& dsView, D3D11_VIEWPORT& viewport, GeometryBuffer& gBuffer);