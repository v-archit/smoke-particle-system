#include "GameEntity.h"

GameEntity::GameEntity(std::shared_ptr<Mesh> meshObject, std::shared_ptr<Material> materialObject)
{
	mesh = meshObject;
	material = materialObject;
}

std::shared_ptr<Mesh> GameEntity::GetMesh()
{
	return mesh;
}

std::shared_ptr<Material> GameEntity::GetMaterial()
{
	return material;
}

Transformation* GameEntity::GetTransform()
{
	return &transform;
}

void GameEntity::SetMaterial(std::shared_ptr<Material> material)
{
	this->material = material;
}

void GameEntity::SetMesh(std::shared_ptr<Mesh> mesh)
{
	this->mesh = mesh;
}

void GameEntity::Draw()
{
	mesh->Draw();
}


