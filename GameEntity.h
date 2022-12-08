#pragma once
#include"Transformation.h"
#include"Mesh.h"
#include<memory>
#include"Material.h"
#include<wrl/client.h>
#include"Camera.h"

class GameEntity
{
private:
	Transformation transform;
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Material> material;

public:
	GameEntity(std::shared_ptr<Mesh> meshObject, std::shared_ptr<Material> materialobject);

	std::shared_ptr<Mesh> GetMesh();
	std::shared_ptr<Material> GetMaterial();
	Transformation* GetTransform();

	void SetMaterial(std::shared_ptr<Material> material);
	void SetMesh(std::shared_ptr<Mesh> mesh);

	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, std::shared_ptr<Camera> camera);
};
