#include "Transformation.h"

Transformation::~Transformation()
{
}

void Transformation::SetPosition(float x, float y, float z)
{
    position = DirectX::XMFLOAT3(x, y, z);

    IsWorldDirty = true;
}

void Transformation::SetRotation(float pitch, float yaw, float roll)
{
    rotation = DirectX::XMFLOAT3(pitch, yaw, roll);

    IsWorldDirty = true;
    IsRotationChanged = true;
}

void Transformation::SetScale(float x, float y, float z)
{
    scale = DirectX::XMFLOAT3(x, y, z);

    IsWorldDirty = true;
}

DirectX::XMFLOAT3 Transformation::GetPosition()
{
    return position;
}

DirectX::XMFLOAT3 Transformation::GetScale()
{
    return scale;
}

DirectX::XMFLOAT3 Transformation::GetRotation()
{
    return rotation;
}

DirectX::XMFLOAT4X4 Transformation::GetWorldMatrix()
{
    if (IsWorldDirty)
    {
        UpdateMatrices();
    }
    return world;
}

DirectX::XMFLOAT4X4 Transformation::GetWorldInverseMatrix()
{
    if (IsWorldDirty)
    {
        UpdateMatrices();
    }
    return worldInverse;
}

DirectX::XMFLOAT3 Transformation::GetRight()
{
    if (IsRotationChanged)
        UpdateOrientation();
    return rightAxis;
}

DirectX::XMFLOAT3 Transformation::GetForward()
{
    if (IsRotationChanged)
        UpdateOrientation();
    return forwardAxis;
}

DirectX::XMFLOAT3 Transformation::GetUp()
{
    if (IsRotationChanged)
        UpdateOrientation();
    return upAxis;
}

void Transformation::MoveAbsolute(float x, float y, float z)
{
    DirectX::XMVECTOR temp = DirectX::XMLoadFloat3(&position);
    DirectX::XMVECTOR tempVector = DirectX::XMVectorSet(x, y, z, 0.0);
    temp = DirectX::XMVectorAdd(temp, tempVector);
    DirectX::XMStoreFloat3(&position, temp);

    IsWorldDirty = true;
}

void Transformation::Rotate(float pitch, float yaw, float roll)
{
    DirectX::XMVECTOR temp = DirectX::XMLoadFloat3(&rotation);
    DirectX::XMVECTOR tempVector = DirectX::XMVectorSet(pitch, yaw, roll, 0.0);
    temp = DirectX::XMVectorAdd(temp, tempVector);
    DirectX::XMStoreFloat3(&rotation, temp);

    IsWorldDirty = true;
    IsRotationChanged = true;
}

void Transformation::Scale(float x, float y, float z)
{
    DirectX::XMVECTOR temp = DirectX::XMLoadFloat3(&scale);
    DirectX::XMVECTOR tempVector = DirectX::XMVectorSet(x, y, z, 0.0);
    temp = DirectX::XMVectorMultiply(temp, tempVector);
    DirectX::XMStoreFloat3(&scale, temp);

    IsWorldDirty = true;
}

void Transformation::MoveRelative(float x, float y, float z)
{
    DirectX::XMVECTOR tempRotation = DirectX::XMQuaternionRotationRollPitchYawFromVector(DirectX::XMLoadFloat3(&rotation));
    DirectX::XMVECTOR relative = DirectX::XMVector3Rotate(DirectX::XMVectorSet(x, y, z, 0), tempRotation);
    DirectX::XMStoreFloat3(&position, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&position), relative));

    IsWorldDirty = true;
}

void Transformation::UpdateMatrices()
{
    DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

    DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);

    DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);

    DirectX::XMMATRIX worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;

    DirectX::XMStoreFloat4x4(&world, worldMatrix);
    DirectX::XMStoreFloat4x4(&worldInverse, DirectX::XMMatrixInverse(0, DirectX::XMMatrixTranspose(worldMatrix)));

    IsWorldDirty = false;
}

void Transformation::UpdateOrientation()
{
    DirectX::XMVECTOR tempRotation = DirectX::XMQuaternionRotationRollPitchYawFromVector(DirectX::XMLoadFloat3(&rotation));
    
    DirectX::XMStoreFloat3(&rightAxis, DirectX::XMVector3Rotate(DirectX::XMVectorSet(1, 0, 0, 0), tempRotation));
    DirectX::XMStoreFloat3(&forwardAxis, DirectX::XMVector3Rotate(DirectX::XMVectorSet(0, 0, 1, 0), tempRotation));
    DirectX::XMStoreFloat3(&upAxis, DirectX::XMVector3Rotate(DirectX::XMVectorSet(0, 1, 0, 0), tempRotation));
    
    IsRotationChanged = false;
}

