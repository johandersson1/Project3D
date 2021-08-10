#pragma once
#include "D3D11Helper.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include "Camera.h"
#include "ShadowMap.h"
#include "Lights.h"
//#include "Lights.h"
using namespace DirectX;

class ShaderData
{
	// A friend class can access private and protected members of another class in which it is declared as friend
	friend class ParticleRenderer; 
	friend class ModelRenderer;
	friend class TerrainRenderer;
	friend class ShadowRenderer;

private:
	static std::string positionOnly_vs_path;
	// For particleSystemRenderer, setting vs
	static ID3D11VertexShader* positionOnly_vs;
	// Setting this in update, and getting the values from Camera and DirectionalLight
	static XMFLOAT3 cameraPosition;
	static XMMATRIX viewMatrix;
	static XMMATRIX perspectiveMatrix;
	static XMMATRIX lightMatrix;
	// positionOnly_layout used for particle and shadowRenderer
	static ID3D11InputLayout* positionOnly_layout;
	// Model_layout used for model and terrainRenderer
	static ID3D11InputLayout* model_layout;
	// For GeometryShader 
	static std::string gs_path;
	// Used for the backfaced culling? check here!
	static ID3D11Buffer* cameraPos;

public:
	static ID3D11GeometryShader* geometryShader;
	static ShadowMap* shadowmap;
	static void Shutdown();
	static void Initialize(ID3D11Device* device, std::string modelVSByteCode );
	static void Update(ID3D11DeviceContext*& context, Camera camera, DirectionalLight& dirLight);
};