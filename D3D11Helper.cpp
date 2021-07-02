#include "D3D11Helper.h"

bool CreateInterfaces(UINT width, UINT height, HWND window, ID3D11Device*& device, ID3D11DeviceContext*& immediateContext, IDXGISwapChain*& swapChain)
{
    DXGI_SWAP_CHAIN_DESC swapChainDesc = {}; //Beskriver en swapchain

    swapChainDesc.BufferDesc.Width = width; //bredd
    swapChainDesc.BufferDesc.Height = height; //h�jd
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 0; //RefreshRate 60 hertz (0 fr�n b�rjan) vsync och fullscreen beh�vs f�r att det ska synas
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM; //32 bits, 8 per channel
    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; //Scanline-order �r ospecificerad.
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; //Ospecificerad skalning.

    // Default
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;

    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; //Anv�nd ytan eller resursen som ett resultat f�r renderingen.
    swapChainDesc.BufferCount = 1; //double buffer 
    swapChainDesc.OutputWindow = window;
    swapChainDesc.Windowed = true;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; //tar bort innerh�llet av backbuffern n�r den kallas p�
    swapChainDesc.Flags = 0;

    UINT flags = 0;
    if (_DEBUG)
        flags = D3D11_CREATE_DEVICE_DEBUG;

    D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 }; //Targets features supported by Direct3D 11.0, including shader model 5.

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr, //IDXGI Adapter
        D3D_DRIVER_TYPE_HARDWARE, //l�ter programmet v�lja, f�r att vi har nullptr p� f�rsta parametern.
        nullptr, //vilken typ av drivrutin f�r mjukvaran
        flags, //the runtime layers to enable 
        featureLevels, //pekare till en array som best�mmer vilken ordning av future levels som skapas = D3D_FEATURE_LEVEL_11_0
        1, //vi kallar p� element D3D_FEATURE_LEVEL_11_0 
        D3D11_SDK_VERSION, //standard versionen som anv�nds (sdk_version)
        &swapChainDesc, //swapchain description, pekare
        &swapChain, //H�mtar adressen f�r swapcahin
        &device, //H�mtar adressen f�r device
        nullptr, //ingen definition beh�vs till detta
        &immediateContext//H�mtar adressen f�r immediateContext  
    );

    return !(FAILED(hr));
}

bool CreateRenderTargetView(ID3D11Device* device, IDXGISwapChain* swapChain, ID3D11RenderTargetView*& rtv)
{
    ID3D11Texture2D* backBuffer = nullptr;
    if (FAILED(swapChain->GetBuffer(0, _uuidof(ID3D11Texture2D), reinterpret_cast<void**> (&backBuffer))))
    {
        std::cerr << "Failed to get back buffer!" << std::endl;
        return false;
    }

    HRESULT hr = device->CreateRenderTargetView(backBuffer, nullptr, &rtv);

    backBuffer->Release();

    return !FAILED(hr);
}

bool CreateDepthStencil(ID3D11Device* device, UINT width, UINT height, ID3D11Texture2D*& dsTexture, ID3D11DepthStencilView*& dsView)
{
    D3D11_TEXTURE2D_DESC textureDesc;
    textureDesc.Width = width; //bredd
    textureDesc.Height = height; //h�jd
    textureDesc.MipLevels = 1; //g�r att renderingen blir snabbare, beh�vs endast en f�r multisampled texture
    textureDesc.ArraySize = 1;//endast en bild
    textureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; //A 32-bit z-buffer format that supports 24 bits for depth and 8 bits for stencil.
    textureDesc.SampleDesc.Count = 1; //antalet multisamples per pixel
    textureDesc.SampleDesc.Quality = 0;//ingen �kat kvalit�, on�digt 
    textureDesc.Usage = D3D11_USAGE_DEFAULT; //	en resurs som kr�ver read and write tillg�ng av GPU
    textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL; //Binder en textur som en depth-stencil target
    textureDesc.CPUAccessFlags = 0; //ingen CPU �tkomst
    textureDesc.MiscFlags = 0; //beh�vs inga mer flaggor

    if (FAILED(device->CreateTexture2D(&textureDesc, nullptr, &dsTexture)))
    {
        std::cerr << "Failed to create depth stencil texture!" << std::endl;
        return false;
    }

    HRESULT hr = device->CreateDepthStencilView(dsTexture, nullptr, &dsView);
    return !(FAILED(hr));
}

void SetViewPort(D3D11_VIEWPORT& viewport, UINT width, UINT height)
{
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = static_cast<float>(width);
    viewport.Height = static_cast<float>(height);
    viewport.MinDepth = 0;
    viewport.MaxDepth = 1;
}
//Kallar p� funktionerna
bool SetupD3D11(UINT width, UINT height, HWND window, ID3D11Device*& device, ID3D11DeviceContext*& immidateContext, 
                IDXGISwapChain*& swapChain, ID3D11RenderTargetView*& rtv, ID3D11Texture2D*& dsTexture, ID3D11DepthStencilView*& dsView, D3D11_VIEWPORT& viewport)
{
    if (!CreateInterfaces(width, height, window, device, immidateContext, swapChain))
    {
        std::cerr << "Error creating interfaces!" << std::endl;
        return false;
    }

    if (!CreateRenderTargetView(device, swapChain, rtv))
    {
        std::cerr << "Errer creating rtv!" << std::endl;
        return false;
    }

    if (!CreateDepthStencil(device, width, height, dsTexture, dsView))
    {
        std::cerr << "Error creating depth stencil view" << std::endl;
        return false;
    }

    SetViewPort(viewport, width, height);

    return true;
}

