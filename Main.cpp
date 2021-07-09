#include <DirectXMath.h>
#include <iostream>
#include "Timer.h"
#include "Camera.h"
#include "WindowHelper.h"
#include "D3D11Helper.h"
#include "PipelineHelper.h"
#include "ParticleSystemRenderer.h"
#include "ModelRenderer.h"
#include "TerrainRenderer.h"

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

//void SetUpLight(ID3D11Device* device, ID3D11Buffer* &lightConstantBuffer, Light &lighting)
//{
//	lighting.lightColour = DirectX::XMFLOAT3{ 1.0f, 1.0f, 1.0f };
//	lighting.lightAmbient = DirectX::XMFLOAT3{ 0.5f, 0.5f, 0.5f };
//	lighting.lightDiffuse = DirectX::XMFLOAT3{ 1.0f, 1.0f, 1.0f };
//	lighting.lightCamPos = DirectX::XMFLOAT3{ 0.0f, 0.0f, -5.0f }; //ljusinställningar 
//
//	D3D11_BUFFER_DESC cbLight;
//	ZeroMemory(&cbLight, sizeof(D3D11_BUFFER_DESC)); //nollställer
//	cbLight.ByteWidth = sizeof(lighting); //Bytesize 
//	cbLight.Usage = D3D11_USAGE_DYNAMIC; //blir tillgänglig för både GPU(read only) och CPU(write only) använd Map.
//	cbLight.BindFlags = D3D11_BIND_CONSTANT_BUFFER; //Binder en buffert som en constantbuffert till ett shader stage
//	cbLight.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;  //resursen ska vara mappable så att CPU kan ändra innehållet
//	cbLight.MiscFlags = 0; //används inte
//
//	device->CreateBuffer(&cbLight, nullptr, &lightConstantBuffer);
//}

void clearView(ID3D11DeviceContext* immediateContext, ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsView, const GeometryBuffer& gBuffer)
{
	float clearcolor[4] = { 0.75f,0.75f,0.3f,0 };
	immediateContext->ClearRenderTargetView(gBuffer.gBuffergBufferRtv[0], clearcolor);
	immediateContext->ClearRenderTargetView(gBuffer.gBuffergBufferRtv[1], clearcolor);
	immediateContext->ClearRenderTargetView(gBuffer.gBuffergBufferRtv[2], clearcolor);
	immediateContext->ClearRenderTargetView(gBuffer.gBuffergBufferRtv[3], clearcolor);
	immediateContext->ClearDepthStencilView(dsView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
}

void update(ID3D11DeviceContext* immediateContext, float dt, Camera& camera, 
			ID3D11Buffer* constantBuffers, Light& lighting, WVP& wvp, Model* biker, ParticleSystem* particlesystem)
{
	camera.Update(dt);
	particlesystem->Update(immediateContext, dt);
	XMMATRIX worldViewProj = XMMatrixTranspose(biker->GetWorldMatrix() * camera.GetViewMatrix() * camera.GetPerspectiveMatrix());

	XMStoreFloat4x4(&wvp.worldSpace, biker->GetWorldMatrix());
	XMStoreFloat4x4(&wvp.worldViewProj, worldViewProj);
	immediateContext->VSSetConstantBuffers(0, 1, &constantBuffers);

	D3D11_MAPPED_SUBRESOURCE dataBegin;
	HRESULT hr = immediateContext->Map(constantBuffers, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &dataBegin);
	if (SUCCEEDED(hr))
	{
		memcpy(dataBegin.pData, &wvp, sizeof(WVP));
		immediateContext->Unmap(constantBuffers, NULL);
	}
	else
	{
		std::cerr << "Failed to update ConstantBuffer (update function)" << std::endl;
	}
}

void RenderGBufferPass(ID3D11DeviceContext* immediateContext, ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsView, D3D11_VIEWPORT& viewport,
	ID3D11PixelShader* pShaderDeferredRender, ID3D11VertexShader* vShaderDeferred,
	ID3D11InputLayout* inputLayout, ID3D11SamplerState* sampler, GeometryBuffer gBuffer,
	ID3D11ShaderResourceView* textureSRV, ID3D11Buffer* vertexBuffer,ParticleSystem* particlesystem, 
	ParticleRenderer* pRenderer, ModelRenderer* mRenderer, const std::vector <Model*>&models, TerrainRenderer* tRenderer, Model* terrain)
{
	clearView(immediateContext, rtv, dsView, gBuffer);

	//UINT stride = sizeof(Vertex);
	//UINT offset = 0;
	/*immediateContext->IASetVertexBuffers(0, 1, biker->GetBuffer(), &stride, &offset);
	immediateContext->IASetInputLayout(inputLayout);
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	immediateContext->VSSetShader(vShaderDeferred, nullptr, 0);*/

	immediateContext->RSSetViewports(1, &viewport);
	immediateContext->PSSetSamplers(0, 1, &sampler);
	immediateContext->DSSetSamplers(0, 1, &sampler);
	immediateContext->OMSetRenderTargets(gBuffer.NROFBUFFERS, gBuffer.gBuffergBufferRtv, dsView);
	/*immediateContext->PSSetShader(pShaderDeferredRender, nullptr, 0);
	immediateContext->PSSetShaderResources(0, 1, biker->GetTexture());*/
	for (auto model : models)
		mRenderer->Render(immediateContext, model);
	/*mRender->Render(immediateContext, biker);
	mRender->Render(immediateContext, sword);*/
	/*immediateContext->Draw(biker->GetVertexCount(), 0);*/
	tRenderer->Render(immediateContext, terrain);
	pRenderer->Render(immediateContext, particlesystem);

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

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	const UINT WIDTH = 1280;
	const UINT HEIGHT = 720;
	HWND window; 
	RedirectIOToConsole();

	Light lighting;
	WVP wvp;

	Camera camera(XM_PIDIV4, (float)WIDTH / (float)HEIGHT, 0.1, 100, 2.5, 2.5f, { 0.0f,0.0f,-5.0f });
	Timer timer;
	float dt = 0.0f;

	ID3D11Device* device; // används för att skapa resurser.
	ID3D11DeviceContext* immediateContext; // genererar renderings kommandon.
	IDXGISwapChain* swapChain;// denna funktion byter back buffern(rtv) och display skärmen.
	ID3D11RenderTargetView* rtv; // denna variabel är en pekare till ett objekt som innehåller all information om renderings objektet.  
	ID3D11DepthStencilView* dsView; // the Depth/Stencil Buffer lagrar depth information för de pixlar som ska renderas.
	D3D11_VIEWPORT viewport; // definierar dimensionerna för en viewport.
	ID3D11Texture2D* dsTexture; // en ID3D11Texture2D är ett objekt som lagrar en platt bild. 
	ID3D11Texture2D* texture;
	
	ID3D11InputLayout* inputLayout; // information som lagras med varje vertex för att förbättra renderingshastigheten
	ID3D11Buffer* vertexBuffer; // buffert resurs, som är ostrukturerat minne
	ID3D11Buffer* constantBuffer;
	ID3D11ShaderResourceView* textureSRV; // anger de (sub resources) en shader kan komma åt under rendering
	ID3D11SamplerState* sampler; // sampler-state som du kan bindar till valfritt shader stage i pipelinen.

	//Deferred
	ID3D11PixelShader* lightPShaderDeferred;
	ID3D11VertexShader* lightVShaderDeferred;
	ID3D11PixelShader* pShaderDeferred;
	ID3D11VertexShader* vShaderDeferred;
	ID3D11InputLayout* renderTargetMeshInputLayout;
	ID3D11Buffer* screenQuadMesh;

	//Gbuffer
	DirectionalLight defferedPS;
	GeometryBuffer gBuffer;
	gBuffer.screenWidth = WIDTH;
	gBuffer.screenHeight = HEIGHT;
	for (int i = 0; i < gBuffer.NROFBUFFERS; i++)
	{
		gBuffer.gBufferTexture[i] = nullptr;
	}

	//ConstantBuffer(s)
	ID3D11Buffer* constantBuffers;

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

	if (!SetupPipeline(device, vertexBuffer, inputLayout, 
		constantBuffers, texture, textureSRV, sampler, pShaderDeferred, vShaderDeferred, lightPShaderDeferred, lightVShaderDeferred,
		renderTargetMeshInputLayout, screenQuadMesh))
	{
		std::cerr << "Failed to setup pipeline!" << std::endl;
		return -3;
	}
	std::vector <Model*>models;
	Model* bike = new Model(device, "biker", { 5.0f, -3.0f, 0.0f }, { 0.0f,XM_PIDIV4,0.0f }, { 0.75f, 0.75f, 0.75f });
	models.push_back(bike);
	Model* sword = new Model(device, "sword", { 10.0f, -3.0f, 0.0f }, { 0.0f,XM_PIDIV4,0.0f }, { 0.4f, 0.4f, 0.4f });
	models.push_back(sword);
	Model* cigg = new Model(device, "cigg", { 0.0f, -3.0f, 0.0f }, { 0.0f,XM_PIDIV4,0.0f }, { 0.05f, 0.05f, 0.05f });
	models.push_back(cigg);
	/*Model* buildings = new Model(device, "buildings", { 0.0f, -3.0f, 0.0f }, { 0.0f,XM_PIDIV4,0.0f }, { 2.0f, 2.0f, 2.0f });
	models.push_back(buildings);*/

	Model* terrain = new Model(device, "terrain", { 0.0f, -4.0f, 0.0f }, { 0.0f, XM_PIDIV4, 0.0f }, { 2.0f, 2.0f, 2.0f });
	terrain->SetDisplacementTexture(device, "Models/terrain/displacement.png");
	ParticleSystem* particlesystem = new ParticleSystem(device, 300, 20, 10, { 20,40,20 }, { 0,20,0 });
	ParticleRenderer* pRenderer = new ParticleRenderer(device);
	ModelRenderer* mRenderer = new ModelRenderer(device);
	TerrainRenderer* tRenderer = new TerrainRenderer(device);
	ShaderData::Initialize(device, mRenderer->GetByteCode());

	MSG msg = {};

	while (!(GetKeyState(VK_ESCAPE) & 0x8000) && msg.message != WM_QUIT)
	{
		timer.Start();
		
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) //if kanske endast visar ett meddelande(while visar alla)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		
		RenderGBufferPass(immediateContext, rtv, dsView, viewport, pShaderDeferred, vShaderDeferred, inputLayout,
			sampler, gBuffer, textureSRV, vertexBuffer, particlesystem, pRenderer, mRenderer,models, tRenderer, terrain);

					//Kallar på vår renderfunktion
		update(immediateContext, dt, camera, constantBuffers,lighting, wvp, bike, particlesystem);

		ShaderData::Update(camera);

		RenderLightPass(immediateContext, rtv, dsView, viewport, pShaderDeferred, vShaderDeferred, inputLayout, vertexBuffer,
			textureSRV, sampler, gBuffer, lightPShaderDeferred,
			lightVShaderDeferred, renderTargetMeshInputLayout, screenQuadMesh);

		swapChain->Present(0, 0); //Presents a rendered image to the user.
		
		dt = timer.DeltaTime();
	}

	delete particlesystem;
	delete pRenderer;

	vShaderDeferred->Release();
	pShaderDeferred->Release();
	screenQuadMesh->Release();
	renderTargetMeshInputLayout->Release();
	lightVShaderDeferred->Release();
	lightPShaderDeferred->Release();
	constantBuffers->Release();
	texture->Release();
	sampler->Release();
	textureSRV->Release();
	vertexBuffer->Release();
	inputLayout->Release();
	dsTexture->Release();
	dsView->Release();
	rtv->Release();
	swapChain->Release();
	immediateContext->Release();
	device->Release();

	return 0;
}
