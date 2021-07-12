#pragma once
#include <DirectXMath.h>
#include <Windows.h>
using namespace DirectX;

struct DirectionalLight
{
	DirectionalLight() { ZeroMemory(this, sizeof(this)); }
	
	struct Data
	{
		XMFLOAT4 ambient;
		XMFLOAT4 diffuse;
		XMFLOAT4 specular;
		XMFLOAT4 position;
		XMFLOAT4 direction;
		XMFLOAT3 attenuation;
		float range;
	}data;

	struct boundSphere
	{
		boundSphere() : Center(0.0f, 0.0f, 0.0f), Radius(0.0f) {}
		DirectX::XMFLOAT3 Center;
		float Radius;
	};

	//DirectionalLight()
	//	:ambient({ 0.5f,0.5f,0.5f,1.0f }), diffuse({ 0.7f,0.7f,0.7f,1.0f }), specular({ 1.0f,1.0f,1.0f,1.0f, }), position({ 0.0f,20,20, 1 }),
	//	camPosition({ 0,0,0,0 }), lightDirection({ 0.0f,-0.5f, -1.0f, 1.0f }), range(10), strength(2.5f), att0(0), att1(1), att2(0) {};
	XMMATRIX GetMatrix() { return this->matrix; }

	void SetMatrix(DirectX::XMFLOAT4X4& ShadowTransform)
	{
		boundSphere mSceneBounds;
		mSceneBounds.Center = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		mSceneBounds.Radius = sqrtf(10.0f * 10.0f + 15.0f) * 2;


		DirectX::XMVECTOR lightDir = DirectX::XMVector3Normalize(DirectX::XMLoadFloat4(&this->data.direction));
		DirectX::XMStoreFloat4(&this->data.direction, lightDir);
		float tempScalar = 2 * mSceneBounds.Radius; //enligt boken är det negativt
		DirectX::XMVECTOR lightPos = DirectX::XMVectorSet(tempScalar * this->data.position.x, tempScalar * this->data.position.y, tempScalar * this->data.position.z, 1.0f);
		DirectX::XMVECTOR targetPos = DirectX::XMLoadFloat3(&mSceneBounds.Center);
		DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		DirectX::XMFLOAT3 sphereCenterLS;
		DirectX::XMMATRIX V = DirectX::XMMatrixLookAtLH(lightPos, targetPos, up);


		DirectX::XMStoreFloat3(&sphereCenterLS, DirectX::XMVector3Transform(targetPos, V));
		float l = sphereCenterLS.x - mSceneBounds.Radius;
		float b = sphereCenterLS.y - mSceneBounds.Radius;
		float n = sphereCenterLS.z - mSceneBounds.Radius;
		float r = sphereCenterLS.x + mSceneBounds.Radius;
		float t = sphereCenterLS.y + mSceneBounds.Radius;
		float f = sphereCenterLS.z + mSceneBounds.Radius;
		DirectX::XMMATRIX P = DirectX::XMMatrixOrthographicOffCenterLH(l, r, b, t, n, f);

		DirectX::XMMATRIX S = V * P;
		DirectX::XMStoreFloat4x4(&ShadowTransform, S);
	}
private:
	XMMATRIX matrix;
};

