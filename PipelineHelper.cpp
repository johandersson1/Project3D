//#define STB_IMAGE_IMPLEMENTATION
#include "Image.h"
#include <DirectXMath.h>
#include "PipelineHelper.h"
#include <vector>
#include <fstream>
#include <string>
#include <iostream>


bool LoadShadersGbuffer(ID3D11Device* device, ID3D11PixelShader*& pShaderDeferred, ID3D11VertexShader*& vShaderDeferred, std::string& defVShaderByteCode)
{

	std::string shaderData;
	std::ifstream reader;
	// Clears the reader and shaderData and does the same thing for the PixelShader

	reader.open(L"x64/Debug/VertexShaderDeferred.cso", std::ios::binary | std::ios::ate);
	if (!reader.is_open())
	{
		std::cerr << "Could not open VS file!" << std::endl;
		return false;
	}

	reader.seekg(0, std::ios::end);
	shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
	reader.seekg(0, std::ios::beg);

	shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());

	if (FAILED(device->CreateVertexShader(shaderData.c_str(), shaderData.length(), nullptr, &vShaderDeferred)))
	{
		std::cerr << "Failed to create pixel shader!" << std::endl;
		return false;
	}

	defVShaderByteCode = shaderData;

	shaderData.clear();
	reader.close();
	reader.open(L"x64/Debug/PixelShaderDeferred.cso", std::ios::binary | std::ios::ate);
	if (!reader.is_open())
	{
		std::cerr << "Could not open PS file!" << std::endl;
		return false;
	}

	reader.seekg(0, std::ios::end);
	shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
	reader.seekg(0, std::ios::beg);

	shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());

	if (FAILED(device->CreatePixelShader(shaderData.c_str(), shaderData.length(), nullptr, &pShaderDeferred)))
	{
		std::cerr << "Failed to create pixel shader!" << std::endl;
		return false;
	}
	reader.close();
	return true;
}

bool LoadShadersLight(ID3D11Device* device, ID3D11PixelShader*& lightPShaderDeferred, ID3D11VertexShader*& lightVSShaderDeferred, std::string& lightVShaderByteCode)
{

	std::string shaderData;
	std::ifstream reader;
	// Clears the reader and shaderData and does the same thing for the PixelShader

	reader.open(L"x64/Debug/VertexShaderDeferredLight.cso", std::ios::binary | std::ios::ate);
	if (!reader.is_open())
	{
		std::cerr << "Could not open VS file!" << std::endl;
		return false;
	}

	reader.seekg(0, std::ios::end);
	shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
	reader.seekg(0, std::ios::beg);

	shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());

	if (FAILED(device->CreateVertexShader(shaderData.c_str(), shaderData.length(), nullptr, &lightVSShaderDeferred)))
	{
		std::cerr << "Failed to create pixel shader!" << std::endl;
		return false;
	}

	lightVShaderByteCode = shaderData;

	shaderData.clear();
	reader.close();
	reader.open(L"x64/Debug/PixelShaderDeferredLight.cso", std::ios::binary | std::ios::ate);
	if (!reader.is_open())
	{
		std::cerr << "Could not open PS file!" << std::endl;
		return false;
	}

	reader.seekg(0, std::ios::end);
	shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
	reader.seekg(0, std::ios::beg);

	shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());

	if (FAILED(device->CreatePixelShader(shaderData.c_str(), shaderData.length(), nullptr, &lightPShaderDeferred)))
	{
		std::cerr << "Failed to create pixel shader!" << std::endl;
		return false;
	}

	return true;

}
//Funktion för att beskriva input-buffer data
bool CreateInputLayout(ID3D11Device* device, ID3D11InputLayout*& inputLayout, const std::string& defVShaderByteCode)
{
	// Array of elements with a description
	D3D11_INPUT_ELEMENT_DESC inputDesc[] =
    {   
		// name, index, format, slot, byteOffset, inputSlotClass, stepRate
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}, // Input data is per-vertex data.
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	    {"WORLDPOS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,0 }
    };
	// Important! Everything needs to be in the right order because if it is loaded in the wrong order, it will not work
    HRESULT hr = device->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), defVShaderByteCode.c_str(), defVShaderByteCode.length(), &inputLayout);

    return !FAILED(hr);
}
// VertexBuffer
bool CreateVertexBuffer(ID3D11Device* device, ID3D11Buffer*& vertexBuffer)
{
	Vertex2 TheQuad[] =
	{
	 { {0.5f, 0.5f, 0.0f}, {1.0f, 0.0f}, {0,0,-1 }},
	 
	 { {0.5f, -0.5f, 0.0f}, {1.0f, 1.0f}, {0,0,-1 }},
	 
	 { {-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f}, {0,0,-1 }},
	
	 { {-0.5f, -0.5f, 0.0f}, {0.0f, 1.0f}, {0,0,-1 }}
	 // (X, Y, Z), UVs, Normals
	};

	// Description of the buffer being created
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.ByteWidth = sizeof(TheQuad); // Bytesize 
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE; // Only GPU can read, can not write, CPU does not access at all, does not change after it has been started
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // Binds the buffer as a Vertex buffer
	bufferDesc.CPUAccessFlags = 0; // Is set to 0 because we use immutable, no more info is needed
	bufferDesc.MiscFlags = 0; // No need for additional flags
	bufferDesc.StructureByteStride = 0; // Defines the size of each element in buffer, not needed

	D3D11_SUBRESOURCE_DATA data; // Specify the data for initializing a subresource
	data.pSysMem = TheQuad; // Pointer to TheQuad, initialization data
	data.SysMemPitch = 0; // The distance between the beginning of a line in a texture to the next line, in bytes
	data.SysMemSlicePitch = 0; // The distance (in bytes) from the beginning of one depth level to the next


	HRESULT hr = device->CreateBuffer(&bufferDesc, &data, &vertexBuffer);
	return !FAILED(hr);
}
bool CreateRenderTargetMesh(ID3D11Device* device, ID3D11Buffer*& renderTargetMesh)
{
	Vertex2 rtvMeshData[] =
	{
		// Top Right
	   { {1.0f, 1.0f, 0.0f}, {1.0f, 0.0f}, {0, 0, -1} },
	   // Bottom Right
	   { {1.0f, -1.0f, 0.0f}, {1.0f, 1.0f}, {0, 0, -1}  },
	   // Top Left
	   { {-1.0f, 1.0f, 0.0f}, {0.0f, 0.0f}, {0, 0, -1}  },
	   // Bottom Left
	   { {-1.0f, -1.0f, 0.0f}, {0.0f, 1.0f}, {0, 0, -1}  }
	   // (X, Y, Z), UVs, Normals
	};

	D3D11_BUFFER_DESC rtvMeshDesc;
    rtvMeshDesc.ByteWidth = sizeof(rtvMeshData); // Bytesize of the vertices
    rtvMeshDesc.Usage = D3D11_USAGE_IMMUTABLE; // Usage_Immutable -- Only read by the GPU, not accessable by the CPU. Not changeable after creation
    rtvMeshDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // Binds the buffer as a Vertex Buffer. 
    rtvMeshDesc.CPUAccessFlags = 0; // Set to 0 since the usage is Immutable. NO need to write or read since we have no access.
    rtvMeshDesc.MiscFlags = 0; // No need for additional flags
    rtvMeshDesc.StructureByteStride = 0; // Defines the size of each element in the buffer structure. No need for this buffer

    // Specifies data for initializing a subresource
    D3D11_SUBRESOURCE_DATA data;
    data.pSysMem = &rtvMeshData; // Pointer to the initialization data
    data.SysMemPitch = 0; // The distance (in bytes) from the beginning of one line of a texture to the next line
    data.SysMemSlicePitch = 0; // The distance (in bytes) from the beginning of one depth level to the next. (Used only for 3D-textures)

    HRESULT hr = device->CreateBuffer(&rtvMeshDesc, &data, &renderTargetMesh);

	return !FAILED(hr);
}

bool RenderMeshInputLayout(ID3D11Device* device, ID3D11InputLayout*& renderTargetMeshInputLayout, const std::string& lightVShaderByteCode)
{
    D3D11_INPUT_ELEMENT_DESC inputDesc[] =
    {
        // Name, Index, Format, Slot, ByteOffset, InputSlotClass, StepRate
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},

        // These need to be in the right order for the data to be placed at the right location
    };

    HRESULT hr = device->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), lightVShaderByteCode.c_str(), lightVShaderByteCode.length(), &renderTargetMeshInputLayout);

    return !FAILED(hr);
}
// Function to create constant buffer used to update the square ("TheQuad")
bool CreateConstantBuffer(ID3D11Device* device, ID3D11Buffer*& constantBuffers)
{
	WVP Rotation;

	D3D11_BUFFER_DESC constantBufferDesc;
	constantBufferDesc.ByteWidth = sizeof(WVP); // Bytesize 
	constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC; // Becomes available for both GPU (read only) and CPU (write only) use Map
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER; // Binds a buffer as a constant buffer to a shader stage
	constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // The resource must be mappable so that the CPU can change the contents
	constantBufferDesc.MiscFlags = 0; // No need for additional flags
	constantBufferDesc.StructureByteStride = 0; // Defines the size of each element in buffer, not needed
	 
	D3D11_SUBRESOURCE_DATA TheData;
	TheData.pSysMem = &Rotation; // Pointer to the initialization data
	TheData.SysMemPitch = 0; // The distance between the beginning of a line in a texture to the next line, in bytes
	TheData.SysMemSlicePitch = 0; // The distance (in bytes) from the beginning of one depth level to the next

	HRESULT hr = device->CreateBuffer(&constantBufferDesc, &TheData, &constantBuffers);

	return !FAILED(hr);
}

//bool createRasterizerState(ID3D11Device*& device, ID3D11RasterizerState*& rasterizerState)
//{
//	D3D11_RASTERIZER_DESC desc = {};
//	desc.FillMode = D3D11_FILL_SOLID;
//	desc.CullMode = D3D11_CULL_NONE;
//	desc.AntialiasedLineEnable = TRUE;
//	desc.MultisampleEnable = FALSE;
//	HRESULT hr = device->CreateRasterizerState(&desc, &rasterizerState);
//	return !FAILED(hr);
//}

bool CreateSamplerState(ID3D11Device* device, ID3D11SamplerState*& sampler, ID3D11SamplerState*& clampSampler)
{
	D3D11_SAMPLER_DESC desc;
	desc.Filter = D3D11_FILTER_ANISOTROPIC; // Improve image quality
	// adds the texture for each u, v for each intersection with integers
	// if the u value is 3, the texture is repeated three times
	desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;  										
	desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.MipLODBias = 0; // No offset from the mipmap
	desc.MaxAnisotropy = 16;// The number of samples that can be taken to improve the quality
	desc.BorderColor[0] = desc.BorderColor[1] = desc.BorderColor[2] = desc.BorderColor[3] = 0; // Puts a color for the edge
	desc.MinLOD = 0; // Lower end of the mipmap range to clamp access to, where 0 is the largest and most detailed mipmap level 
	desc.MaxLOD = D3D11_FLOAT32_MAX; 
	// Upper end of the mipmap range to clamp access to, 
	// where 0 is the largest and most detailed mipmap level and any level higher than that is less detailed

	HRESULT hr = device->CreateSamplerState(&desc, &sampler);
	if (FAILED(hr))
		return false ;

	desc.Filter = D3D11_FILTER_ANISOTROPIC; 
	desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.MipLODBias = 0; 
	desc.MaxAnisotropy = 16;
	desc.BorderColor[0] = desc.BorderColor[1] = desc.BorderColor[2] = desc.BorderColor[3] = 0; 
	desc.MinLOD = 0; 
	desc.MaxLOD = D3D11_FLOAT32_MAX;
	
	hr = device->CreateSamplerState(&desc, &clampSampler);
	if (FAILED(hr))
		return false;

	return !FAILED(hr);
}
// CreateTexture funktionen
bool CreateTexture(ID3D11Device* device, ID3D11Texture2D*& texture, ID3D11ShaderResourceView*& textureSRV)
{
	// Width, height, and rgb
	int textureWidth = 512;
	int textureHeight = 512;
	int channels = 3;

	unsigned char* theImageData = stbi_load("wiz.jpg", &textureWidth, &textureHeight, &channels, 4);
	std::vector<unsigned char> textureData;
	textureData.resize(textureWidth * textureHeight * 4);
	// Goes through for each pixel and assigns the color depending on the image from TheImageData
	for (int h = 0; h < textureHeight; ++h)
	{
		for (int w = 0; w < textureWidth; ++w)
		{
			unsigned int pos0 = w * 4 + textureWidth * 4 * h;
			textureData[pos0 + 0] = theImageData[pos0 + 0];
			textureData[pos0 + 1] = theImageData[pos0 + 1];
			textureData[pos0 + 2] = theImageData[pos0 + 2];
			textureData[pos0 + 3] = theImageData[pos0 + 3];
		}
	}

	D3D11_TEXTURE2D_DESC desc;
	desc.Width = textureWidth; // Width
	desc.Height = textureHeight; // Height
	desc.MipLevels = 1; // Makes the rendering faster, only one is needed for multisampled texture
	desc.ArraySize = 1; // Only one picture
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // RGBA
	desc.SampleDesc.Count = 1; // The number of multisamples per pixel
	desc.SampleDesc.Quality = 0; // No increased quality, unnecessary
	desc.Usage = D3D11_USAGE_IMMUTABLE; // Only GPU can read, can not write, CPU does not access at all, does not change after it has been started
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE; // Binds it as a shader
	desc.CPUAccessFlags = 0; // no CPU access
	desc.MiscFlags = 0; // No more flags are needed

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &textureData[0];// Pointer to the initialization data
	data.SysMemPitch = textureWidth * 4; // The distance between the beginning of a line in a texture to the next line, in bytes
	data.SysMemSlicePitch = 0; // The distance (in bytes) from the beginning of one depth level to the next.

	if (FAILED(device->CreateTexture2D(&desc, &data, &texture)))
	{
		std::cerr << "Failed to create texture!" << std::endl;
		return false;
	}
	HRESULT hr = device->CreateShaderResourceView(texture, nullptr, &textureSRV);
	return !FAILED(hr);

	//delete[] theImageData; //delete 
}

bool SetUpRasterizer(ID3D11Device*& device, ID3D11RasterizerState*& rasterizerStateWireFrame, ID3D11RasterizerState*& rasterizerStateSolid)
{
	D3D11_RASTERIZER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
	desc.CullMode = D3D11_CULL_NONE;
	desc.FrontCounterClockwise = false;
	desc.DepthBias = 0;
	desc.DepthBiasClamp = 0;
	desc.SlopeScaledDepthBias = 0;
	desc.DepthClipEnable = true;
	desc.ScissorEnable = false;
	desc.MultisampleEnable = false;
	desc.AntialiasedLineEnable = false;
	HRESULT hr = device->CreateRasterizerState(&desc, &rasterizerStateWireFrame);
	if (FAILED(hr))
	{
		std::cout << "Failed to create wireframe" << std::endl;	
	}
	desc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	hr = device->CreateRasterizerState(&desc, &rasterizerStateSolid);
	if (FAILED(hr))
	{
		std::cout << "Failed to create fill solid" << std::endl;
	}


	return !FAILED(hr);
}

// Calls the various functions
bool SetupPipeline(ID3D11Device* device, ID3D11Buffer*& vertexBuffer, ID3D11InputLayout*& inputLayout, ID3D11Buffer*& constantBuffers,
	ID3D11Texture2D*& texture, ID3D11ShaderResourceView*& textureSRV, ID3D11SamplerState*& sampler, 
	ID3D11PixelShader*& pShaderDeferred, ID3D11VertexShader*& vShaderDeferred, ID3D11PixelShader*& lightPShaderDeferred,
	ID3D11VertexShader*& lightVShaderDeferred, ID3D11InputLayout*& renderTargetMesh, ID3D11Buffer*& screenQuadMesh, 
	ID3D11RasterizerState*& rasterizerStateWireFrame, ID3D11RasterizerState*& rasterizerStateSolid, ID3D11SamplerState*& clampSampler)
{
	std::string vShaderByteCode;
	std::string defVShaderByteCode;
	std::string lightVShaderByteCode;

	if (!LoadShadersGbuffer(device, pShaderDeferred, vShaderDeferred, defVShaderByteCode))
	{
		std::cerr << "Error loading gbuffer!" << std::endl;
		return false;
	}

	if (!LoadShadersLight(device, lightPShaderDeferred, lightVShaderDeferred, lightVShaderByteCode))
	{
		std::cerr << "Error loading light!" << std::endl;
		return false;
	}

	if (!CreateRenderTargetMesh(device, screenQuadMesh))
    {
        OutputDebugString(L"Failed to create screensized quad!");
        return false;
    }

	if (!RenderMeshInputLayout(device, renderTargetMesh, lightVShaderByteCode)) 
    {
        std::cerr << "Error creating input layout!" << std::endl;
        return false;
    }

	//if (!/*LoadShaders(device, vShader, pShader, vShaderByteCode))*/
	//{
	//	std::cerr << "Error loading shaders!" << std::endl;
	//	return false;
	//}

	if (!CreateInputLayout(device, inputLayout, defVShaderByteCode))
	{
		std::cerr << "Error creating input layout!" << std::endl;
		return false;
	}

	if (!CreateVertexBuffer(device, vertexBuffer))
	{
		std::cerr << "Error creating vertex buffer!" << std::endl;
		return false;
	}

	if (!CreateConstantBuffer(device, constantBuffers))
	{
		std::cerr << "Error creating constant buffer!" << std::endl;
		return false;
	}

	if (!CreateTexture(device, texture, textureSRV))
	{
		std::cerr << "Error creating texture or srv!" << std::endl;
		return false;
	}

	if (!CreateSamplerState(device, sampler, clampSampler))
	{
		std::cerr << "Error creating sampler state! " << std::endl;
		return false;
	}
	if(!SetUpRasterizer(device, rasterizerStateWireFrame, rasterizerStateSolid))
	{
		std::cerr << "Error creating rasterizer state! " << std::endl;
		return false;
	}
}
