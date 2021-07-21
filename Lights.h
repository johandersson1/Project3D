#pragma once
#include <DirectXMath.h>
#include <Windows.h>
using namespace DirectX;

class DirectionalLight
{
private:
	XMMATRIX matrix = XMMatrixIdentity();
public:
	DirectionalLight() = default;
	DirectionalLight(float range, XMVECTOR direction)
	{
		direction = XMVector3Normalize(direction);

		XMVECTOR position = -direction * range;
		XMVECTOR target = { 0,0,0 };
		XMVECTOR up = { 0,1,0 };

		XMMATRIX M1 = XMMatrixLookAtLH(position, target, up);
		XMMATRIX M2 = XMMatrixOrthographicOffCenterLH(-range, range, -range, range, -range, range);

		matrix = XMMatrixTranspose(M1 * M2);
	}

	XMMATRIX GetMatrix() const { return this->matrix; }
};

struct PointLight
{
public:
	XMFLOAT4 color = { 1,1,1,1 };
	XMFLOAT3 position = { 0,0,0 };
	float range = 0;
	XMFLOAT3 attenuation = { 0,0,0 };
	float padding = 0;
public:
	PointLight() = default;
	PointLight(float range, XMFLOAT3 position,  XMFLOAT3 color = { 1.0f, 1.0f, 1.0f }, XMFLOAT3 attenuation = { 1.0f, 1.0f, 1.0f })
	{
		this->color = XMFLOAT4(color.x, color.y, color.z, 1.0f);
		this->position = position;
		this->attenuation = attenuation;
		this->range = range;
	}
};


