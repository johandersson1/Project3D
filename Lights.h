#pragma once
#include <DirectXMath.h>
#include <Windows.h>
#include "Timer.h"
using namespace DirectX;

class DirectionalLight
{
private:

	XMMATRIX matrix = XMMatrixIdentity();

public:
	struct Data
	{
		XMFLOAT4 color{ 1, 1, 1, 1 };
		XMFLOAT3 direction;
		float range;
	}data;

	DirectionalLight() = default;
	DirectionalLight(float range)
	{
		data.range = range;
		// OrtographicMatrix = Builds a custom orthogonal projection matrix for a left-handed coordinate system.
		XMMATRIX ortographicMatrix = XMMatrixOrthographicOffCenterLH(-range, range, -range, range, -range, range * 12.0f);

		XMVECTOR direction = { 0.75f, 0.65f, 0 };
		// Returns the normalized version of a 3D vector
		XMVector3Normalize(direction);

		XMVECTOR position = direction * data.range;		// from origo through direction, distance on range

		XMStoreFloat3(&data.direction, direction);

		XMMATRIX viewMatrix = XMMatrixLookAtLH(position, { 0,0,0 }, { 0 ,1 ,0 });
		matrix = XMMatrixTranspose(viewMatrix * ortographicMatrix);
	}


	XMMATRIX GetMatrix() const { return this->matrix; }
};



