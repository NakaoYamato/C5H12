#include "VECTOR.h"

//--------------------------------------------------------------
//  VECTOR2の長さの二乗を取得
float Vec2LengthSq(const Vector2& v)
{
    return (v.x * v.x + v.y * v.y);
}
//--------------------------------------------------------------
//  VECTOR2の長さを取得
float Vec2Length(const Vector2& v)
{
    return sqrtf(Vec2LengthSq(v));
}
//--------------------------------------------------------------
//  VECTOR2を単位化
Vector2 Vec2Normalize(const Vector2& v)
{
    float d = Vec2Length(v);
    return d != 0.0f ? v / d : v;
}
//--------------------------------------------------------------
//  VECTOR2対VECTOR2の内積
float Vec2Dot(const Vector2& src, const Vector2& dst)
{
    return DirectX::XMVectorGetX(
        DirectX::XMVector2Dot(DirectX::XMLoadFloat2(&src), DirectX::XMLoadFloat2(&dst))
    );
}
//--------------------------------------------------------------
//  VECTOR2対VECTOR2の外積
Vector2 Vec2Cross(const Vector2& src, const Vector2& dst)
{
    Vector2 v = {};
    DirectX::XMStoreFloat2(&v,
        DirectX::XMVector2Cross(DirectX::XMLoadFloat2(&src), DirectX::XMLoadFloat2(&dst)));
    return v;
}
//--------------------------------------------------------------
//  srcとdstで保管処理
Vector2 Vec2Lerp(const Vector2& src, const Vector2& dst, float t, float(*Easing)(float))
{
    return Vector2(
        EasingLerp(src.x, dst.x, t, Easing),
        EasingLerp(src.y, dst.y, t, Easing)
    );
}
//--------------------------------------------------------------
//  VECTOR3の長さの二乗を取得
float   Vec3LengthSq(const Vector3& v)
{
    return (v.x * v.x + v.y * v.y + v.z * v.z);
}
//--------------------------------------------------------------
//  VECTOR3の長さを取得
float   Vec3Length(const Vector3& v)
{
    return sqrtf(Vec3LengthSq(v));
}
//--------------------------------------------------------------
//  VECTOR3を単位化
Vector3 Vec3Normalize(const Vector3& v)
{
    float d = Vec3Length(v);
    return d != 0.0f ? v / d : v;
}
//--------------------------------------------------------------
//  VECTOR3対VECTOR3の内積
float Vec3Dot(const Vector3& src, const Vector3& dst)
{
    return DirectX::XMVectorGetX(
        DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&src), DirectX::XMLoadFloat3(&dst))
    );
}
//--------------------------------------------------------------
//  VECTOR3対VECTOR3の外積
Vector3 Vec3Cross(const Vector3& src, const Vector3& dst)
{
    Vector3 v = {};
    DirectX::XMStoreFloat3(&v,
        DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&src), DirectX::XMLoadFloat3(&dst)));
    return v;
}

Vector3 Vec3ConvertToDegrees(const Vector3& v)
{
    Vector3 d{};
    d.x = DirectX::XMConvertToDegrees(v.x);
    d.y = DirectX::XMConvertToDegrees(v.y);
    d.z = DirectX::XMConvertToDegrees(v.z);
    return d;
}

Vector3 Vec3ConvertToRadians(const Vector3& v)
{
    Vector3 d{};
    d.x = DirectX::XMConvertToRadians(v.x);
    d.y = DirectX::XMConvertToRadians(v.y);
    d.z = DirectX::XMConvertToRadians(v.z);
    return d;
}

Vector3 Vec3Lerp(const Vector3& src, const Vector3& dst, float t, float(*Easing)(float))
{
    return Vector3(
        EasingLerp(src.x, dst.x, t, Easing),
        EasingLerp(src.y, dst.y, t, Easing),
        EasingLerp(src.z, dst.z, t, Easing)
    );
}

Vector3 Vec3FromRotationMatrix(const DirectX::XMMATRIX& R)
{
    DirectX::XMFLOAT4X4 r{};
    DirectX::XMStoreFloat4x4(&r, R);
    return Vec3FromRotationMatrix(r);
}

Vector3 Vec3TransformCoord(const Vector3& src, const DirectX::XMFLOAT4X4& m)
{
    return Vec3TransformCoord(src, DirectX::XMLoadFloat4x4(&m));
}

//--------------------------------------------------------------
// 行列との掛け算
Vector3 Vec3TransformCoord(const Vector3& src, const DirectX::XMMATRIX& M)
{
    Vector3 res{};
    DirectX::XMStoreFloat3(&res,
        DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&src), M));
    return res;
}

Vector3 Vec3TransformNormal(const Vector3& src, const DirectX::XMFLOAT4X4& m)
{
    return Vec3TransformNormal(src, DirectX::XMLoadFloat4x4(&m));
}

Vector3 Vec3TransformNormal(const Vector3& src, const DirectX::XMMATRIX& M)
{
    Vector3 res{};
    DirectX::XMStoreFloat3(&res,
        DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&src), M));
    return res;
}

Vector3 Vec3CalcAngle(const Vector3& normalVec)
{
    Vector3 res{};
    res.x = std::acosf(normalVec.x);
    res.y = std::acosf(normalVec.y);
    res.z = std::acosf(normalVec.z);
    return res;
}

Vector3 Vec3FromRotationMatrix(const DirectX::XMFLOAT4X4& m)
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


//--------------------------------------------------
// VECTOR2
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


//--------------------------------------------------
// 構造体
//--------------------------------------------------
//--------------------------------------------------
// VECTOR3
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

Vector4 Vector4::operator*(const Vector4& v) const
{
    return Vector4(x * v.x, y * v.y, z * v.z, w * v.w);
}
