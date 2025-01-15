#include "Camera.h"

// 指定方向を向く
void Camera::SetLookAt(const Vector3& eye, const Vector3& focus, const Vector3& up)
{
    using namespace DirectX;

    XMVECTOR Eye = XMLoadFloat3(&eye);
    XMVECTOR Focus = XMLoadFloat3(&focus);
    XMVECTOR Up = XMLoadFloat3(&up);
    XMMATRIX View = XMMatrixLookAtLH(Eye, Focus, Up);
    XMStoreFloat4x4(&data_.view_, View);

    // ビューを逆行列化し、ワールド行列に戻す
    DirectX::XMMATRIX World = DirectX::XMMatrixInverse(nullptr, View);
    DirectX::XMFLOAT4X4 world;
    DirectX::XMStoreFloat4x4(&world, World);

    // カメラの方向を取り出す
    this->data_.right_.x = world.m[0][0];
    this->data_.right_.y = world.m[0][1];
    this->data_.right_.z = world.m[0][2];

    this->data_.up_.x = world.m[1][0];
    this->data_.up_.y = world.m[1][1];
    this->data_.up_.z = world.m[1][2];

    this->data_.front_.x = world.m[2][0];
    this->data_.front_.y = world.m[2][1];
    this->data_.front_.z = world.m[2][2];

    // 視点、注視点を保存    
    this->data_.eye_ = eye;
    this->data_.focus_ = focus;
}

// パースペクティブ設定
void Camera::SetPerspectiveFov(float fovY, float aspect, float nearZ, float farZ)
{
    using namespace DirectX;
    data_.fovY_ = fovY;
    data_.aspect_ = aspect;
    data_.nearZ_ = nearZ;
    data_.farZ_ = farZ;

    // 画角、画面比率、クリップ距離からプロジェクション行列を作成
    XMMATRIX Projection =
        XMMatrixPerspectiveFovLH(fovY, aspect, nearZ, farZ);
    XMStoreFloat4x4(&data_.projection_, Projection);
}
