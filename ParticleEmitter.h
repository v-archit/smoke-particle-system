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

	ParticleEmitter(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 startVelocity, std::shared_ptr<Material> material, int maxParticleCount, float lifetime,
		float emissionTime, float startSize, float endSize, DirectX::XMFLOAT4 startColor, DirectX::XMFLOAT4 endColor, 
		Microsoft::WRL::ComPtr<ID3D11Device> device);

	~ParticleEmitter();

	void InititalizeGeometry();
	//update particles positions etc.
	void SimulateParticles(float dt, std::shared_ptr<Camera> camera);
	void DrawParticles(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, std::shared_ptr<Camera> camera);
private:
	int maxParticleCount;
	Particle* particles;
	ParticleVertex* particleVertices;
	Transformation transform;
	float lifetime;
	//total time elapsed after simulating
	float timeElapsed;
	//time taken by 1 particle to emit
	float emissionTime;
	float startSize;
	DirectX::XMFLOAT3 startVelocity;
	float endSize;
	//particle index
	int pIndex;
	DirectX::XMFLOAT4 startColor;
	DirectX::XMFLOAT4 endColor;
	DirectX::XMFLOAT2 particleUV[4];

	std::shared_ptr<Material> material;

	Microsoft::WRL::ComPtr<ID3D11Buffer> vBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> iBuffer;

	//create v and i buffers
	void CreateBuffers(Microsoft::WRL::ComPtr<ID3D11Device> device);
	void UpdateParticles(float dt, int pIndex, std::shared_ptr<Camera> camera);

	void EmitParticles();

	//Credits: Prof Cascioli
	//algo to calculate vertex position in respect to camera
	DirectX::XMFLOAT3 CalcParticleVertexPosition(int particleIndex, int quadCornerIndex, std::shared_ptr<Camera> camera);
};