#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <iostream>
#include <DirectXMath.h>

// The G-Buffer with NROFBUFFERS and the views & textures

struct GeometryBuffer
{
    // Enum constants are public, static and final 
   /* enum indexName {
        indexPosition,
        indexColor,
        indexNormal,
        indexCount
    };*/
    static const int NROFBUFFERS = 8;
    int screenWidth;
    int screenHeight;
    ID3D11Texture2D* gBufferTexture[NROFBUFFERS];
    ID3D11RenderTargetView* gBuffergBufferRtv[NROFBUFFERS];
    ID3D11ShaderResourceView* gBufferSrv[NROFBUFFERS];
};

bool SetupD3D11( UINT width, UINT height, HWND window, ID3D11Device*& device, 
	             ID3D11DeviceContext*& immidateContext, IDXGISwapChain*& swapChain,
	             ID3D11RenderTargetView*& rtv, ID3D11Texture2D*& dsTexture, 
	             ID3D11DepthStencilView*& dsView, D3D11_VIEWPORT& viewport, GeometryBuffer& gBuffer);