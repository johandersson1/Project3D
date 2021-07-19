#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <iostream>
#include <DirectXMath.h>

struct GeometryBuffer
{
    enum indexName {
        indexPosition,
        indexColor,
        indexNormal,
        indexCount
    };
    static const int NROFBUFFERS = 7;
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