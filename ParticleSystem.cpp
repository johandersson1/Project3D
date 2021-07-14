#include "ParticleSystem.h"

ParticleSystem::ParticleSystem(ID3D11Device* device, int maxParticles, float maxVelocity, float minVelocity, XMFLOAT3 bounds, XMFLOAT3 position)
	:maxParticles(maxParticles), maxVelocity(maxVelocity), minVelocity(minVelocity), bounds(bounds), position(position),
	 particleCount(0), timeBetweenParticles(0.02f), timeSinceLastParticle(0.0f)
{
    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    bufferDesc.ByteWidth = sizeof(XMFLOAT3) * maxParticles;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = 0;

    positions.reserve(sizeof(XMFLOAT3) * maxParticles);

    D3D11_SUBRESOURCE_DATA data = {};
    data.pSysMem = positions.data();
    data.SysMemPitch = 0;
    data.SysMemSlicePitch = 0;

    HRESULT hr = device->CreateBuffer(&bufferDesc, &data, &vertexBuffer);
    if FAILED(hr)
    {
        std::cout << "FAILED TO CREATE BUFFER" << std::endl;
    }

}



void ParticleSystem::Update(ID3D11DeviceContext* context, float dt)
{
    positions.clear();

    if (particleCount < maxParticles)
    {
        timeSinceLastParticle += dt;

        if (timeSinceLastParticle >= timeBetweenParticles)
        {
            timeSinceLastParticle = 0;

            Particle newParticle = {};

            newParticle.position.x = Random::Real(position.x - bounds.x / 2.0f, position.x + bounds.x / 2.0f);
            newParticle.position.z = Random::Real(position.z - bounds.z / 2.0f, position.z + bounds.z / 2.0f);
            newParticle.position.y = bounds.y;
            newParticle.velocity = Random::Real(minVelocity, maxVelocity);

            particles.push_back(newParticle);
            particleCount++;
        }
    }

    for (auto& particle : particles)
    {
        particle.position.y -= particle.velocity * dt;
        if (particle.position.y <= position.y - bounds.y)
        {
            particle.position.y = position.y;
        }
        positions.push_back(particle.position);
    }
    UpdateBuffer(context);
}


void ParticleSystem::UpdateBuffer(ID3D11DeviceContext* context)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource = {};

	HRESULT hr = context->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hr))
	{
		std::cout << "FAILED TO TO CREATE PARTICLE VERTEXBUFFER" << std::endl;
	}
	memcpy(mappedResource.pData, positions.data(), sizeof(XMFLOAT3) * particleCount);
	context->Unmap(vertexBuffer, 0);
}

ParticleSystem::~ParticleSystem()
{
    vertexBuffer->Release();
}