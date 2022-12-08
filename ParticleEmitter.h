#pragma once

#include"Particle.h"
#include"DirectXMath.h"
#include <wrl/client.h>
#include <d3d11.h>
#include<memory>
#include"Camera.h"
#include "SimpleShader.h"
#include"Transformation.h"
#include"Material.h"

class ParticleEmitter
{
public:

	ParticleEmitter(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 startVelocity, int maxParticleCount, float lifetime, 
		int emissionRate, float startSize, float endSize, DirectX::XMFLOAT4 startColor, DirectX::XMFLOAT4 endColor, 
		Microsoft::WRL::ComPtr<ID3D11Device> device);

	~ParticleEmitter();

	void SimulateParticles(float dt);
	void DrawParticles(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, std::shared_ptr<Camera> camera);
private:
	int maxParticleCount;
	Particle* particles;
	ParticleVertex* particleVertices;
	Transformation transform;
	float lifetime;
	int emissionRate;
	float startSize;
	DirectX::XMFLOAT3 startVelocity;
	float endSize;
	int pIndex;
	DirectX::XMFLOAT4 startColor;
	DirectX::XMFLOAT4 endColor;
	DirectX::XMFLOAT2 particleUV[4];

	std::shared_ptr<Material> material;

	Microsoft::WRL::ComPtr<ID3D11Buffer> vBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> iBuffer;

	void CreateBuffers(Microsoft::WRL::ComPtr<ID3D11Device> device);
	void UpdateParticles(float dt, int pIndex);
	void EmitParticles();
	unsigned int* CreateConstantIndices();
	DirectX::XMFLOAT3 CalcParticleVertexPosition(int particleIndex, int quadCornerIndex, std::shared_ptr<Camera> camera);
};