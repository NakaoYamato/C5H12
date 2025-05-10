#include "VECTOR.h"

#pragma region Vector2
#pragma region 演算子オーバーロード
Vector2& Vector2::operator=(const Vector2& v)
{
    x = v.x; y = v.y; return *this;
}
Vector2& Vector2::operator+=(const Vector2& v)
{
    x += v.x;
    y += v.y;
    return *this;
}
Vector2& Vector2::operator-=(const Vector2& v)
{
    x -= v.x;
    y -= v.y;
    return *this;
}
Vector2& Vector2::operator*=(float f)
{
    x *= f;
    y *= f;
    return *this;
}
Vector2& Vector2::operator/=(float f)
{
    x /= f;
    y /= f;
    return *this;
}
Vector2 Vector2::operator+() const
{
    return Vector2(x, y);
}
Vector2 Vector2::operator-() const
{
    return Vector2(-x, -y);
}
Vector2 Vector2::operator+(const Vector2& v) const
{
    return Vector2(x + v.x, y + v.y);
}
Vector2 Vector2::operator-(const Vector2& v) const
{
    return Vector2(x - v.x, y - v.y);
}
Vector2 Vector2::operator*(float f) const
{
    return Vector2(x * f, y * f);
}
Vector2 operator*(float f, const Vector2& v)
{
    return Vector2(v.x * f, v.y * f);
}
Vector2 Vector2::operator/(float f) const
{
    return Vector2(x / f, y / f);
}
bool Vector2::operator == (const Vector2& v) const
{
    return (x == v.x) && (y == v.y);
}
bool Vector2::operator != (const Vector2& v) const
{
    return (x != v.x) || (y != v.y);
}
#pragma endregion

#pragma region 静的メンバ関数
//  VECTOR2の長さの二乗を取得
float Vector2::LengthSq(const Vector2& v)
{
    return (v.x * v.x + v.y * v.y);
}
//  VECTOR2の長さを取得
float Vector2::Length(const Vector2& v)
{
    return sqrtf(LengthSq(v));
}
//  VECTOR2を単位化
Vector2 Vector2::Normalize(const Vector2& v)
{
    float d = Length(v);
    return d != 0.0f ? v / d : v;
}
//  VECTOR2対VECTOR2の内積
float Vector2::Dot(const Vector2& src, const Vector2& dst)
{
    return DirectX::XMVectorGetX(
        DirectX::XMVector2Dot(DirectX::XMLoadFloat2(&src), DirectX::XMLoadFloat2(&dst))
    );
}
//  VECTOR2対VECTOR2の外積
Vector2 Vector2::Cross(const Vector2& src, const Vector2& dst)
{
    Vector2 v = {};
    DirectX::XMStoreFloat2(&v,
        DirectX::XMVector2Cross(DirectX::XMLoadFloat2(&src), DirectX::XMLoadFloat2(&dst)));
    return v;
}
//  srcとdstで保管処理
Vector2 Vector2::Lerp(const Vector2& src, const Vector2& dst, float t, float(*Easing)(float))
{
    return Vector2(
        EasingLerp(src.x, dst.x, t, Easing),
        EasingLerp(src.y, dst.y, t, Easing)
    );
}
#pragma endregion
#pragma endregion

#pragma region Vector3
#pragma region 演算子オーバーロード
Vector3& Vector3::operator=(const Vector3& v)
{
    x = v.x;
    y = v.y;
    z = v.z;
    return *this;
}
Vector3& Vector3::operator+=(const Vector3& v)
{
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
}
Vector3& Vector3::operator-=(const Vector3& v)
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
}
Vector3& Vector3::operator*=(float f)
{
    x *= f;
    y *= f;
    z *= f;
    return *this;
}
Vector3& Vector3::operator/=(float f)
{
    x /= f;
    y /= f;
    z /= f;
    return *this;
}
Vector3 Vector3::operator+() const
{
    return Vector3(x, y, z);
}
Vector3 Vector3::operator-() const
{
    return Vector3(-x, -y, -z);
}
Vector3 Vector3::operator+(const Vector3& v) const
{
    return Vector3(x + v.x, y + v.y, z + v.z);
}
Vector3 Vector3::operator-(const Vector3& v) const
{
    return Vector3(x - v.x, y - v.y, z - v.z);
}
Vector3 Vector3::operator*(float f) const
{
    return Vector3(x * f, y * f, z * f);
}
Vector3 operator*(float f, const Vector3& v)
{
    return Vector3(v.x * f, v.y * f, v.z * f);
}
Vector3 Vector3::operator/(float f) const
{
    return Vector3(x / f, y / f, z / f);
}
bool Vector3::operator == (const Vector3& v) const
{
    return (x == v.x) && (y == v.y) && (z == v.z);
}
bool Vector3::operator != (const Vector3& v) const
{
    return (x != v.x) || (y != v.y) || (z != v.z);
}
#pragma endregion

#pragma region 静的メンバ関数
//  VECTOR3の長さの二乗を取得
float Vector3::LengthSq(const Vector3& v)
{
    return (v.x * v.x + v.y * v.y + v.z * v.z);
}
//  VECTOR3の長さを取得
float Vector3::Length(const Vector3& v)
{
    return sqrtf(Vector3::LengthSq(v));
}
//  VECTOR3を単位化
Vector3 Vector3::Normalize(const Vector3& v)
{
    float d = Vector3::Length(v);
    return d != 0.0f ? v / d : v;
}
//  VECTOR3対VECTOR3の内積
float Vector3::Dot(const Vector3& src, const Vector3& dst)
{
    return DirectX::XMVectorGetX(
        DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&src), DirectX::XMLoadFloat3(&dst))
    );
}
//  VECTOR3対VECTOR3の外積
Vector3 Vector3::Cross(const Vector3& src, const Vector3& dst)
{
    Vector3 v = {};
    DirectX::XMStoreFloat3(&v,
        DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&src), DirectX::XMLoadFloat3(&dst)));
    return v;
}
//  srcとdstで保管処理
//  t   : 経過時間(0.0f ~ 1.0f)
Vector3 Vector3::Lerp(const Vector3& src, const Vector3& dst, float t, float(*Easing)(float))
{
    return Vector3(
        EasingLerp(src.x, dst.x, t, Easing),
        EasingLerp(src.y, dst.y, t, Easing),
        EasingLerp(src.z, dst.z, t, Easing)
    );
}
//  VECTOR3を度数法に変換
Vector3 Vector3::ToDegrees(const Vector3& v)
{
    Vector3 d{};
    d.x = DirectX::XMConvertToDegrees(v.x);
    d.y = DirectX::XMConvertToDegrees(v.y);
    d.z = DirectX::XMConvertToDegrees(v.z);
    return d;
}
//  VECTOR3を弧度法に変換
Vector3 Vector3::ToRadians(const Vector3& v)
{
    Vector3 d{};
    d.x = DirectX::XMConvertToRadians(v.x);
    d.y = DirectX::XMConvertToRadians(v.y);
    d.z = DirectX::XMConvertToRadians(v.z);
    return d;
}
//  回転行列からオイラー角取得(回転順ZXY XMMatrixRotationRollPitchYawはZXYの順番で回転している)
Vector3 Vector3::FromRotationMatrix(const DirectX::XMFLOAT4X4& m)
{
    // 参考資料:https://qiita.com/aa_debdeb/items/3d02e28fb9ebfa357eaf
    Vector3 r{};
    r.x = asinf(m.m[2][1]);

    const float cosX = cosf(r.x);
    if (cosX != 0.0f)
    {
        r.y = atanf(-m.m[2][0] / m.m[2][1]);
        r.z = atanf(-m.m[0][1] / m.m[1][1]);
    }
    else
    {
        // シンバルロック
        // r.y が0と仮定
        r.y = 0.0f;
        r.z = atanf(m.m[1][0] / m.m[0][0]);
    }

    return r;
}
//  回転行列からオイラー角取得(回転順ZXY XMMatrixRotationRollPitchYawはZXYの順番で回転している)
Vector3 Vector3::FromRotationMatrix(const DirectX::XMMATRIX& R)
{
    DirectX::XMFLOAT4X4 r{};
    DirectX::XMStoreFloat4x4(&r, R);
    return Vector3::FromRotationMatrix(r);
}
// 行列との掛け算
Vector3 Vector3::TransformCoord(const Vector3& src, const DirectX::XMFLOAT4X4& m)
{
    return Vector3::TransformCoord(src, DirectX::XMLoadFloat4x4(&m));
}
// 行列との掛け算
Vector3 Vector3::TransformCoord(const Vector3& src, const DirectX::XMMATRIX& M)
{
    Vector3 res{};
    DirectX::XMStoreFloat3(&res,
        DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&src), M));
    return res;
}
// 行列との掛け算
Vector3 Vector3::TransformNormal(const Vector3& src, const DirectX::XMFLOAT4X4& m)
{
    return Vector3::TransformNormal(src, DirectX::XMLoadFloat4x4(&m));
}
// 行列との掛け算
Vector3 Vector3::TransformNormal(const Vector3& src, const DirectX::XMMATRIX& M)
{
    Vector3 res{};
    DirectX::XMStoreFloat3(&res,
        DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&src), M));
    return res;
}
// 各軸の角度取得
Vector3 Vector3::CalcAngle(const Vector3& normalVec)
{
    Vector3 res{};
    res.x = std::acosf(normalVec.x);
    res.y = std::acosf(normalVec.y);
    res.z = std::acosf(normalVec.z);
    return res;
}
#pragma endregion

#pragma endregion

#pragma region Vector4
Vector4 Vector4::operator*(float f) const
{
    return Vector4(x * f, y * f, z * f, w);
}

Vector4 Vector4::operator*(const Vector4& v) const
{
    return Vector4(x * v.x, y * v.y, z * v.z, w * v.w);
}
#pragma endregion

