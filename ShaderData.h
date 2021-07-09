#pragma once
#include "D3D11Helper.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include "Camera.h"
#include "ShadowMap.h"
//#include "Lights.h"
using namespace DirectX;


class ShaderData
{
	friend class ParticleRenderer;
	friend class ModelRenderer;
	friend class TerrainRenderer;
	friend class ShadowRenderer;
private:
	static std::string positionOnly_vs_path;
	static ID3D11VertexShader* positionOnly_vs;

	static XMFLOAT3 cameraPosition;
	static XMMATRIX viewMatrix;
	static XMMATRIX perspectiveMatrix;
	
	static ID3D11InputLayout* positionOnly_layout;

	static ID3D11InputLayout* model_layout;

public:

	static ShadowMap* shadowmap;
	static void Shutdown();
	static void Initialize(ID3D11Device* device, std::string modelVSByteCode );
	static void Update( Camera camera);
};