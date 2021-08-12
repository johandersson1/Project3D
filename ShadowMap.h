#pragma once
#include <d3d11.h>
#include <iostream>

class ShadowMap
{
private:
	ID3D11ShaderResourceView* depthMapSRV;
	ID3D11DepthStencilView* depthMapDSV;
	D3D11_VIEWPORT viewPort;
public:
	ShadowMap() = default;
	ShadowMap(ID3D11Device* device, UINT size):depthMapSRV(nullptr), depthMapDSV(nullptr)
	{
		// VIEWPORT
		viewPort.TopLeftX = 0.0f;
		viewPort.TopLeftY = 0.0f;
		viewPort.Width = static_cast<float>(size);
		viewPort.Height = static_cast<float>(size);
		viewPort.MinDepth = 0.0f;
		viewPort.MaxDepth = 1.0f;

		// TEXTURE2D
		//Use typeless format because the dsv is going to interpret
		//The bits as DXGI_FORMATD24UNORM_S8UINT, where the SRV is going
		//to interpret the bits as DXGI_FORMAT_R24_UNORM_X8_TYPELESS.

		D3D11_TEXTURE2D_DESC texDesc;
		texDesc.Width = size;
		texDesc.Height = size;
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;									// A two-component, 32-bit typeless format that supports 24 bits for the red channel and 8 bits for the green channel.
																						// this is how depth is defined
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Usage = D3D11_USAGE_DEFAULT;								
		texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;		
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = 0;

		HRESULT(hr);
		ID3D11Texture2D* depthmap = 0;
		hr = (device->CreateTexture2D(&texDesc, 0, &depthmap));
		if ((FAILED(hr)))
		{
			std::cout << "FAILED TO CREATE TEXTURE2D (SHADOW)" << std::endl;
			return;
		}

		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Flags = 0;
		dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // A 32-bit z-buffer format that supports 24 bits for depth and 8 bits for stencil.
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D; // The resource will be accessed as a 2D texture.
		dsvDesc.Texture2D.MipSlice = 0;
		hr = (device->CreateDepthStencilView(depthmap, &dsvDesc, &depthMapDSV));
		if ((FAILED(hr)))
		{
			std::cout << "FAILED TO CREATE DSV (SHADOW)" << std::endl;
			return;
		}
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
		srvDesc.Texture2D.MostDetailedMip = 0;
		hr = (device->CreateShaderResourceView(depthmap, &srvDesc, &depthMapSRV));
		if ((FAILED(hr)))
		{
			std::cout << "FAILED TO CREATE SRV (SHADOW)" << std::endl;
			return;
		}

		depthmap->Release(); // View saves as a reference to the texture so we can release our reference
	}
	void ShutDown()
	{
		this->depthMapDSV->Release();
		this->depthMapSRV->Release();
	}
	~ShadowMap() { this->depthMapSRV->Release(); this->depthMapDSV->Release(); }
	
	ID3D11DepthStencilView** GetDSV() { return &this->depthMapDSV; }
	ID3D11ShaderResourceView** GetSRV() { return &this->depthMapSRV; }

	// Function to bind (initialize?) the shadowmap
	void Bind(ID3D11DeviceContext* context)
	{
		// Set the viewport and the shader resource to the specific SR that we are going to write the depth texture to
		ID3D11ShaderResourceView* null = nullptr;
		context->PSSetShaderResources(8, 1, &null);
		context->RSSetViewports(1, &viewPort);
		// Set null render target because we are only going to draw to depth buffer. 
		// Setting a null render target will disable color writes.
		ID3D11RenderTargetView* renderTargets[1] = { 0 };
		context->OMSetRenderTargets(1, renderTargets, depthMapDSV);
		context->ClearDepthStencilView(depthMapDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
		// Clears the depth-stencil resource.
	}
};

