#include "Camera.h"

// 指定方向を向く
void Camera::SetLookAt(const Vector3& eye, const Vector3& focus, const Vector3& up)
{
    DirectX::XMVECTOR Eye = DirectX::XMLoadFloat3(&eye);
    DirectX::XMVECTOR Focus = DirectX::XMLoadFloat3(&focus);
    DirectX::XMVECTOR Up = DirectX::XMLoadFloat3(&up);
    DirectX::XMMATRIX View = DirectX::XMMatrixLookAtLH(Eye, Focus, Up);
    DirectX::XMStoreFloat4x4(&this->_view, View);

    // ビューを逆行列化し、ワールド行列に戻す
    DirectX::XMMATRIX World = DirectX::XMMatrixInverse(nullptr, View);
    DirectX::XMFLOAT4X4 world;
    DirectX::XMStoreFloat4x4(&world, World);

    // カメラの方向を取り出す
    _right.x = world.m[0][0];
    _right.y = world.m[0][1];
    _right.z = world.m[0][2];

    _up.x = world.m[1][0];
    _up.y = world.m[1][1];
    _up.z = world.m[1][2];

    _front.x = world.m[2][0];
    _front.y = world.m[2][1];
    _front.z = world.m[2][2];

    // 視点、注視点を保存    
    _eye = eye;
    _focus = focus;
}

// パースペクティブ設定
void Camera::SetPerspectiveFov(float fovY, float aspect, float nearZ, float farZ)
{
    fovY = fovY;
    aspect = aspect;
    nearZ = nearZ;
    farZ = farZ;

    // 画角、画面比率、クリップ距離からプロジェクション行列を作成
    DirectX::XMMATRIX Projection =
        DirectX::XMMatrixPerspectiveFovLH(fovY, aspect, nearZ, farZ);
    DirectX::XMStoreFloat4x4(&this->_projection, Projection);
}
