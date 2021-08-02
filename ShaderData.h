#pragma once
#include "D3D11Helper.h"
#include "D3D11Help.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include "Camera.h"
#include "ShadowMap.h"
#include "Lights.h"
//#include "Lights.h"
using namespace DirectX;


class ShaderData
{
	friend class ParticleRenderer;
	friend class ModelRenderer;
	friend class TerrainRenderer;
	friend class ShadowRenderer;
	friend class BlendModelRenderer;
	friend class WaterRenderer;

private:
	static std::string positionOnly_vs_path;
	static ID3D11VertexShader* positionOnly_vs;

	static XMFLOAT3 cameraPosition;
	static XMMATRIX viewMatrix;
	static XMMATRIX perspectiveMatrix;
	static XMMATRIX lightMatrix;

	static ID3D11InputLayout* positionOnly_layout;

	static ID3D11InputLayout* model_layout;

	static std::string gs_path;
		
	static ID3D11Buffer* cameraPos;

public:
	static ID3D11GeometryShader* geometryShader;
	static ShadowMap* shadowmap;
	static void Shutdown();
	static void Initialize(ID3D11Device* device, std::string modelVSByteCode );
	static void Update(ID3D11DeviceContext*& context, Camera camera, DirectionalLight& dirLight);
};