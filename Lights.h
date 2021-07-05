#pragma once
#include <DirectXMath.h>
#include <Windows.h>
using namespace DirectX;

struct DirectionalLight
{
	DirectionalLight() { ZeroMemory(this, sizeof(this)); }
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
	XMFLOAT4 specular;
	XMFLOAT4 position;
	XMFLOAT4 camPosition;
	XMFLOAT4 lightDirection;
	float range;
	float strength;
	float att0;
	float att1;
	float att2;
	float pad;
	float pad2;
	float pad3;
};