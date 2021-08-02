#pragma once

#include "Random.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <iostream>
using namespace DirectX;
class ParticleSystem
{
	friend class ParticleRenderer;
private:
	struct Particle
	{
		XMFLOAT3 position;
		float velocity;
	};

	XMFLOAT3 bounds;
	XMFLOAT3 position;

	float maxVelocity;
	float minVelocity;

	int particleCount;
	int maxParticles;

	float timeBetweenParticles;
	float timeSinceLastParticle;

	std::vector<Particle> particles;
	std::vector<XMFLOAT3> positions;

	ID3D11Buffer* vertexBuffer;

	void UpdateBuffer(ID3D11DeviceContext* context);

public:
	ParticleSystem() = default;
	ParticleSystem(ID3D11Device* device, int maxParticles, float maxVelocity, float minVelocity, XMFLOAT3 bounds, XMFLOAT3 position);
	~ParticleSystem();
	void Update(ID3D11DeviceContext* context, float dt);
	void Shutdown() { vertexBuffer->Release(); }

};