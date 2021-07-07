//#define STB_IMAGE_IMPLEMENTATION
#include "Image.h"
#include <DirectXMath.h>
#include "PipelineHelper.h"
#include <vector>
#include <fstream>
#include <string>
#include <iostream>

//Ladda in shaders funktion

//bool LoadShaders(ID3D11Device* device, ID3D11VertexShader*& vShader, ID3D11PixelShader*& pShader, std::string& vShaderByteCode)
//{
//	std::string shaderData;
//	std::ifstream reader;
//	reader.open("VertexShader.cso", std::ios::binary | std::ios::ate); //�ppnar.cso-file 
//	//�ndra till nya!!
//	if (!reader.is_open())
//	{
//		std::cerr << "Could not open VS file!" << std::endl;
//		return false;
//	}
//	reader.seekg(0, std::ios::end); //L�ser in cso filen och h�mtar den totala storleken p� filen
//	shaderData.reserve(static_cast<unsigned int>(reader.tellg())); //S�tter stringen till den minsta till�tna storleken f�r att den ska passa
//	reader.seekg(0, std::ios::beg); //Flyttar tillbaka till b�rjan av .cso filen (beg)
//
//	shaderData.assign((std::istreambuf_iterator<char>(reader)), //Tilldelar datan av parametrarna till shaderData
//	std::istreambuf_iterator<char>());
//
//	if (FAILED(device->CreateVertexShader(shaderData.c_str(), shaderData.length(), nullptr, &vShader))) //fail check 
//	{
//		std::cerr << "Failed to create vertex shader!" << std::endl;
//		return false;
//	}
//	//Rensar upp f�r shaderData och reader sedan utf�r vi samma sak f�r PixelShader
//	vShaderByteCode = shaderData;
//	shaderData.clear();
//	reader.close();
//	reader.open("PixelShader.cso", std::ios::binary | std::ios::ate);
//	//�ndra till nya!!
//	if (!reader.is_open())
//	{
//		std::cerr << "Could not open PS file!" << std::endl;
//		return false;
//	}
//
//	reader.seekg(0, std::ios::end);
//	shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
//	reader.seekg(0, std::ios::beg);
//
//	shaderData.assign((std::istreambuf_iterator<char>(reader)),
//	std::istreambuf_iterator<char>());
//
//	if (FAILED(device->CreatePixelShader(shaderData.c_str(), shaderData.length(), nullptr, &pShader)))
//	{
//		std::cerr << "Failed to create pixel shader!" << std::endl;
//		return false;
//	}
//
//	return true;
//}

bool LoadShadersGbuffer(ID3D11Device* device, ID3D11PixelShader*& pShaderDeferred, ID3D11VertexShader*& vShaderDeferred, std::string& defVShaderByteCode)
{

	std::string shaderData;
	std::ifstream reader;
	//Clears the reader and shaderData and does the same thing for the PixelShader

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

	return true;
}

bool LoadShadersLight(ID3D11Device* device, ID3D11PixelShader*& lightPShaderDeferred, ID3D11VertexShader*& lightVSShaderDeferred, std::string& lightVShaderByteCode)
{

	std::string shaderData;
	std::ifstream reader;
	//Clears the reader and shaderData and does the same thing for the PixelShader

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
//Funktion f�r att beskriva input-buffer data
bool CreateInputLayout(ID3D11Device* device, ID3D11InputLayout*& inputLayout, const std::string& defVShaderByteCode)
{
	//array av element med en beskrivning
	D3D11_INPUT_ELEMENT_DESC inputDesc[] =
    {   
		//name, index, format, slot, byteOffset, inputSlotClass, stepRate
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}, //Input data is per-vertex data.
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	    {"WORLDPOS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,0 }
    };
	//Viktigt!! Allt beh�ver vara i r�tt ordning f�r om det laddas in i fel ordning s� funkar det ej
    HRESULT hr = device->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), defVShaderByteCode.c_str(), defVShaderByteCode.length(), &inputLayout);

    return !FAILED(hr);
}
//vertexbuffern
bool CreateVertexBuffer(ID3D11Device* device, ID3D11Buffer*& vertexBuffer)
{
	//TheQuads vertices  
	Vertex2 TheQuad[] =
	{
	 { {0.5f, 0.5f, 0.0f}, {1.0f, 0.0f}, {0,0,-1 }},
	 
	 { {0.5f, -0.5f, 0.0f}, {1.0f, 1.0f}, {0,0,-1 }},
	 
	 { {-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f}, {0,0,-1 }},
	
	 { {-0.5f, -0.5f, 0.0f}, {0.0f, 1.0f}, {0,0,-1 }}
	 //(x,y,z), uvs, colour, normals
	};

	//beskrivning av buffern som skapas
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.ByteWidth = sizeof(TheQuad); //Bytesize 
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE; //Endast GPU kan l�sa, kan ej skriva, CPU kommer inte �t alls ,�ndras inte efter den har blivit p�b�rjad
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; //binder buffern som en Vertex-buffer
	bufferDesc.CPUAccessFlags = 0; //s�tts till 0 f�r vi anv�nder immutable, beh�vs inte mer info
	bufferDesc.MiscFlags = 0; //inga mer flaggor beh�vs
	bufferDesc.StructureByteStride = 0; //definerar storleken f�r varje element i buffern, beh�vs ej

	D3D11_SUBRESOURCE_DATA data; //specificera datan f�r initialisering av en subresource
	data.pSysMem = TheQuad; //pekare till TheQuad, initialization data.
	data.SysMemPitch = 0; //distansen mellan b�rjan av en rad i en texture till n�sta rad, i bytes
	data.SysMemSlicePitch = 0; // Avst�ndet(i byte) fr�n b�rjan av en djupniv� till n�sta.


	HRESULT hr = device->CreateBuffer(&bufferDesc, &data, &vertexBuffer);
	return !FAILED(hr);
}
bool CreateRenderTargetMesh(ID3D11Device* device, ID3D11Buffer*& renderTargetMesh)
{
	Vertex2 rtvMeshData[] =
	{
		//Top Right
	   { {1.0f, 1.0f, 0.0f}, {1.0f, 0.0f}, {0, 0, -1} },
	   //Bottom Right
	   { {1.0f, -1.0f, 0.0f}, {1.0f, 1.0f}, {0, 0, -1}  },
	   //Top Left
	   { {-1.0f, 1.0f, 0.0f}, {0.0f, 0.0f}, {0, 0, -1}  },
	   //Bottom Left
	   { {-1.0f, -1.0f, 0.0f}, {0.0f, 1.0f}, {0, 0, -1}  }
	   //(X, Y, Z), UVs, Colour, Normals
	};

	D3D11_BUFFER_DESC rtvMeshDesc;
    rtvMeshDesc.ByteWidth = sizeof(rtvMeshData); //Bytesize of the vertices
    rtvMeshDesc.Usage = D3D11_USAGE_IMMUTABLE; //Usage_Immutable -- Only read by the GPU, not accessable by the CPU. Not changeable after creation
    rtvMeshDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; //Binds the buffer as a Vertex Buffer. 
    rtvMeshDesc.CPUAccessFlags = 0; //Set to 0 since the usage is Immutable. NO need to write or read since we have no access.
    rtvMeshDesc.MiscFlags = 0; //No need for additional flags
    rtvMeshDesc.StructureByteStride = 0; //Defines the size of each element in the buffer structure. No need for this buffer

    //Specifies data for initializing a subresource
    D3D11_SUBRESOURCE_DATA data;
    data.pSysMem = &rtvMeshData; //Pointer to the initialization data
    data.SysMemPitch = 0; //The distance (in bytes) from the beginning of one line of a texture to the next line.
    data.SysMemSlicePitch = 0; //The distance (in bytes) from the beginning of one depth level to the next. (Used only for 3D-textures)

    HRESULT hr = device->CreateBuffer(&rtvMeshDesc, &data, &renderTargetMesh);

	return !FAILED(hr);
}

bool RenderMeshInputLayout(ID3D11Device* device, ID3D11InputLayout*& renderTargetMeshInputLayout, const std::string& lightVShaderByteCode)
{
    D3D11_INPUT_ELEMENT_DESC inputDesc[] =
    {
        //Name, Index, Format, Slot, ByteOffset, InputSlotClass, StepRate
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},

        //These need to be in the right order for the data to be placed at the right location
    };

    HRESULT hr = device->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), lightVShaderByteCode.c_str(), lightVShaderByteCode.length(), &renderTargetMeshInputLayout);

    return !FAILED(hr);
}
//funktion f�r att skapa constantbuffer som anv�nds f�r att uppdatera fyrkanten(TheQuad)
bool CreateConstantBuffer(ID3D11Device* device, ID3D11Buffer*& constantBuffers)
{
	WVP Rotation;

	D3D11_BUFFER_DESC constantBufferDesc;
	constantBufferDesc.ByteWidth = sizeof(WVP); //Bytesize 
	constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC; //blir tillg�nglig f�r b�de GPU(read only) och CPU(write only) anv�nd Map.
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER; //Binder en buffert som en constantbuffert till ett shader stage
	constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; //resursen ska vara mappable s� att CPU kan �ndra inneh�llet
	constantBufferDesc.MiscFlags = 0; //inga mer flaggor beh�vs
	constantBufferDesc.StructureByteStride = 0; //definerar storleken f�r varje element i buffern, beh�vs ej

	D3D11_SUBRESOURCE_DATA TheData;
	TheData.pSysMem = &Rotation; //pekare till initialisering datan
	TheData.SysMemPitch = 0; //distansen mellan b�rjan av en rad i en texture till n�sta rad, i bytes
	TheData.SysMemSlicePitch = 0; // Avst�ndet(i byte) fr�n b�rjan av en djupniv� till n�sta.

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

bool CreateSamplerState(ID3D11Device* device, ID3D11SamplerState*& sampler)
{
	D3D11_SAMPLER_DESC desc;
	desc.Filter = D3D11_FILTER_ANISOTROPIC; //f�rb�ttra bildkvaliteten
	//l�gger till texturen f�r varje u,v f�r varje korsning med heltal
	//om u v�rdet �r 3, s� upprepas texturen tre g�nger. 
	desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;  										
	desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.MipLODBias = 0; //ingen offset fr�n mipmapen 
	desc.MaxAnisotropy = 16;//antalet samples som kan tas f�r att f�rb�ttra kvaliten
	desc.BorderColor[0] = desc.BorderColor[1] = desc.BorderColor[2] = desc.BorderColor[3] = 0; //s�tter en f�rg f�r kanten
	desc.MinLOD = 0; //Lower end of the mipmap range to clamp access to, where 0 is the largest and most detailed mipmap level 
	desc.MaxLOD = D3D11_FLOAT32_MAX; 
	//Upper end of the mipmap range to clamp access to, 
	//where 0 is the largest and most detailed mipmap level and any level higher than that is less detailed

	HRESULT hr = device->CreateSamplerState(&desc, &sampler);
	return !FAILED(hr);
}
// CreateTexture funktionen
bool CreateTexture(ID3D11Device* device, ID3D11Texture2D*& texture, ID3D11ShaderResourceView*& textureSRV)
{
	//bredd, h�jd, och rgb
	int textureWidth = 512;
	int textureHeight = 512;
	int channels = 3;
	//laddar in bilden fr�n Debug folder
	unsigned char* theImageData = stbi_load("wiz.jpg", &textureWidth, &textureHeight, &channels, 4);
	std::vector<unsigned char> textureData;
	textureData.resize(textureWidth * textureHeight * 4);
	//g�r igenom f�r varje pixel och tilldelar f�rgen beroende p� bilden fr�n TheImageData
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
	desc.Width = textureWidth; //bredd
	desc.Height = textureHeight; //h�jd
	desc.MipLevels = 1; //g�r att renderingen blir snabbare, beh�vs endast en f�r multisampled texture
	desc.ArraySize = 1; //endast en bild
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //RGBA
	desc.SampleDesc.Count = 1; //Antalet multisamples per pixel
	desc.SampleDesc.Quality = 0; //ingen �kat kvalit�, on�digt 
	desc.Usage = D3D11_USAGE_IMMUTABLE; //Endast GPU kan l�sa, kan ej skriva, CPU kommer inte �t alls ,�ndras inte efter den har blivit p�b�rjad
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE; //binder den som shader
	desc.CPUAccessFlags = 0; //ingen CPU �tkomst
	desc.MiscFlags = 0; //beh�vs inga mer flaggor

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &textureData[0];//pekare till initialisering datan
	data.SysMemPitch = textureWidth * 4; //distansen mellan b�rjan av en rad i en texture till n�sta rad, i bytes
	data.SysMemSlicePitch = 0; // Avst�ndet(i byte) fr�n b�rjan av en djupniv� till n�sta.

	if (FAILED(device->CreateTexture2D(&desc, &data, &texture)))
	{
		std::cerr << "Failed to create texture!" << std::endl;
		return false;
	}
	HRESULT hr = device->CreateShaderResourceView(texture, nullptr, &textureSRV);
	return !FAILED(hr);

	//delete[] theImageData; //delete 
}
//Anv�nds ej, kan kopplas in f�r att visa baksidan av texturen, och �ven visa wireframe
//bool SetUpRasterizer(ID3D11Device*& device, ID3D11RasterizerState*& rasterizerState)
//{
//	D3D11_RASTERIZER_DESC desc = {};
//	desc.FillMode = D3D11_FILL_SOLID;
//	desc.CullMode = D3D11_CULL_NONE;
//	desc.AntialiasedLineEnable = TRUE; //Set to TRUE to use the quadrilateral line anti-aliasing algorithm 
//									   //and to FALSE to use the alpha line anti-aliasing algorithm
//
//	desc.MultisampleEnable = FALSE;    //Specifies whether to enable line antialiasing; only applies 
//	                                   //if doing line drawing and MultisampleEnable is FALSE.
//
//	HRESULT hr = device->CreateRasterizerState(&desc, &rasterizerState);
//	return !FAILED(hr);
//}

//Kallar p� de olika funktionerna
bool SetupPipeline(ID3D11Device* device, ID3D11Buffer*& vertexBuffer, ID3D11InputLayout*& inputLayout, ID3D11Buffer*& constantBuffers,
	ID3D11Texture2D*& texture, ID3D11ShaderResourceView*& textureSRV, ID3D11SamplerState*& sampler, 
	ID3D11PixelShader*& pShaderDeferred, ID3D11VertexShader*& vShaderDeferred, ID3D11PixelShader*& lightPShaderDeferred,
	ID3D11VertexShader*& lightVShaderDeferred, ID3D11InputLayout*& renderTargetMesh, ID3D11Buffer*& screenQuadMesh)
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

	if (!CreateSamplerState(device, sampler))
	{
		std::cerr << "Error creating sampler state! " << std::endl;
		return false;
	}
}
