#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <iostream>
#include <DirectXMath.h>

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
// Setup all things D3D11
bool SetupD3D11( UINT width, UINT height, HWND window, ID3D11Device*& device, 
	             ID3D11DeviceContext*& immidateContext, IDXGISwapChain*& swapChain,
	             ID3D11RenderTargetView*& rtv, ID3D11Texture2D*& dsTexture, 
	             ID3D11DepthStencilView*& dsView, D3D11_VIEWPORT& viewport, GeometryBuffer& gBuffer);