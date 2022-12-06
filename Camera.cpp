#include "Camera.h"
#include"Input.h"

void Camera::UpdateProjectionMatrix(float aspectRatio)
{
    DirectX::XMStoreFloat4x4(&projectionMatrix, DirectX::XMMatrixPerspectiveFovLH(fieldOfView, aspectRatio, nearDistance, farDistance));
}

void Camera::UpdateViewMatrix()
{
    DirectX::XMFLOAT3 tempPosition = transform.GetPosition();
    DirectX::XMFLOAT3 tempForward = transform.GetForward();

    DirectX::XMVECTOR positionVector = DirectX::XMLoadFloat3(&tempPosition);
    DirectX::XMVECTOR forwardVector = DirectX::XMLoadFloat3(&tempForward);
    DirectX::XMVECTOR worldUpVector = DirectX::XMVectorSet(0, 1, 0, 0);

    DirectX::XMStoreFloat4x4(&viewMatrix, DirectX::XMMatrixLookToLH(positionVector, forwardVector, worldUpVector));
}

void Camera::Update(float dt)
{
    Input& input = Input::GetInstance();

    if (input.KeyDown('W'))
    {
        transform.MoveRelative(0, 0, movementSpeed * dt);
    }
    if (input.KeyDown('A'))
    {
        transform.MoveRelative(-movementSpeed * dt, 0, 0);
    }
    if (input.KeyDown('S'))
    {
        transform.MoveRelative(0, 0, -movementSpeed * dt);
    }
    if (input.KeyDown('D'))
    {
        transform.MoveRelative(movementSpeed * dt, 0, 0);
    }
    if (input.KeyDown(VK_SPACE))
    {
        transform.MoveAbsolute(0, movementSpeed * dt, 0);
    }
    if (input.KeyDown('X'))
    {
        transform.MoveAbsolute(0, -movementSpeed * dt, 0);
    }

    if (input.MouseRightDown())
    {
        int cursorMovementX = input.GetMouseXDelta();
        int cursorMovementY = input.GetMouseYDelta();

        transform.Rotate(cursorMovementY * mouseLookSpeed * dt, cursorMovementX * mouseLookSpeed * dt, 0);

        float clampRotationX = transform.GetRotation().x;
        if (clampRotationX < -DirectX::XM_PIDIV2)
            clampRotationX = -DirectX::XM_PIDIV2;
        else if (clampRotationX > DirectX::XM_PIDIV2)
            clampRotationX = DirectX::XM_PIDIV2;
        else
            clampRotationX = clampRotationX;

        transform.SetRotation(clampRotationX, transform.GetRotation().y, transform.GetRotation().z);
    }

    UpdateViewMatrix();
}

Camera::Camera(float aspectRatio, DirectX::XMFLOAT3 initialPosition, DirectX::XMFLOAT3 startOrientation, float fieldOfView, float nearDistance, float farDistance, float movementSpeed, float mouseLookSpeed, bool isPerspective)
{
    transform.SetPosition(initialPosition.x, initialPosition.y, initialPosition.z);
    transform.SetRotation(startOrientation.x, startOrientation.y, startOrientation.z);

    this->fieldOfView = fieldOfView;
    this->nearDistance = nearDistance;
    this->farDistance = farDistance;
    this->movementSpeed = movementSpeed;
    this->mouseLookSpeed = mouseLookSpeed;
    this->isPerspective = isPerspective;

    UpdateViewMatrix();
    UpdateProjectionMatrix(aspectRatio);
}

Camera::~Camera()
{
}

Transformation* Camera::GetTransform()
{
    return &transform;
}

DirectX::XMFLOAT4X4 Camera::GetViewMatrix()
{
    return viewMatrix;
}

DirectX::XMFLOAT4X4 Camera::GetProjectionMatrix()
{
    return projectionMatrix;
}
