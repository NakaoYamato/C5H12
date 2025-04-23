#include "Camera.h"

void Camera::SetLookAt(const Vector3& eye, const Vector3& focus, const Vector3& up)
{
    DirectX::XMVECTOR Eye = DirectX::XMLoadFloat3(&eye);
    DirectX::XMVECTOR Focus = DirectX::XMLoadFloat3(&focus);
    DirectX::XMVECTOR Up = DirectX::XMLoadFloat3(&up);
    DirectX::XMMATRIX View = DirectX::XMMatrixLookAtLH(Eye, Focus, Up);
    DirectX::XMStoreFloat4x4(&this->view, View);

    // ビューを逆行列化し、ワールド行列に戻す
    DirectX::XMMATRIX World = DirectX::XMMatrixInverse(nullptr, View);
    DirectX::XMFLOAT4X4 world;
    DirectX::XMStoreFloat4x4(&world, World);

    // カメラの方向を取り出す
    this->right.x = world.m[0][0];
    this->right.y = world.m[0][1];
    this->right.z = world.m[0][2];

    this->up.x = world.m[1][0];
    this->up.y = world.m[1][1];
    this->up.z = world.m[1][2];

    this->front.x = world.m[2][0];
    this->front.y = world.m[2][1];
    this->front.z = world.m[2][2];

    // 視点、注視点を保存    
    this->eye = eye;
    this->focus = focus;
}

void Camera::SetPerspectiveFov(float fovY, float aspect, float nearZ, float farZ)
{
    using namespace DirectX;
    fovY = fovY;
    aspect = aspect;
    nearZ = nearZ;
    farZ = farZ;

    // 画角、画面比率、クリップ距離からプロジェクション行列を作成
    XMMATRIX Projection =
        XMMatrixPerspectiveFovLH(fovY, aspect, nearZ, farZ);
    XMStoreFloat4x4(&this->projection, Projection);
}
