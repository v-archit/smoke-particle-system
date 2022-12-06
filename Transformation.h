#include<DirectXMath.h>

#pragma once

class Transformation
{
private:
	DirectX::XMFLOAT4X4 world;
	DirectX::XMFLOAT4X4 worldInverse;
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 scale;
	DirectX::XMFLOAT3 rotation;

	bool IsWorldDirty;
	bool IsRotationChanged;

	DirectX::XMFLOAT3 rightAxis;
	DirectX::XMFLOAT3 forwardAxis;
	DirectX::XMFLOAT3 upAxis;

public:

	Transformation()
	{
		DirectX::XMStoreFloat4x4(&world, DirectX::XMMatrixIdentity());
		DirectX::XMStoreFloat4x4(&worldInverse, DirectX::XMMatrixIdentity());
		
		position = DirectX::XMFLOAT3(0, 0, 0);
		scale = DirectX::XMFLOAT3(1, 1, 1);
		rotation = DirectX::XMFLOAT3(0, 0, 0);

		IsWorldDirty = false;
		IsRotationChanged = false;

		rightAxis = DirectX::XMFLOAT3(1, 0, 0);
		forwardAxis = DirectX::XMFLOAT3(0, 0, 1);
		upAxis = DirectX::XMFLOAT3(0, 1, 0);
	}

	~Transformation();

	void SetPosition(float x, float y, float z);
	void SetRotation(float pitch, float yaw, float roll);
	void SetScale(float x, float y, float z);

	DirectX::XMFLOAT3 GetPosition();
	DirectX::XMFLOAT3 GetScale();
	DirectX::XMFLOAT3 GetRotation();
	DirectX::XMFLOAT4X4 GetWorldMatrix();
	DirectX::XMFLOAT4X4 GetWorldInverseMatrix();
	DirectX::XMFLOAT3 GetRight();
	DirectX::XMFLOAT3 GetUp();
	DirectX::XMFLOAT3 GetForward();

	void MoveAbsolute(float x, float y, float z);
	void Rotate(float pitch, float yaw, float roll);
	void Scale(float x, float y, float z);
	void MoveRelative(float x, float y, float z);

	void UpdateMatrices();
	void UpdateOrientation();
};
