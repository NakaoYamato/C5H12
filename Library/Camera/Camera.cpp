#include "Camera.h"

// 指定方向を向く
void Camera::SetLookAt(const Vector3& eye, const Vector3& focus, const Vector3& up)
{
    using namespace DirectX;

    XMVECTOR Eye = XMLoadFloat3(&eye);
    XMVECTOR Focus = XMLoadFloat3(&focus);
    XMVECTOR Up = XMLoadFloat3(&up);
    XMMATRIX View = XMMatrixLookAtLH(Eye, Focus, Up);
    XMStoreFloat4x4(&_data.view, View);

    // ビューを逆行列化し、ワールド行列に戻す
    DirectX::XMMATRIX World = DirectX::XMMatrixInverse(nullptr, View);
    DirectX::XMFLOAT4X4 world;
    DirectX::XMStoreFloat4x4(&world, World);

    // カメラの方向を取り出す
    this->_data.right.x = world.m[0][0];
    this->_data.right.y = world.m[0][1];
    this->_data.right.z = world.m[0][2];

    this->_data.up.x = world.m[1][0];
    this->_data.up.y = world.m[1][1];
    this->_data.up.z = world.m[1][2];

    this->_data.front.x = world.m[2][0];
    this->_data.front.y = world.m[2][1];
    this->_data.front.z = world.m[2][2];

    // 視点、注視点を保存    
    this->_data.eye = eye;
    this->_data.focus = focus;
}

// パースペクティブ設定
void Camera::SetPerspectiveFov(float fovY, float aspect, float nearZ, float farZ)
{
    using namespace DirectX;
    _data.fovY = fovY;
    _data.aspect = aspect;
    _data.nearZ = nearZ;
    _data.farZ = farZ;

    // 画角、画面比率、クリップ距離からプロジェクション行列を作成
    XMMATRIX Projection =
        XMMatrixPerspectiveFovLH(fovY, aspect, nearZ, farZ);
    XMStoreFloat4x4(&_data.projection, Projection);
}
