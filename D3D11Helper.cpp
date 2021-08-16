#include "D3D11Helper.h"

bool CreateInterfaces(UINT width, UINT height, HWND window, ID3D11Device*& device, ID3D11DeviceContext*& immediateContext, IDXGISwapChain*& swapChain)
{
    DXGI_SWAP_CHAIN_DESC swapChainDesc = {}; // Describes a swapchain

    swapChainDesc.BufferDesc.Width = width; // Width
    swapChainDesc.BufferDesc.Height = height; // Height
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 0; // RefreshRate 60 hertz (0 from the beginning) vsync and fullscreen are needed for it to be visible
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM; // 32 bits, 8 per channel
    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // Scanline order is unspecified
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; // Unspecified scaling

    // Default
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;

    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // Use the surface or resource as a result of the rendering
    swapChainDesc.BufferCount = 1; // Double buffer
    swapChainDesc.OutputWindow = window;
    swapChainDesc.Windowed = true;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; // Removes the contents of the backup buffer when it is called on
    swapChainDesc.Flags = 0;

    UINT flags = 0;

#ifdef _DEBUG
	flags = D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 }; //Targets features supported by Direct3D 11.0, including shader model 5.

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr, //IDXGI Adapter
        D3D_DRIVER_TYPE_HARDWARE, // Lets the program select, because we have nullptr on the first parameter.
        nullptr, // The type of driver for the software
        flags, // The runtime layers to enable
        featureLevels, // Pointer to an array that determines the order of future levels created = D3D_FEATURE_LEVEL_11_0
        1, // We call element D3D_FEATURE_LEVEL_11_0
        D3D11_SDK_VERSION, // Standard version used (sdk_version)
        &swapChainDesc, // Swapchain description, pointers
        &swapChain, // Retrieves the swapchain address
        &device, // Retrieves the address of the device
        nullptr, // No definition is needed for this
        &immediateContext// Retrieves the address for immediateContext
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
    textureDesc.Width = width; // Width
    textureDesc.Height = height; // Height
    textureDesc.MipLevels = 1; // Makes the rendering faster, only one is needed for multisampled texture
    textureDesc.ArraySize = 1; // Only one picture
    textureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // A 32-bit z-buffer format that supports 24 bits for depth and 8 bits for stencil.
    textureDesc.SampleDesc.Count = 1; // The number of multisamples per pixel
    textureDesc.SampleDesc.Quality = 0; // No increased quality, unnecessary
    textureDesc.Usage = D3D11_USAGE_DEFAULT; //	A resource that requires read and write access of the GPU
    textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL; // Binds a texture as a depth-stencil target
    textureDesc.CPUAccessFlags = 0; // No CPU access
    textureDesc.MiscFlags = 0; // No more flags are needed

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

bool CreateGbuffer(ID3D11Device* device, GeometryBuffer& gBuffer)
{

    HRESULT hr;
	// Create textures 
    D3D11_TEXTURE2D_DESC TextureGbufferDesc;
    ZeroMemory(&TextureGbufferDesc, sizeof(TextureGbufferDesc));

    TextureGbufferDesc.Width = gBuffer.screenWidth;
    TextureGbufferDesc.Height = gBuffer.screenHeight;
    TextureGbufferDesc.MipLevels = 1;														
    TextureGbufferDesc.ArraySize = 1;
    TextureGbufferDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    TextureGbufferDesc.SampleDesc.Count = 1;
    TextureGbufferDesc.Usage = D3D11_USAGE_DEFAULT;											// GPU READ WRITE
    TextureGbufferDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;	// Render target and Shader resource to write to and use in a shader
    TextureGbufferDesc.CPUAccessFlags = 0;
    TextureGbufferDesc.MiscFlags = 0;

    for (int i = 0; i < gBuffer.NROFBUFFERS; i++)
    {
        hr = device->CreateTexture2D(&TextureGbufferDesc, nullptr, &gBuffer.gBufferTexture[i]);
        if (FAILED(hr))
        {
            return false;
        }
    }

	// Create RTVs
    D3D11_RENDER_TARGET_VIEW_DESC rtvGbufferDesc;
    ZeroMemory(&rtvGbufferDesc, sizeof(rtvGbufferDesc));
    rtvGbufferDesc.Format = TextureGbufferDesc.Format;										// Same format as the texture ( could just type it out but.. )
    rtvGbufferDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;							// Will be used as a Texture2D
    rtvGbufferDesc.Texture2D.MipSlice = 0;									

    for (int i = 0; i < gBuffer.NROFBUFFERS; i++)
    {
        hr = device->CreateRenderTargetView(gBuffer.gBufferTexture[i], &rtvGbufferDesc, &gBuffer.gBuffergBufferRtv[i]);
        if (FAILED(hr))
        {
            return false;
        }
    }


	// Create Shader Resource Views
    D3D11_SHADER_RESOURCE_VIEW_DESC srvGbufferDesc;
    ZeroMemory(&srvGbufferDesc, sizeof(srvGbufferDesc));
    srvGbufferDesc.Format = TextureGbufferDesc.Format;										// Same format as the texture ( could just type it out but.. )
    srvGbufferDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;							
    srvGbufferDesc.Texture2D.MostDetailedMip = 0;
    srvGbufferDesc.Texture2D.MipLevels = 1;													// Lowest it can be, defaulted to -1 ( not possible )

    for (int i = 0; i < gBuffer.NROFBUFFERS; i++)
    {
        hr = device->CreateShaderResourceView(gBuffer.gBufferTexture[i], &srvGbufferDesc, &gBuffer.gBufferSrv[i]);
        if (FAILED(hr))
        {
            return false;
        }

    }
    return true;

}

// Calls the various functions
bool SetupD3D11(UINT width, UINT height, HWND window, ID3D11Device*& device, ID3D11DeviceContext*& immidateContext, 
                IDXGISwapChain*& swapChain, ID3D11RenderTargetView*& rtv, ID3D11Texture2D*& dsTexture, ID3D11DepthStencilView*& dsView, D3D11_VIEWPORT& viewport, GeometryBuffer& gBuffer)
{
    if (!CreateInterfaces(width, height, window, device, immidateContext, swapChain))
    {
        std::cerr << "Error creating interfaces!" << std::endl;
        return false;
    }

	// New function for deferred rendering
    if (!CreateGbuffer(device, gBuffer))
    {
        OutputDebugString(L"Failed to create Gbuffer!");
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

