#include <DirectXMath.h>
#include "WindowHelper.h"
#include "D3D11Helper.h"
#include "PipelineHelper.h"
#include <chrono>
#include <iostream>
#include "Camera.h"

//Console Setup
#include<io.h>
#include<fcntl.h>


void RedirectIOToConsole()
{
	AllocConsole();
	HANDLE stdHandle;
	int hConsole;
	FILE* fp;
	stdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	hConsole = _open_osfhandle((intptr_t)stdHandle, _O_TEXT);
	fp = _fdopen(hConsole, "w");

	freopen_s(&fp, "CONOUT$", "w", stdout);
}

void SetUpLight(ID3D11Device* device, ID3D11Buffer* &lightConstantBuffer, Light &lighting)
{
	lighting.lightColour = DirectX::XMFLOAT3{ 1.0f, 1.0f, 1.0f };
	lighting.lightAmbient = DirectX::XMFLOAT3{ 0.5f, 0.5f, 0.5f };
	lighting.lightDiffuse = DirectX::XMFLOAT3{ 1.0f, 1.0f, 1.0f };
	lighting.lightCamPos = DirectX::XMFLOAT3{ 0.0f, 0.0f, -5.0f }; //ljusinställningar 

	D3D11_BUFFER_DESC cbLight;
	ZeroMemory(&cbLight, sizeof(D3D11_BUFFER_DESC)); //nollställer
	cbLight.ByteWidth = sizeof(lighting); //Bytesize 
	cbLight.Usage = D3D11_USAGE_DYNAMIC; //blir tillgänglig för både GPU(read only) och CPU(write only) använd Map.
	cbLight.BindFlags = D3D11_BIND_CONSTANT_BUFFER; //Binder en buffert som en constantbuffert till ett shader stage
	cbLight.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;  //resursen ska vara mappable så att CPU kan ändra innehållet
	cbLight.MiscFlags = 0; //används inte

	device->CreateBuffer(&cbLight, nullptr, &lightConstantBuffer);
}

void clearView(ID3D11DeviceContext* immediateContext, ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsView)
{
	//Background
	float clearColour[4] = { 0.5f, 1.0f, 1.0f,1 };
	immediateContext->ClearRenderTargetView(rtv, clearColour);
	immediateContext->ClearDepthStencilView(dsView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
}

void update(ID3D11DeviceContext* immediateContext , XMMATRIX &worldSpace, XMMATRIX &theRotation, XMMATRIX arbitraryPoint, //Ändrat så at vi gör alla uträkningar i update istället för i olika funktioner
		XMMATRIX translation, float &Rotation, float RotationAmount, std::chrono::duration<float> TheDeltaTime, 
		Camera& camera, ID3D11Buffer* constantBuffers, Light& lighting)
{
	//Rotation += RotationAmount * TheDeltaTime.count();
 //    if (Rotation >= DirectX::XM_PI * 2)
 //    {
	//	Rotation -= DirectX::XM_PI * 2;
 //    }
	//theRotation = arbitraryPoint * DirectX::XMMatrixRotationY(Rotation); //XMMatrixRotationY = Bygger en matris som roterar runt y-axeln.
	//worldSpace = theRotation * translation; //matris * matristranslation = worldspace

	XMMATRIX scalingMatrix = XMMatrixScaling(1, 1, 1);
	XMMATRIX rotationMatrix = XMMatrixRotationX(0) * DirectX::XMMatrixRotationY(0) * DirectX::XMMatrixRotationZ(0);
	XMMATRIX translationMatrix = DirectX::XMMatrixTranslation(0, 0, 0);
	XMMATRIX worldMatrix = scalingMatrix * rotationMatrix * translationMatrix;
	
	XMMATRIX viewMatrix = XMMatrixLookToLH(DirectX::XMVectorSet(camera.getCameraPos().x, camera.getCameraPos().y, camera.getCameraPos().z, 0), DirectX::XMVectorSet(camera.getCameraDir().x, camera.getCameraDir().y, camera.getCameraDir().z, 0), DirectX::XMVectorSet(0, 1, 0, 0));
	XMMATRIX worldViewProj = XMMatrixTranspose(worldMatrix * viewMatrix * camera.cameraProjection);

	immediateContext->VSSetConstantBuffers(0, 1, &constantBuffers);
	D3D11_MAPPED_SUBRESOURCE dataBegin;
	HRESULT hr = immediateContext->Map(constantBuffers, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &dataBegin);
	if (SUCCEEDED(hr))
	{
		memcpy(dataBegin.pData, &worldViewProj, sizeof(XMMATRIX));
		immediateContext->Unmap(constantBuffers, NULL);
	}
	else
	{
		std::cerr << "Failed to update ConstantBuffer (update function)" << std::endl;
	}
	lighting.lightCamPos = camera.getCameraPos();
}


void RenderGBufferPass(ID3D11DeviceContext* immediateContext, ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsView, D3D11_VIEWPORT& viewport,
	ID3D11PixelShader* pShaderDeferredRender, ID3D11VertexShader* vShaderDeferred,
	ID3D11InputLayout* inputLayout, ID3D11SamplerState* sampler, GeometryBuffer gBuffer,
	ID3D11ShaderResourceView* textureSRV, ID3D11Buffer* vertexBuffer)
{
	//D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST

	clearView(immediateContext, rtv, dsView);
	float clearcolor[4] = { 0.75f,0.75f,0.3f,0 };
	immediateContext->ClearRenderTargetView(gBuffer.gBuffergBufferRtv[0], clearcolor);
	immediateContext->ClearRenderTargetView(gBuffer.gBuffergBufferRtv[1], clearcolor);
	immediateContext->ClearRenderTargetView(gBuffer.gBuffergBufferRtv[2], clearcolor);
	immediateContext->ClearRenderTargetView(gBuffer.gBuffergBufferRtv[3], clearcolor);
	

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	immediateContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	//immediateContext->IASetVertexBuffers(1, 2, &groundBuffer, &stride, &offset);
	immediateContext->IASetInputLayout(inputLayout);
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//immediateContext->HSSetShader(hshader, nullptr, 0);
	//immediateContext->DSSetShader(dshader, nullptr, 0);
	//immediateContext->RSSetState(rasterizerState);
	immediateContext->VSSetShader(vShaderDeferred, nullptr, 0);
	immediateContext->RSSetViewports(1, &viewport);
	immediateContext->PSSetShader(pShaderDeferredRender, nullptr, 0);
	/*immediateContext->PSSetShaderResources(0, 1, gBuffer.gBufferSrv);*/
	immediateContext->PSSetShaderResources(0, 1, &textureSRV);
	immediateContext->PSSetSamplers(0, 1, &sampler);
	immediateContext->OMSetRenderTargets(gBuffer.NROFBUFFERS, gBuffer.gBuffergBufferRtv, dsView);
	immediateContext->Draw(4, 0);

	//test
	//Clean up
	ID3D11RenderTargetView* nullArr[gBuffer.NROFBUFFERS];
	for (int i = 0; i < gBuffer.NROFBUFFERS; i++)
	{
		nullArr[i] = nullptr;
	}
	immediateContext->OMSetRenderTargets(gBuffer.NROFBUFFERS, nullArr, dsView);
}

void RenderLightPass(ID3D11DeviceContext* immediateContext, ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsView, D3D11_VIEWPORT& viewport,
	ID3D11PixelShader* pShaderDeferredRender,ID3D11VertexShader* vShaderDeferred, ID3D11InputLayout* inputLayout, ID3D11Buffer* vertexBuffer,
	ID3D11ShaderResourceView* textureSRV, ID3D11SamplerState* sampler, GeometryBuffer gBuffer, ID3D11PixelShader* lightPShaderDeferred, 
	ID3D11VertexShader* lightVShaderDeferred, ID3D11InputLayout* renderTargetMeshInputLayout, ID3D11Buffer* screenQuadMesh)
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	immediateContext->IASetVertexBuffers(0, 1, &screenQuadMesh, &stride, &offset);
	immediateContext->IASetInputLayout(renderTargetMeshInputLayout);
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	immediateContext->VSSetShader(lightVShaderDeferred, nullptr, 0);
	//immediateContext->HSSetShader(nullptr, nullptr, 0);
	//immediateContext->DSSetShader(nullptr, nullptr, 0);
	immediateContext->RSSetViewports(1, &viewport);
	immediateContext->PSSetShader(lightPShaderDeferred, NULL, 0);
	immediateContext->PSSetShaderResources(0, gBuffer.NROFBUFFERS, gBuffer.gBufferSrv);
	immediateContext->PSSetSamplers(0, 1, &sampler);
	immediateContext->RSSetState(nullptr);
	immediateContext->OMSetRenderTargets(1, &rtv, dsView);
	immediateContext->Draw(4, 0);

	//Clean up
	ID3D11ShaderResourceView* nullArr[gBuffer.NROFBUFFERS];
	for (int i = 0; i < gBuffer.NROFBUFFERS; i++)
	{
		nullArr[i] = nullptr;
	}
	immediateContext->PSSetShaderResources(0, gBuffer.NROFBUFFERS, nullArr);
}

void Render(ID3D11DeviceContext* immediateContext, ID3D11RenderTargetView* rtv,
	ID3D11DepthStencilView* dsView, D3D11_VIEWPORT& viewport, ID3D11VertexShader* vShader,
	ID3D11PixelShader* pShader, ID3D11InputLayout* inputLayout, ID3D11Buffer* vertexBuffer,
	ID3D11ShaderResourceView* textureSRV, ID3D11SamplerState* sampler, ID3D11Buffer* constantBuffers, 
	ID3D11Buffer* lightConstantBuffer,DirectX::XMMATRIX worldSpace, WVP &imageCamera, Light &lighting)
{
	float clearColour[4] = { 0.5f, 0.5f, 0.5f, 1.0f };

	//D3D11_MAPPED_SUBRESOURCE databegin;
	//HRESULT hr = immediateContext->Map(constantBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &databegin);
	//if (SUCCEEDED(hr))
	//{
	//	memcpy(databegin.pData, &imageCamera, sizeof(WVP));
	//	immediateContext->Unmap(constantBuffer, NULL);
	//}
	//else
	//{
	//	OutputDebugString((L"Failed to update ConstantBuffer"));
	//}

	D3D11_MAPPED_SUBRESOURCE databegin2;
	HRESULT hr1 = immediateContext->Map(lightConstantBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &databegin2);
	if (SUCCEEDED(hr1))
	{
		memcpy(databegin2.pData, &lighting, sizeof(Light));
		immediateContext->Unmap(lightConstantBuffer, NULL);
	}
	else
	{
		OutputDebugString((L"Failed to update lightConstantBuffer "));
	}
	
	//imageCamera.worldSpace = XMMatrixTranspose(worldSpace); 
	////Transponering av scale ger samma matris
	////Transponering av ren rotation producerar invers som behövs
	////Ange kamerans invers på objekt för att “centrera”
	//imageCamera.worldViewProj = XMMatrixTranspose(worldSpace * cameraPerspective * cameraProjection); //Skapar en ny matris
	
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	immediateContext->ClearRenderTargetView(rtv, clearColour);
	immediateContext->ClearDepthStencilView(dsView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
	immediateContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	immediateContext->IASetInputLayout(inputLayout);
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	immediateContext->VSSetShader(vShader, NULL, 0);
	immediateContext->RSSetViewports(1, &viewport);
	immediateContext->PSSetShader(pShader, NULL, 0);
	immediateContext->PSSetShaderResources(0, 1, &textureSRV);
	immediateContext->PSSetSamplers(0, 1, &sampler);
	immediateContext->OMSetRenderTargets(1, &rtv, dsView);
	immediateContext->PSSetConstantBuffers(0, 1, &lightConstantBuffer);
	immediateContext->VSSetConstantBuffers(0, 1, &constantBuffers);
	immediateContext->Draw(4,0);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	const UINT WIDTH = 1024; //bredd för fönstret 
	const UINT HEIGHT = 1024;//höjd för fönstret 
	HWND window; //A handle to a window.
	RedirectIOToConsole();
	//std::chrono::steady_clock::time_point theDeltaTime = std::chrono::steady_clock::now();

	//Structs from PipelineHelper
	Light lighting;
	WVP imageCamera;

	Camera camera(XMFLOAT3(0,0,-3), XMFLOAT3(0,0,1), 1.0f);

	XMMATRIX cameraPerspective;
	XMMATRIX cameraProjection;
	XMMATRIX theRotation;

	XMMATRIX worldSpace = DirectX::XMMatrixIdentity(); //Identitets matris 
	XMMATRIX translation = DirectX::XMMatrixTranslation(0.0f, 0.0f, 1.0f); //translationsmatris 
	XMMATRIX arbitraryPoint = DirectX::XMMatrixTranslation(0.0f, 0.0f, -2.0f); //translationsmatris 

	//kopplar in kameran
	XMVECTOR cameraPos = DirectX::XMVectorSet(0.0f, 0.0f, 5.0f, 0.0f); //skapar en vector med 4 floats 
	XMVECTOR lookAt = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR upVector = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	ID3D11Device* device; // används för att skapa resurser.
	ID3D11DeviceContext* immediateContext; // genererar renderings kommandon.
	IDXGISwapChain* swapChain;// denna funktion byter back buffern(rtv) och display skärmen.
	ID3D11RenderTargetView* rtv; // denna variabel är en pekare till ett objekt som innehåller all information om renderings objektet.  
	ID3D11DepthStencilView* dsView; // the Depth/Stencil Buffer lagrar depth information för de pixlar som ska renderas.
	D3D11_VIEWPORT viewport; // definierar dimensionerna för en viewport.
	ID3D11Texture2D* dsTexture; // en ID3D11Texture2D är ett objekt som lagrar en platt bild. 
	ID3D11Texture2D* texture;
	ID3D11VertexShader* vShader; // hanterar ett körbart program för att styra vertex-shader-scenen
	ID3D11PixelShader* pShader; // hanterar ett körbart program för att styra pixel-shader-scenen
	ID3D11InputLayout* inputLayout; // information som lagras med varje vertex för att förbättra renderingshastigheten
	ID3D11Buffer* vertexBuffer; // buffert resurs, som är ostrukturerat minne
	ID3D11Buffer* constantBuffer;
	ID3D11Buffer* lightConstantBuffer;
	ID3D11ShaderResourceView* textureSRV; // anger de (sub resources) en shader kan komma åt under rendering
	ID3D11SamplerState* sampler; // sampler-state som du kan bindar till valfritt shader stage i pipelinen.


	//Deferred light
	ID3D11PixelShader* lightPShaderDeferred;
	ID3D11VertexShader* lightVShaderDeferred;

	ID3D11InputLayout* renderTargetMeshInputLayout;
	ID3D11Buffer* screenQuadMesh;

	ID3D11Buffer* groundBuffer;

	//ID3D11Buffer* groundBuffer;

	//Gbuffer
	DirectionalLight defferedPS;
	GeometryBuffer gBuffer;
	ID3D11PixelShader* pShaderDeferred;
	ID3D11VertexShader* vShaderDeferred;

	gBuffer.screenWidth = WIDTH;
	gBuffer.screenHeight = HEIGHT;
	for (int i = 0; i < gBuffer.NROFBUFFERS; i++)
	{
		gBuffer.gBufferTexture[i] = nullptr;
	}


	//ConstantBuffer(s)
	ID3D11Buffer* constantBuffers;

	// tiden
	std::chrono::steady_clock::time_point startTime;
	std::chrono::steady_clock::time_point stopTime;

	float Rotation = 0.5f;
	float theRotationAmount = 0.5f;

	if (!SetupWindow(hInstance, WIDTH, HEIGHT, nCmdShow, window))
	{
		std::cerr << "Failed to setup window!" << std::endl;
		return -1; 
	}

	if (!SetupD3D11(WIDTH, HEIGHT, window, device, immediateContext, 
		swapChain, rtv, dsTexture, dsView, viewport, gBuffer))
	{
		std::cerr << "Failed to setup d3d11!" << std::endl;
		return -2;
	}

	if (!SetupPipeline(device, vertexBuffer, vShader, pShader, inputLayout, 
		constantBuffers, texture, textureSRV, sampler, pShaderDeferred, vShaderDeferred, lightPShaderDeferred, lightVShaderDeferred,
		renderTargetMeshInputLayout, screenQuadMesh))
	{
		std::cerr << "Failed to setup pipeline!" << std::endl;
		return -3;
	}
	SetUpLight(device, lightConstantBuffer, lighting); //kallar på SetUpLight

	MSG msg = {};

	while (!(GetKeyState(VK_ESCAPE) & 0x8000) && msg.message != WM_QUIT)
	{
		const std::chrono::duration <float> TheDeltaTime = stopTime - startTime;
		startTime = std::chrono::steady_clock::now();

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) //if kanske endast visar ett meddelande(while visar alla)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			camera.detectInput(0.003f, 0.005f);
			camera.clean();

			/*Render(immediateContext, rtv, dsView, viewport, vShader, pShader, inputLayout, vertexBuffer, 
					textureSRV, sampler, constantBuffers, lightConstantBuffer, worldSpace, imageCamera, lighting);*/

			RenderGBufferPass(immediateContext, rtv, dsView, viewport, pShaderDeferred, vShaderDeferred, inputLayout,
				sampler, gBuffer, textureSRV, vertexBuffer);

			RenderLightPass(immediateContext, rtv, dsView, viewport, pShaderDeferred, vShaderDeferred, inputLayout, vertexBuffer,
				textureSRV, sampler, gBuffer, lightPShaderDeferred,
				lightVShaderDeferred, renderTargetMeshInputLayout, screenQuadMesh);


			//Kallar på vår renderfunktion
			update(immediateContext, worldSpace, theRotation, arbitraryPoint, translation, Rotation, theRotationAmount, 
		    	   TheDeltaTime, camera, constantBuffers, lighting);

			swapChain->Present(0, 0); //Presents a rendered image to the user.
		}
		stopTime = std::chrono::steady_clock::now();
	}

	constantBuffers->Release();
	texture->Release();
	sampler->Release();
	textureSRV->Release();
	lightConstantBuffer->Release();
	vertexBuffer->Release();
	inputLayout->Release();
	pShader->Release();
	vShader->Release();
	dsTexture->Release();
	dsView->Release();
	rtv->Release();
	swapChain->Release();
	immediateContext->Release();
	device->Release();

	return 0;
}
