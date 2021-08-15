#pragma once
#include "D3D11Helper.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include "Camera.h"
#include "ShadowMap.h"
#include "Lights.h"
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
	static ID3D11InputLayout* positionOnly_layout;	// positionOnly_layout used for particle and shadowRenderer
	static ID3D11VertexShader* positionOnly_vs;
	static std::string gs_path; 					// For GeometryShader 

	static ID3D11InputLayout* model_layout;			// model_layout used for model- and terrainRenderer

	// Setting this in update, and getting the values from Camera and DirectionalLight
	static XMFLOAT3 cameraPosition;					// XMFLOAT3 containg the cameras Position
	static XMMATRIX viewMatrix;						// Matrix containing the cameras viewMatrix
	static XMMATRIX perspectiveMatrix;				// Matrix containing the cameras perpectiveMatrix
	static XMMATRIX lightMatrix;					// Matrix containing the light orthographic viewProjection-matrix
	static ID3D11Buffer* cameraPos; 				// Used for the backfaced culling

public:
	static ID3D11GeometryShader* geometryShader;
	static ShadowMap* shadowmap;
	static void Shutdown();
	static void Initialize(ID3D11Device* device, std::string modelVSByteCode );
	static void Update(ID3D11DeviceContext*& context, Camera camera, DirectionalLight& dirLight);
};