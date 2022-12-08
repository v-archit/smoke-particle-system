#include "ParticleEmitter.h"

using namespace DirectX;        //for operator overloading

ParticleEmitter::ParticleEmitter(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 startVelocity, int maxParticleCount, 
	float lifetime, int emissionRate, float startSize, float endSize, DirectX::XMFLOAT4 startColor, 
	DirectX::XMFLOAT4 endColor, Microsoft::WRL::ComPtr<ID3D11Device> device) 
	: maxParticleCount(maxParticleCount), lifetime(lifetime), emissionRate(emissionRate), startSize(startSize), 
	endSize(endSize), startColor(startColor), endColor(endColor), pIndex(0) 
{
	transform.SetPosition(position.x, position.y, position.z);
	
	//defualt initialization
	particles = new Particle[maxParticleCount];
	particleVertices = new ParticleVertex[4 * maxParticleCount];      //4 vertices per particle

	particleUV[0] = DirectX::XMFLOAT2(0, 0);
	particleUV[1] = DirectX::XMFLOAT2(1, 0);
	particleUV[2] = DirectX::XMFLOAT2(1, 1);
	particleUV[3] = DirectX::XMFLOAT2(0, 1);

	//clockwise default uv
	for (int i = 0; i < maxParticleCount; i++)
	{
		particleVertices[i].UV =   DirectX::XMFLOAT2(0, 0);
		particleVertices[i+1].UV = DirectX::XMFLOAT2(1, 0);
		particleVertices[i+2].UV = DirectX::XMFLOAT2(1, 1);
		particleVertices[i+3].UV = DirectX::XMFLOAT2(0, 1);
	}
	
	CreateBuffers(device);
}

ParticleEmitter::~ParticleEmitter()
{
}

void ParticleEmitter::SimulateParticles(float dt)
{
	for (int i = 0; i < pIndex; i++)
	{
		UpdateParticles(dt, i);
	}
	EmitParticles();
}

//Credits: Prof Cascioli
DirectX::XMFLOAT3 ParticleEmitter::CalcParticleVertexPosition(int particleIndex, int quadCornerIndex, std::shared_ptr<Camera> camera)
{
	// Get the right and up vectors out of the view matrix
	DirectX::XMFLOAT4X4 view = camera->GetViewMatrix();
	DirectX::XMVECTOR camRight = DirectX::XMVectorSet(view._11, view._21, view._31, 0);
	DirectX::XMVECTOR camUp = DirectX::XMVectorSet(view._12, view._22, view._32, 0);

	// Determine the offset of this corner of the quad
	// Since the UV's are already set when the emitter is created, 
	// we can alter that data to determine the general offset of this corner
	DirectX::XMFLOAT2 offset = particleUV[quadCornerIndex];
	offset.x = offset.x * 2 - 1;	// Convert from [0,1] to [-1,1]
	offset.y = (offset.y * -2 + 1);	// Same, but flip the Y

	// Load into a vector, which we'll assume is float3 with a Z of 0
	// Create a Z rotation matrix and apply it to the offset
	DirectX::XMVECTOR offsetVec = DirectX::XMLoadFloat2(&offset);

	// Add and scale the camera up/right vectors to the position as necessary
	DirectX::XMVECTOR posVec = DirectX::XMLoadFloat3(&particles[particleIndex].Position);
	posVec += camRight * DirectX::XMVectorGetX(offsetVec) * particles[particleIndex].Size;
	posVec += camUp * DirectX::XMVectorGetY(offsetVec) * particles[particleIndex].Size;

	// This position is all set
	DirectX::XMFLOAT3 pos;
	DirectX::XMStoreFloat3(&pos, posVec);
	return pos;
}

void ParticleEmitter::DrawParticles(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, std::shared_ptr<Camera> camera)
{
	//update final vertex positions and color before drawing
	for (int i = 0; i < pIndex; i++)
	{
		int j = i * 4;                //4 vertices per particle
		particleVertices[j].Position = CalcParticleVertexPosition(i, 0, camera);
		particleVertices[j + 1].Position = CalcParticleVertexPosition(i, 1, camera);
		particleVertices[j + 2].Position = CalcParticleVertexPosition(i, 2, camera);
		particleVertices[j + 3].Position = CalcParticleVertexPosition(i, 3, camera);

		particleVertices[j].Color = particles[i].Color;
		particleVertices[j + 1].Color = particles[i].Color;
		particleVertices[j + 2].Color = particles[i].Color;
		particleVertices[j + 3].Color = particles[i].Color;
	}

	//send vertices to dynamic buffer
	D3D11_MAPPED_SUBRESOURCE mResource;
	deviceContext->Map(vBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mResource);
	memcpy(mResource.pData, particleVertices, sizeof(ParticleVertex) * 4 * maxParticleCount);  //max particle array size
	deviceContext->Unmap(vBuffer.Get(), 0);

	UINT stride = sizeof(ParticleVertex);
	UINT offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, vBuffer.GetAddressOf(), &stride, &offset);
	deviceContext->IASetIndexBuffer(iBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	//prepare vs, ps, and set it
	material->PrepareMaterial(&transform, camera);

	//6 indices per particle
	deviceContext->DrawIndexed(
		pIndex * 6,
		0,
		0);

}

void ParticleEmitter::CreateBuffers(Microsoft::WRL::ComPtr<ID3D11Device> device)
{
	D3D11_BUFFER_DESC vBufferDesc = {};
	vBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vBufferDesc.ByteWidth = sizeof(ParticleVertex) * 4 * maxParticleCount;     //4 vertex
	vBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	device->CreateBuffer(&vBufferDesc, 0, vBuffer.GetAddressOf());

	unsigned int* indices = CreateConstantIndices();

	D3D11_SUBRESOURCE_DATA initialIndices = {};
	initialIndices.pSysMem = indices;

	D3D11_BUFFER_DESC iBufferDesc = {};
	vBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vBufferDesc.ByteWidth = sizeof(unsigned int) * 6 * maxParticleCount;     //6 index
	vBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	vBufferDesc.CPUAccessFlags = 0;

	device->CreateBuffer(&vBufferDesc, &initialIndices, iBuffer.GetAddressOf());

}

void ParticleEmitter::UpdateParticles(float dt, int pIndex)
{
	//update age for color and position calculation
	particles[pIndex].Age += dt;

	float ageRatio = particles[pIndex].Age / lifetime;

	//Determine position pn basis of age
	DirectX::XMVECTOR tempPosition = DirectX::XMLoadFloat3(&particles[pIndex].StartPosition);
	DirectX::XMVECTOR tempVelocity = DirectX::XMLoadFloat3(&particles[pIndex].StartVelocity);
	DirectX::XMVECTOR finalPosition = tempPosition + (tempVelocity * particles[pIndex].Age);
	DirectX::XMStoreFloat3(&particles[pIndex].Position, finalPosition);

	//Determine size on basis of age
	particles[pIndex].Size = startSize + (ageRatio * (endSize - startSize));

	//Determine color on basis of age
	DirectX::XMVECTOR newColor = DirectX::XMVectorLerp(DirectX::XMLoadFloat4(&startColor), DirectX::XMLoadFloat4(&endColor),
		ageRatio);
}

void ParticleEmitter::EmitParticles()
{
	if (pIndex <= maxParticleCount)
	{
		//set per particle position
		particles[pIndex].StartPosition = transform.GetPosition();
		particles[pIndex].Position = transform.GetPosition();

		//set per particle velocity
		particles[pIndex].StartVelocity = startVelocity;

		//set start size
		particles[pIndex].Size = startSize;
		
		//set start age
		particles[pIndex].Age = 0;
		
		//set start color
		particles[pIndex].Color = startColor;

		pIndex++;
	}
}

unsigned int* ParticleEmitter::CreateConstantIndices()
{
	unsigned int* constIndices = new unsigned int[maxParticleCount * 6];
	
	//fill with clockwise index
	for (int i = 0, j = 0; i < maxParticleCount * 4; i += 4, j += 6)
	{
		constIndices[j] =   i;
		constIndices[j+1] = i+1;
		constIndices[j+2] = i+2;
		constIndices[j+3] = i;
		constIndices[j+4] = i+2;
		constIndices[j+5] = i+3;
	}

	return constIndices;
}
