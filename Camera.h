#pragma once

#include"Transformation.h"
#include<DirectXMath.h>

class Camera
{
private:
	Transformation transform;
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projectionMatrix;
	
	float fieldOfView;
	float nearDistance;
	float farDistance;
	float movementSpeed;
	float mouseLookSpeed;

	bool isPerspective;

public:
	Camera(float aspectRatio, DirectX::XMFLOAT3 initialPosition, DirectX::XMFLOAT3 startOrientation, float fieldOfView, float nearDistance, float farDistance, float movementSpeed, float mouseLookSpeed, bool isPerspective);
	~Camera();

	DirectX::XMFLOAT4X4 GetViewMatrix();
	Transformation* GetTransform();
	DirectX::XMFLOAT4X4 GetProjectionMatrix();

	void UpdateProjectionMatrix(float aspectRatio);
	void UpdateViewMatrix();
	void Update(float dt);
};
