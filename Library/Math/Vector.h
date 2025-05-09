#pragma once

#include "Easing.h"

#include <DirectXMath.h>

#pragma region プロトタイプ宣言
class Vector2;
class Vector3;
class Vector4;
#pragma endregion

#pragma region 定義
#define _VECTOR2_ZERO   Vector2(0.0f, 0.0f)
#define _VECTOR2_ONE    Vector2(1.0f, 1.0f)

#define _VECTOR3_ZERO   Vector3(0.0f, 0.0f, 0.0f)
#define _VECTOR3_ONE    Vector3(1.0f, 1.0f, 1.0f)
#define _VECTOR3_RIGHT  Vector3(1.0f, 0.0f, 0.0f)
#define _VECTOR3_UP     Vector3(0.0f, 1.0f, 0.0f)
#define _VECTOR3_FRONT  Vector3(0.0f, 0.0f, 1.0f)

#define _VECTOR4_RIGHT  Vector4(1.0f, 0.0f, 0.0f, 0.0f)
#define _VECTOR4_UP     Vector4(0.0f, 1.0f, 0.0f, 0.0f)
#define _VECTOR4_FRONT  Vector4(0.0f, 0.0f, 1.0f, 0.0f)
#define _VECTOR4_ZERO   Vector4(0.0f, 0.0f, 0.0f, 0.0f)
#define _VECTOR4_BLACK  Vector4(0.0f, 0.0f, 0.0f, 0.0f)
#define _VECTOR4_WHITE  Vector4(1.0f, 1.0f, 1.0f, 1.0f)
#define _VECTOR4_GRAY   Vector4(0.2f, 0.2f, 0.2f, 1.0f)
#define _VECTOR4_RED    Vector4(1.0f, 0.0f, 0.0f, 1.0f)
#define _VECTOR4_GREEN  Vector4(0.0f, 1.0f, 0.0f, 1.0f)
#define _VECTOR4_BLUE   Vector4(0.0f, 0.0f, 1.0f, 1.0f)
#define _VECTOR4_YELLOW Vector4(1.0f, 1.0f, 0.0f, 1.0f)
#pragma endregion

#pragma region 関数
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
#pragma endregion

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

#pragma region 関数
    //  VECTOR2の長さの二乗を取得
	float Length() const { return Vec2Length(*this); }
    //  VECTOR2の長さを取得
	float LengthSq() const { return Vec2LengthSq(*this); }
    //  VECTOR2対VECTOR2の内積
	float Dot(const Vector2& v) const { return Vec2Dot(*this, v); }
    //  VECTOR2対VECTOR2の外積
    Vector2 Cross(const Vector2& rhs) const { return Vec2Cross(*this, rhs); }
    //  VECTOR2を単位化
	Vector2 Normalize() const { return Vec2Normalize(*this); }
    //  thisとdstで保管処理
	Vector2 Lerp(const Vector2& dst, float t, float (*Easing)(float) = nullptr) const
	{
		return Vec2Lerp(*this, dst, t, Easing);
	}
#pragma endregion
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
#pragma region 関数
    //  VECTOR3の長さの二乗を取得
	float Length() const { return Vec3Length(*this); }
    //  VECTOR3の長さを取得
	float LengthSq() const { return Vec3LengthSq(*this); }
    //  VECTOR3対VECTOR3の内積
	float Dot(const Vector3& v) const { return Vec3Dot(*this, v); }
    //  VECTOR3対VECTOR3の外積
	Vector3 Cross(const Vector3& rhs) const { return Vec3Cross(*this, rhs); }
    //  VECTOR3を単位化
	Vector3 Normalize() const { return Vec3Normalize(*this); }
    //  thisとdstで保管処理
	Vector3 Lerp(const Vector3& dst, float t, float (*Easing)(float) = nullptr) const
	{
		return Vec3Lerp(*this, dst, t, Easing);
	}
    //  VECTOR3を度数法に変換
	Vector3 ConvertToDegrees() const { return Vec3ConvertToDegrees(*this); }
    //  VECTOR3を弧度法に変換
	Vector3 ConvertToRadians() const { return Vec3ConvertToRadians(*this); }
    //  回転行列からオイラー角取得(回転順ZXY XMMatrixRotationRollPitchYawはZXYの順番で回転している)
	Vector3 FromRotationMatrix(const DirectX::XMFLOAT4X4& r) const
	{
		return Vec3FromRotationMatrix(r);
	}
    //  回転行列からオイラー角取得(回転順ZXY XMMatrixRotationRollPitchYawはZXYの順番で回転している)
	Vector3 FromRotationMatrix(const DirectX::XMMATRIX& r) const
	{
		return Vec3FromRotationMatrix(r);
	}
    // 行列との掛け算
	Vector3 TransformCoord(const DirectX::XMFLOAT4X4& m) const
	{
		return Vec3TransformCoord(*this, m);
	}
    // 行列との掛け算
	Vector3 TransformCoord(const DirectX::XMMATRIX& m) const
	{
		return Vec3TransformCoord(*this, m);
	}
    // 行列との掛け算
	Vector3 TransformNormal(const DirectX::XMFLOAT4X4& m) const
	{
		return Vec3TransformNormal(*this, m);
	}
    // 行列との掛け算
	Vector3 TransformNormal(const DirectX::XMMATRIX& m) const
	{
		return Vec3TransformNormal(*this, m);
	}
    // 各軸の角度取得
    /// <param name="normalVec">正規化されたベクトル</param>
    /// <returns>ラジアン</returns>
	Vector3 CalcAngle() const
	{
		return Vec3CalcAngle(*this);
	}
#pragma endregion
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

    Vector4 operator*(float) const;
    Vector4 operator*(const Vector4&) const;

    const Vector3 Vec3()const { return Vector3(x, y, z); }
};