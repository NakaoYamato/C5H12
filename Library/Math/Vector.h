#pragma once

#include "Easing.h"

#include <DirectXMath.h>
//**************************************************************
// XMFLOAT の便利化
//**************************************************************
//------< プロトタイプ宣言 >-------------------------
class Vector2;
class Vector3;
class Vector4;

//--------------------------------------------------
// マクロ
//--------------------------------------------------
#define VECTOR2_ZERO Vector2(0.0f, 0.0f)
#define VECTOR3_ZERO Vector3(0.0f, 0.0f, 0.0f)
#define VECTOR4_ZERO Vector4(0.0f, 0.0f, 0.0f, 0.0f)
#define VECTOR4_BLACK Vector4(0.0f, 0.0f, 0.0f, 0.0f)
#define VECTOR4_WHITE Vector4(1.0f, 1.0f, 1.0f, 1.0f)
#define VECTOR4_RED Vector4(1.0f, 0.0f, 0.0f, 1.0f)
#define VECTOR4_GREEN Vector4(0.0f, 1.0f, 0.0f, 1.0f)
#define VECTOR4_BLUE Vector4(0.0f, 0.0f, 1.0f, 1.0f)

//--------------------------------------------------
// 関数
//--------------------------------------------------
//--------------------------------------------------------------
//  VECTOR2の長さの二乗を取得
float   Vec2LengthSq(const Vector2&);
//--------------------------------------------------------------
//  VECTOR2の長さを取得
float   Vec2Length(const Vector2&);
//--------------------------------------------------------------
//  VECTOR2を単位化
Vector2 Vec2Normalize(const Vector2&);
//--------------------------------------------------------------
//  VECTOR2対VECTOR2の内積
float Vec2Dot(const Vector2& src, const Vector2& dst);
//--------------------------------------------------------------
//  VECTOR2対VECTOR2の外積
Vector2 Vec2Cross(const Vector2& src, const Vector2& dst);
//--------------------------------------------------------------
//  srcとdstで保管処理
//  t   : 経過時間(0.0f ~ 1.0f)
Vector2 Vec2Lerp(const Vector2& src, const Vector2& dst, float t, float (*Easing)(float) = nullptr);
//--------------------------------------------------------------
//  VECTOR3の長さの二乗を取得
float   Vec3LengthSq(const Vector3&);
//--------------------------------------------------------------
//  VECTOR3の長さを取得
float   Vec3Length(const Vector3&);
//--------------------------------------------------------------
//  VECTOR3を単位化
Vector3 Vec3Normalize(const Vector3&);
//--------------------------------------------------------------
//  VECTOR3対VECTOR3の内積
float Vec3Dot(const Vector3& src, const Vector3& dst);
//--------------------------------------------------------------
//  VECTOR3対VECTOR3の外積
Vector3 Vec3Cross(const Vector3& src, const Vector3& dst);
//--------------------------------------------------------------
//  VECTOR3を度数法に変換
Vector3 Vec3ConvertToDegrees(const Vector3& v);
//--------------------------------------------------------------
//  VECTOR3を弧度法に変換
Vector3 Vec3ConvertToRadians(const Vector3& v);
//--------------------------------------------------------------
//  srcとdstで保管処理
//  t   : 経過時間(0.0f ~ 1.0f)
Vector3 Vec3Lerp(const Vector3& src, const Vector3& dst, float t, float (*Easing)(float) = nullptr);
//--------------------------------------------------------------
//  回転行列からオイラー角取得(回転順ZXY XMMatrixRotationRollPitchYawはZXYの順番で回転している)
Vector3 Vec3FromRotationMatrix(const DirectX::XMFLOAT4X4& r);
Vector3 Vec3FromRotationMatrix(const DirectX::XMMATRIX& R);
//--------------------------------------------------------------
// 行列との掛け算
Vector3 Vec3TransformCoord(const Vector3& src, const DirectX::XMFLOAT4X4& m);
Vector3 Vec3TransformCoord(const Vector3& src, const DirectX::XMMATRIX& M);
Vector3 Vec3TransformNormal(const Vector3& src, const DirectX::XMFLOAT4X4& m);
Vector3 Vec3TransformNormal(const Vector3& src, const DirectX::XMMATRIX& M);
//--------------------------------------------------------------
// 各軸の角度取得
/// <param name="normalVec">正規化されたベクトル</param>
/// <returns>ラジアン</returns>
Vector3 Vec3CalcAngle(const Vector3& normalVec);


//--------------------------------------------------
// 構造体
//--------------------------------------------------
// VECTOR2
class Vector2 : public DirectX::XMFLOAT2
{
public:
    Vector2() : DirectX::XMFLOAT2(0, 0) {};
    Vector2(float x, float y) : DirectX::XMFLOAT2(x, y) {};
    Vector2(int x, int y) : DirectX::XMFLOAT2(static_cast<float>(x), static_cast<float>(y)) {};
    Vector2(const Vector2& v) { x = v.x; y = v.y; }
    Vector2(const DirectX::XMFLOAT2& v) { x = v.x; y = v.y; }
    ~Vector2() {};

    Vector2& operator=(const Vector2&);
    Vector2& operator+=(const Vector2&);
    Vector2& operator-=(const Vector2&);
    Vector2& operator*=(float);
    Vector2& operator/=(float);

    Vector2 operator+() const;
    Vector2 operator-() const;

    Vector2 operator+(const Vector2&) const;
    Vector2 operator-(const Vector2&) const;
    Vector2 operator*(float) const;
    friend Vector2 operator*(float, const Vector2&);
    Vector2 operator/(float) const;

    bool operator == (const Vector2&) const;
    bool operator != (const Vector2&) const;
};
//--------------------------------------------------
// VECTOR3
class Vector3 : public DirectX::XMFLOAT3
{
public:
    Vector3() : DirectX::XMFLOAT3(0, 0, 0) {};
    Vector3(float x, float y, float z) : DirectX::XMFLOAT3(x, y, z) {};
    Vector3(const Vector3& v) { x = v.x; y = v.y; z = v.z; }
    Vector3(const DirectX::XMFLOAT3& v) { x = v.x; y = v.y; z = v.z; }
    ~Vector3() {};

    Vector3& operator=(const Vector3&);
    Vector3& operator+=(const Vector3&);
    Vector3& operator-=(const Vector3&);
    Vector3& operator*=(float);
    Vector3& operator/=(float);

    Vector3 operator+() const;
    Vector3 operator-() const;

    Vector3 operator+(const Vector3&) const;
    Vector3 operator-(const Vector3&) const;
    Vector3 operator*(float) const;
    friend Vector3 operator*(float, const Vector3&);
    Vector3 operator/(float) const;

    bool operator == (const Vector3&) const;
    bool operator != (const Vector3&) const;
};
//--------------------------------------------------
// VECTOR4
class Vector4 : public DirectX::XMFLOAT4
{
public:
    Vector4() : DirectX::XMFLOAT4(0, 0, 0, 0) {};
    Vector4(float x, float y, float z, float w) : DirectX::XMFLOAT4(x, y, z, w) {};
    Vector4(const Vector3& v) { x = v.x; y = v.y; z = v.z; w = 0.0f; }
    Vector4(const Vector4& v) { x = v.x; y = v.y; z = v.z; w = v.w; }
    Vector4(const DirectX::XMFLOAT4& v) { x = v.x; y = v.y; z = v.z; w = v.w; }
    ~Vector4() {};

    Vector4 operator*(const Vector4&) const;

    const Vector3 Vec3()const { return Vector3(x, y, z); }
};