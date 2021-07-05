#include <DirectXMath.h>
#include "WindowHelper.h"
#include "D3D11Helper.h"
#include "PipelineHelper.h"
#include <chrono>
#include <iostream>
#include "Camera.h"

void SetUpSpace(DirectX::XMMATRIX &cameraPerspective, DirectX::XMMATRIX &cameraProjection, DirectX::XMVECTOR cameraPos, DirectX::XMVECTOR lookAt, DirectX::XMVECTOR upVector)
{
	cameraPerspective = DirectX::XMMatrixLookAtLH(cameraPos, lookAt, upVector); //skapar v�nster-orienterat koordinatsystem,
	cameraProjection = DirectX::XMMatrixPerspectiveFovLH(0.45f * 3.14f, (float)640 / 480, 0.1f, 20.0f); //projicering matris baserad p� field of view, 
}

void SetUpLight(ID3D11Device* device, ID3D11Buffer* &lightConstantBuffer, Light &lighting)
{
	lighting.lightColour = DirectX::XMFLOAT3{ 1.0f, 1.0f, 1.0f };
	lighting.lightAmbient = DirectX::XMFLOAT3{ 0.25f, 0.25f, 0.25f };
	lighting.lightDiffuse = DirectX::XMFLOAT3{ 1.0f, 1.0f, 1.0f };
	lighting.lightCamPos = DirectX::XMFLOAT3{ 0.0f, 0.0f, 5.0f }; //ljusinst�llningar 

	D3D11_BUFFER_DESC cbLight;
	ZeroMemory(&cbLight, sizeof(D3D11_BUFFER_DESC)); //nollst�ller
	cbLight.ByteWidth = sizeof(lighting); //Bytesize 
	cbLight.Usage = D3D11_USAGE_DYNAMIC; //blir tillg�nglig f�r b�de GPU(read only) och CPU(write only) anv�nd Map.
	cbLight.BindFlags = D3D11_BIND_CONSTANT_BUFFER; //Binder en buffert som en constantbuffert till ett shader stage
	cbLight.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;  //resursen ska vara mappable s� att CPU kan �ndra inneh�llet
	cbLight.MiscFlags = 0; //anv�nds inte

	device->CreateBuffer(&cbLight, nullptr, &lightConstantBuffer);
}

void update(ID3D11DeviceContext* immediateContext , XMMATRIX &worldSpace, XMMATRIX &theRotation, XMMATRIX arbitraryPoint, 
		XMMATRIX translation, float &Rotation, float RotationAmount, std::chrono::duration<float> TheDeltaTime, 
		Camera camera, ID3D11Buffer* constantBuffers)
{
	Rotation += RotationAmount * TheDeltaTime.count();
     if (Rotation >= DirectX::XM_PI * 2)
     {
		Rotation -= DirectX::XM_PI * 2;
     }
	theRotation = arbitraryPoint * DirectX::XMMatrixRotationY(Rotation); //XMMatrixRotationY = Bygger en matris som roterar runt y-axeln.
	worldSpace = theRotation * translation; //matris * matristranslation = worldspace
	XMMATRIX scalingMatrix = XMMatrixScaling(1, 1, 1);
	XMMATRIX rotationMatrix = XMMatrixRotationX(0) * DirectX::XMMatrixRotationY(0) * DirectX::XMMatrixRotationZ(0);
	XMMATRIX translationMatrix = DirectX::XMMatrixTranslation(0, 0, 0);
	XMMATRIX worldMatrix = scalingMatrix * rotationMatrix * translationMatrix;
	
	//XMMATRIX viewMatrix = XMMatrixLookToLH(DirectX::XMVectorSet(camera->getCameraPos().x, camera->getCameraPos().y, camera->getCameraPos().z, 0), DirectX::XMVectorSet(camera->getCameraDir().x, camera->getCameraDir().y, camera->getCameraDir().z, 0), DirectX::XMVectorSet(0, 1, 0, 0));
	XMMATRIX worldViewProj = XMMatrixTranspose(worldMatrix * camera.cameraPerspective * camera.cameraProjection);

	immediateContext->VSSetConstantBuffers(0, 1, &constantBuffers);
	D3D11_MAPPED_SUBRESOURCE dataBegin;
	HRESULT hr = immediateContext->Map(constantBuffers, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &dataBegin);
	if (SUCCEEDED(hr))
	{
		memcpy(dataBegin.pData, &camera, sizeof(WVP));
		immediateContext->Unmap(constantBuffers, NULL);
	}
	else
	{
		std::cerr << "Failed to update ConstantBuffer (update function)" << std::endl;
	}

	
}

void Render(ID3D11DeviceContext* immediateContext, ID3D11RenderTargetView* rtv,
	ID3D11DepthStencilView* dsView, D3D11_VIEWPORT& viewport, ID3D11VertexShader* vShader,
	ID3D11PixelShader* pShader, ID3D11InputLayout* inputLayout, ID3D11Buffer* vertexBuffer,
	ID3D11ShaderResourceView* textureSRV, ID3D11SamplerState* sampler, ID3D11Buffer* constantBuffers, 
	ID3D11Buffer* lightConstantBuffer,DirectX::XMMATRIX worldSpace,
	DirectX::XMMATRIX cameraPerspective, DirectX::XMMATRIX cameraProjection, 
	WVP &imageCamera, Light &lighting)
{
	float clearColour[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

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
	////Transponering av ren rotation producerar invers som beh�vs
	////Ange kamerans invers p� objekt f�r att �centrera�
	//imageCamera.worldViewProj = XMMatrixTranspose(worldSpace * cameraPerspective * cameraProjection); //Skapar en ny matris
	
	UINT stride = sizeof(SimpleVertex);
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
	const UINT WIDTH = 1024; //bredd f�r f�nstret 
	const UINT HEIGHT = 1024;//h�jd f�r f�nstret 
	HWND window; //A handle to a window.

	//std::chrono::steady_clock::time_point theDeltaTime = std::chrono::steady_clock::now();

	//Structs from PipelineHelper
	Light lighting;
	WVP imageCamera;


	Camera camera(XMFLOAT3(0,0,-3), XMFLOAT3(0,0,1));



	DirectX::XMMATRIX cameraPerspective;
	DirectX::XMMATRIX cameraProjection;
	DirectX::XMMATRIX theRotation;

	DirectX::XMMATRIX worldSpace = DirectX::XMMatrixIdentity(); //Identitets matris 
	DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation(0.0f, 0.0f, 1.0f); //translationsmatris 
	DirectX::XMMATRIX arbitraryPoint = DirectX::XMMatrixTranslation(0.0f, 0.0f, -2.0f); //translationsmatris 

	//kopplar in kameran
	DirectX::XMVECTOR cameraPos = DirectX::XMVectorSet(0.0f, 0.0f, 5.0f, 0.0f); //skapar en vector med 4 floats 
	DirectX::XMVECTOR lookAt = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	DirectX::XMVECTOR upVector = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	ID3D11Device* device; // anv�nds f�r att skapa resurser.
	ID3D11DeviceContext* immediateContext; // genererar renderings kommandon.
	IDXGISwapChain* swapChain;// denna funktion byter back buffern(rtv) och display sk�rmen.
	ID3D11RenderTargetView* rtv; // denna variabel �r en pekare till ett objekt som inneh�ller all information om renderings objektet.  
	ID3D11DepthStencilView* dsView; // the Depth/Stencil Buffer lagrar depth information f�r de pixlar som ska renderas.
	D3D11_VIEWPORT viewport; // definierar dimensionerna f�r en viewport.
	ID3D11Texture2D* dsTexture; // en ID3D11Texture2D �r ett objekt som lagrar en platt bild. 
	ID3D11Texture2D* texture;
	ID3D11VertexShader* vShader; // hanterar ett k�rbart program f�r att styra vertex-shader-scenen
	ID3D11PixelShader* pShader; // hanterar ett k�rbart program f�r att styra pixel-shader-scenen
	ID3D11InputLayout* inputLayout; // information som lagras med varje vertex f�r att f�rb�ttra renderingshastigheten
	ID3D11Buffer* vertexBuffer; // buffert resurs, som �r ostrukturerat minne
	ID3D11Buffer* constantBuffer;
	ID3D11Buffer* lightConstantBuffer;
	ID3D11ShaderResourceView* textureSRV; // anger de (sub resources) en shader kan komma �t under rendering
	ID3D11SamplerState* sampler; // sampler-state som du kan bindar till valfritt shader stage i pipelinen.

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
		swapChain, rtv, dsTexture, dsView, viewport))
	{
		std::cerr << "Failed to setup d3d11!" << std::endl;
		return -2;
	}

	if (!SetupPipeline(device, vertexBuffer, vShader, pShader, inputLayout, 
		constantBuffers, texture, textureSRV, sampler))
	{
		std::cerr << "Failed to setup pipeline!" << std::endl;
		return -3;
	}

	SetUpSpace(cameraPerspective, cameraProjection, cameraPos, lookAt, upVector); //kallar p� SetUpSpace
	SetUpLight(device, lightConstantBuffer, lighting); //kallar p� SetUpLight

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
			Render(immediateContext, rtv, dsView, viewport, vShader, pShader, 
				   inputLayout, vertexBuffer, textureSRV, sampler, constantBuffers, lightConstantBuffer, worldSpace,
				   cameraPerspective, cameraProjection, imageCamera, lighting);
			//Kallar p� v�r renderfunktion
			update(immediateContext, worldSpace, theRotation, arbitraryPoint, translation, Rotation, theRotationAmount, 
		    		TheDeltaTime, camera, constantBuffers);

			swapChain->Present(0, 0); //Presents a rendered image to the user.
		}
		stopTime = std::chrono::steady_clock::now();
	}

	constantBuffers->Release();
	texture->Release();
	sampler->Release();
	textureSRV->Release();
	lightConstantBuffer->Release();
	//constantBuffer->Release();
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