#pragma once
#include <DirectXMath.h>
#include <Windows.h>
#include "Timer.h"
using namespace DirectX;

class DirectionalLight
{
private:

	float range;
	int dir = 1;
	float currentAngle = 0.5;
	XMVECTOR position;
	XMMATRIX viewMatrix;
	XMMATRIX ortographicMatrix;
	XMMATRIX matrix = XMMatrixIdentity();
public:
	struct Data
	{
		XMFLOAT4 color{ 1, 1, 1, 1 };
		XMFLOAT3 direction;
		float range;
	}data;
public:

	DirectionalLight() = default;
	DirectionalLight(float range, XMVECTOR direction)
	{
		data.range = range;
		direction = XMVector3Normalize(direction);
		position = -direction * range;
		XMVECTOR target = { 0,0,0 };
		XMVECTOR up = { 0,1,0 };

		viewMatrix = XMMatrixLookAtLH(position, target, up);
		ortographicMatrix = XMMatrixOrthographicOffCenterLH(-range, range, -range, range, -range, range);

		matrix = XMMatrixTranspose(viewMatrix * ortographicMatrix);
	}

	void Update(float dt)
	{
		currentAngle += dt * 0.35f * dir;
		if (currentAngle > XM_PI - 0.5f || currentAngle < 0 + 0.5f)
		{
			dir *= -1;
		}
		float x = cos(currentAngle);
		float y = sin(currentAngle);

		XMVECTOR direction = { x, y, 0 };
		XMVector2Normalize(direction);

		position = direction * data.range;
		
		XMStoreFloat3(&data.direction, direction);

		this->viewMatrix = XMMatrixLookAtLH(position, { 0,0,0 }, { 0,1,0 });
		matrix = XMMatrixTranspose(viewMatrix * ortographicMatrix);
	}

	XMVECTOR GetPosition() { return this->position; }

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
	void UpdatePosition(XMFLOAT3 position)
	{
		this->position = position;
	};
};


