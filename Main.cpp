#include <DirectXMath.h>
#include <iostream>
#include "Timer.h"
#include "Camera.h"
#include "WindowHelper.h"
#include "D3D11Helper.h"
#include "PipelineHelper.h"
#include "ParticleSystemRenderer.h"
#include "Lights.h"
#include "ModelRenderer.h"
#include "TerrainRenderer.h"
#include "ShadowRenderer.h"
#include "WaterRenderer.h"

//Console Setup
#include<io.h>
#include<fcntl.h>
// Console Function -- found online
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

// Function to clear the window
void clearView(ID3D11DeviceContext* immediateContext, ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsView, const GeometryBuffer& gBuffer)
{
	float clearcolor[4] = { 0.75f, 0.75f, 0.75f,0 };
	immediateContext->ClearRenderTargetView(gBuffer.gBuffergBufferRtv[0], clearcolor);
	immediateContext->ClearRenderTargetView(gBuffer.gBuffergBufferRtv[1], clearcolor);
	immediateContext->ClearRenderTargetView(gBuffer.gBuffergBufferRtv[2], clearcolor);
	immediateContext->ClearRenderTargetView(gBuffer.gBuffergBufferRtv[3], clearcolor);
	immediateContext->ClearRenderTargetView(gBuffer.gBuffergBufferRtv[4], clearcolor);
	immediateContext->ClearRenderTargetView(gBuffer.gBuffergBufferRtv[5], clearcolor);
	immediateContext->ClearRenderTargetView(gBuffer.gBuffergBufferRtv[6], clearcolor);
	immediateContext->ClearRenderTargetView(gBuffer.gBuffergBufferRtv[7], clearcolor);
	immediateContext->ClearDepthStencilView(dsView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
}

// Update function to update the camera, the particles and the CB containing worldSpace and WVP matricies
void update(ID3D11DeviceContext* immediateContext, float dt, Camera& camera, ID3D11Buffer* constantBuffers, WVP& wvp,
			ParticleSystem* particlesystem, DirectionalLight& dirLight, ID3D11Buffer* dirLightBuffer, Model* water, Model* cameraModel)
{
	camera.Update(dt);
	particlesystem->Update(immediateContext, dt);

	dirLight.Update(dt);
	UpdateBuffer(immediateContext, dirLightBuffer, dirLight.data);

	cameraModel->SetTranslation(dirLight.GetPosition());
	cameraModel->Update();

	XMFLOAT3 xPos;
	XMStoreFloat3(&xPos, dirLight.GetPosition());

	// light "pos"
	//std::cout << xPos.x << " " << xPos.y << " " << xPos.z << std::endl;

	water->WaterSettings(DirectX::XMFLOAT2(0.1f, 0.1f), dt);
	UpdateBuffer(immediateContext, *water->GetWaterBuffer(), water->GetUVOffset());
}

// Geometry Pass for deferred rendering (and shadows)
void RenderGBufferPass(ID3D11DeviceContext* immediateContext, ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsView, D3D11_VIEWPORT& viewport,
	ID3D11PixelShader* pShaderDeferredRender, ID3D11VertexShader* vShaderDeferred,
	ID3D11InputLayout* inputLayout, ID3D11SamplerState* sampler, GeometryBuffer gBuffer,
	ID3D11ShaderResourceView* textureSRV, ID3D11Buffer* vertexBuffer,ParticleSystem* particlesystem, 
	ParticleRenderer* pRenderer, ModelRenderer* mRenderer, const std::vector <Model*>&models, TerrainRenderer* tRenderer, Model* terrain,
	ShadowRenderer* sRenderer, ID3D11RasterizerState*& rasterizerStateWireFrame, ID3D11RasterizerState*& rasterizerStateSolid,
	Model* water, ID3D11Device * device, Model* cameraModel, ID3D11SamplerState* clampSampler)
{
	
	ShaderData::shadowmap->Bind(immediateContext); 
	// Binds the shadowmap (sets resources)
	// Loops through the models-vector and renders shadows
	for (auto model : models)
		sRenderer->Render(immediateContext, model);

	sRenderer->Render(immediateContext, water);
	sRenderer->Render(immediateContext, terrain);

	immediateContext->RSSetViewports(NULL, NULL);
	clearView(immediateContext, rtv, dsView, gBuffer); 	// Clear the window for next render pass

	immediateContext->RSSetViewports(1, &viewport);
	immediateContext->PSSetSamplers(0, 1, &sampler);
	immediateContext->PSSetSamplers(1, 1, &clampSampler);
	immediateContext->DSSetSamplers(0, 1, &sampler);
	immediateContext->DSSetSamplers(1, 1, &clampSampler);
	immediateContext->RSSetState(rasterizerStateSolid);
	immediateContext->GSSetShader(ShaderData::geometryShader, nullptr, 0);
	immediateContext->OMSetRenderTargets(gBuffer.NROFBUFFERS, gBuffer.gBuffergBufferRtv, dsView);

	for (auto model : models)
		mRenderer->Render(device, immediateContext, model, false);

	mRenderer->Render(device,immediateContext, water, true);
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

// Light pass -- renders the geometry and lighting on the final screen quad
void RenderLightPass(ID3D11DeviceContext* immediateContext, ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsView, D3D11_VIEWPORT& viewport,
	ID3D11PixelShader* pShaderDeferredRender,ID3D11VertexShader* vShaderDeferred, ID3D11InputLayout* inputLayout, ID3D11Buffer* vertexBuffer,
	ID3D11ShaderResourceView* textureSRV, ID3D11SamplerState* sampler, GeometryBuffer gBuffer, ID3D11PixelShader* lightPShaderDeferred, 
	ID3D11VertexShader* lightVShaderDeferred, ID3D11InputLayout* renderTargetMeshInputLayout, ID3D11Buffer* screenQuadMesh, DirectionalLight &dirLight, 
	ID3D11Buffer* dirLightBuffer, Camera camera, ID3D11Buffer* cameraPos, ID3D11SamplerState* clampSampler)
{
	
	immediateContext->PSSetConstantBuffers(1, 1, &cameraPos);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	immediateContext->IASetVertexBuffers(0, 1, &screenQuadMesh, &stride, &offset);
	immediateContext->IASetInputLayout(renderTargetMeshInputLayout);
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	immediateContext->VSSetShader(lightVShaderDeferred, nullptr, 0);
	immediateContext->RSSetViewports(1, &viewport);
	immediateContext->PSSetShader(lightPShaderDeferred, NULL, 0);
	immediateContext->PSSetShaderResources(0, gBuffer.NROFBUFFERS, gBuffer.gBufferSrv);
	immediateContext->PSSetConstantBuffers(0, 1, &dirLightBuffer);
	immediateContext->PSSetSamplers(0, 1, &sampler);
	immediateContext->RSSetState(nullptr);
	immediateContext->OMSetRenderTargets(1, &rtv, dsView); //*ShaderData::shadowmap->GetDSV()
	immediateContext->Draw(4, 0);

	//Clean up the PixelShaders
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
	const UINT WIDTH = 1600;
	const UINT HEIGHT = 900;
	HWND window; 
	RedirectIOToConsole();

	WVP wvp;

	Camera camera(XM_PIDIV4, (float)WIDTH / (float)HEIGHT, 0.1, 100, 2.5, 5.0f, { 15.0f, 10.0f ,0.0f });
	ID3D11Buffer* cameraBuffer;
	Timer timer;
	float dt = 0.0f;

	ID3D11Device* device; // Used to create resources.
	ID3D11DeviceContext* immediateContext; // Generates rendering commands.
	IDXGISwapChain* swapChain;// This function changes the back buffer (rtv) and the display screen.
	ID3D11RenderTargetView* rtv; // This variable is a pointer to an object that contains all the information about the rendering object. 
	ID3D11DepthStencilView* dsView; // The Depth / Stencil Buffer stores depth information for the pixels to be rendered.
	D3D11_VIEWPORT viewport; // Defines the dimensions of a viewport.
	ID3D11Texture2D* dsTexture; // An ID3D11Texture2D is an object that stores a flat image.
	ID3D11RasterizerState* rasterizerStateWireFrame;
	ID3D11RasterizerState* rasterizerStateSolid;

	ID3D11Texture2D* texture;

	ID3D11InputLayout* inputLayout; // Information stored with each vertex to improve the rendering speed
	ID3D11Buffer* vertexBuffer; // Buffer resource, which is unstructured memory
	ID3D11ShaderResourceView* textureSRV; // Indicates the (sub resources) a shader can access during rendering
	ID3D11SamplerState* wrapSampler; // Sampler-state that you can bind to any shader stage in the pipeline.
	ID3D11SamplerState* clampSampler;

	//Deferred
	ID3D11PixelShader* lightPShaderDeferred; // A pixelshader interface manages an executable program(a pixel shader) that controls the pixel - shader stage.
	ID3D11VertexShader* lightVShaderDeferred;
	ID3D11PixelShader* pShaderDeferred;
	ID3D11VertexShader* vShaderDeferred;
	ID3D11InputLayout* renderTargetMeshInputLayout; // An input-layout interface holds a definition of how to feed vertex data 
													// that is laid out in memory into the input-assembler stage of the graphics pipeline.
	ID3D11Buffer* screenQuadMesh;

	//Gbuffer
	GeometryBuffer gBuffer;
	gBuffer.screenWidth = WIDTH;
	gBuffer.screenHeight = HEIGHT;
	for (int i = 0; i < gBuffer.NROFBUFFERS; i++)
	{
		gBuffer.gBufferTexture[i] = nullptr;
	}

	DirectionalLight dirLight(14, { 0, 1,0 });
	ID3D11Buffer* dirLightBuffer;

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
		constantBuffers, texture, textureSRV, wrapSampler, pShaderDeferred, vShaderDeferred, lightPShaderDeferred, lightVShaderDeferred,
		renderTargetMeshInputLayout, screenQuadMesh, rasterizerStateWireFrame, rasterizerStateSolid, clampSampler))
	{
		std::cerr << "Failed to setup pipeline!" << std::endl;
		return -3;
	}

	// Creating a vector that stores the models
	std::vector <Model*>models;
	std::vector <Model*>things;
	// Creating a new model for each mesh in the scene

	Model* bike = new Model(device, "biker", { -5.0f, 4, -5.0f }, { 0.0f,XM_PIDIV2, XM_PIDIV4 }, { 1, 1, 1 });
	models.push_back(bike);

	Model* bike2 = new Model(device, "biker", { 0.0f, 4, 6.0f }, { 0.0f,-XM_PIDIV2, -XM_PIDIV4 }, { 0.5f, 0.5f, 0.5f });
	models.push_back(bike2);

	Model* cameraModel = new Model(device, "newCube", { 10, -3.6, 0 }, { 0.0f,0.0f,0.0f }, {0.1, 0.1, 0.1});
	models.push_back(cameraModel);


	Model* water = new Model(device, "water", { 0, 3, 0 }, { 0.0f,0.0f,-XM_PIDIV2 }, { 15, 15,15 });
	
	//Model* chessBoard = new Model(device, "ChessBoard", { 0.0f, 1.5, -30 }, { 0.0f,0.0f,0.0f }, { 15, 15, 15});
	//models.push_back(chessBoard);


	/*Model* bChessPieces = new Model(device, "BlackChess", { 0.0f, 2, 20 }, { 0.0f,0.0f,0.0f }, { 15, 15, 15 });
	models.push_back(bChessPieces);


	Model* wChessPieces = new Model(device, "WhiteChess", { 0.0f, 2, 20 }, { 0.0f,0.0f,0.0f }, { 15, 15, 15 });
	models.push_back(wChessPieces);*/


	Model* terrain = new Model(device, "terrain", { 0.0f, 2, 0 }, { 0.0f, 0.0f, 0.0f }, { 1, 1, 1 });
	terrain->SetDisplacementTexture(device, "Models/terrain/displacement.png");
	terrain->AddTexture(device, "snow.jpg");
	things.push_back(terrain);


	ParticleSystem* particlesystem = new ParticleSystem(device, 200, 5, 1, { 60,25,60 }, { 0,20,0 });
	ParticleRenderer* pRenderer = new ParticleRenderer(device);
	ModelRenderer* mRenderer = new ModelRenderer(device);
	TerrainRenderer* tRenderer = new TerrainRenderer(device);
	ShadowRenderer* sRenderer = new ShadowRenderer(device);
	//WaterRenderer* wRenderer = new WaterRenderer(device);

	ShaderData::Initialize(device, mRenderer->GetByteCode());

	CreateBuffer(device, dirLightBuffer, sizeof(DirectionalLight::Data));
	UpdateBuffer(immediateContext, dirLightBuffer, dirLight.data);
	CreateBuffer(device, cameraBuffer, sizeof(XMFLOAT4));
	CreateBuffer(device, *water->GetWaterBuffer(), sizeof(XMFLOAT4));


	MSG msg = {};

	while (!(GetKeyState(VK_ESCAPE) & 0x8000) && msg.message != WM_QUIT)
	{
		timer.Start();
		
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	
		ShaderData::Update(immediateContext, camera, dirLight);

		//UpdateBuffer(immediateContext, cameraBuffer, camera.GetPosition());

		RenderGBufferPass(immediateContext, rtv, dsView, viewport, pShaderDeferred, vShaderDeferred, inputLayout,
			wrapSampler, gBuffer, textureSRV, vertexBuffer, particlesystem, pRenderer, mRenderer,models, tRenderer, terrain, sRenderer, 
			rasterizerStateWireFrame, rasterizerStateSolid, water, device, cameraModel, clampSampler);
		
		update(immediateContext, dt, camera, constantBuffers, wvp, particlesystem, dirLight, dirLightBuffer, water , cameraModel);

		RenderLightPass(immediateContext, rtv, dsView, viewport, pShaderDeferred, vShaderDeferred, inputLayout, vertexBuffer,
			textureSRV, wrapSampler, gBuffer, lightPShaderDeferred,	lightVShaderDeferred, renderTargetMeshInputLayout, screenQuadMesh, 
			dirLight, dirLightBuffer, camera, cameraBuffer, clampSampler);

		swapChain->Present(0, 0); // Presents a rendered image to the user.
		
		dt = timer.DeltaTime();
	}

	delete particlesystem;
	delete pRenderer;

	for (int i = 0; i < gBuffer.NROFBUFFERS; i++)
	{
		gBuffer.gBuffergBufferRtv[i]->Release();
		gBuffer.gBufferSrv[i]->Release();
		gBuffer.gBufferTexture[i]->Release();
	}
	cameraBuffer->Release();
	dirLightBuffer->Release();
	vShaderDeferred->Release();
	pShaderDeferred->Release();
	screenQuadMesh->Release();
	renderTargetMeshInputLayout->Release();
	lightVShaderDeferred->Release();
	lightPShaderDeferred->Release();
	constantBuffers->Release();
	texture->Release();
	clampSampler->Release();
	wrapSampler->Release();
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
