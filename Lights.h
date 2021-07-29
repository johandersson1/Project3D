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
		ortographicMatrix = XMMatrixOrthographicOffCenterLH(-range, range, -range, range, -range, range * 12.0f);
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
		XMVector3Normalize(direction);

		position = direction * data.range;

		XMVECTOR up = { 0 ,1 ,0 };

		if (currentAngle > XM_PIDIV2)
		{
			up = { 0, -1, 0 };
		}

		XMStoreFloat3(&data.direction, direction);

		this->viewMatrix = XMMatrixLookAtLH(position, { 0,0,0 }, up);
		matrix = XMMatrixTranspose(viewMatrix * ortographicMatrix);
	}

	XMVECTOR GetPosition() { return this->position; }

	XMMATRIX GetMatrix() const { return this->matrix; }
};



