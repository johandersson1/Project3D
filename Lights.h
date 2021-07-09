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

	DirectionalLight()
		:ambient({ 0.5f,0.5f,0.5f,1.0f }), diffuse({ 0.7f,0.7f,0.7f,1.0f }), specular({ 1.0f,1.0f,1.0f,1.0f, }), position({ 0.0f,20,20, 1 }),
		camPosition({ 0,0,0,0 }), lightDirection({ 0.0f,-0.5f, -1.0f, 1.0f }), range(10), strength(2.5f), att0(0), att1(1), att2(0) {};

};