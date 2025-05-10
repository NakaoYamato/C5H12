#pragma once

#include "Easing.h"

#include <DirectXMath.h>

#pragma region プロトタイプ宣言
class Vector2;
class Vector3;
class Vector4;
#pragma endregion

// Vector2
class Vector2 : public DirectX::XMFLOAT2
{
public:
    Vector2() : DirectX::XMFLOAT2(0, 0) {};
    Vector2(float x, float y) : DirectX::XMFLOAT2(x, y) {};
    Vector2(int x, int y) : DirectX::XMFLOAT2(static_cast<float>(x), static_cast<float>(y)) {};
    Vector2(const Vector2& v) { x = v.x; y = v.y; }
    Vector2(const DirectX::XMFLOAT2& v) { x = v.x; y = v.y; }
    ~Vector2() {};

#pragma region 演算子オーバーロード
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
#pragma endregion

#pragma region 静的メンバ変数
    static constexpr DirectX::XMFLOAT2 Zero = { 0.0f,0.0f };
    static constexpr DirectX::XMFLOAT2 One = { 1.0f,1.0f };
    static constexpr DirectX::XMFLOAT2 Max = { FLT_MAX,FLT_MAX };
    static constexpr DirectX::XMFLOAT2 XOneYZero = { 1.0f,0.0f };
    static constexpr DirectX::XMFLOAT2 XZeroYOne = { 0.0f,1.0f };
#pragma endregion

#pragma region 静的メンバ関数
    //  VECTOR2の長さの二乗を取得
    static float   LengthSq(const Vector2&);
    //  VECTOR2の長さを取得
    static float   Length(const Vector2&);
    //  VECTOR2を単位化
    static Vector2 Normalize(const Vector2&);
    //  VECTOR2対VECTOR2の内積
    static float Dot(const Vector2& src, const Vector2& dst);
    //  VECTOR2対VECTOR2の外積
    static Vector2 Cross(const Vector2& src, const Vector2& dst);
    //  srcとdstで保管処理
    //  t   : 経過時間(0.0f ~ 1.0f)
    static Vector2 Lerp(const Vector2& src, const Vector2& dst, float t, float (*Easing)(float) = nullptr);
#pragma endregion

#pragma region 関数
    //  VECTOR2の長さの二乗を取得
	float Length() const { return Length(*this); }
    //  VECTOR2の長さを取得
	float LengthSq() const { return LengthSq(*this); }
    //  VECTOR2対VECTOR2の内積
	float Dot(const Vector2& v) const { return Dot(*this, v); }
    //  VECTOR2対VECTOR2の外積
    Vector2 Cross(const Vector2& rhs) const { return Cross(*this, rhs); }
    //  VECTOR2を単位化
	Vector2 Normalize() const { return Normalize(*this); }
    //  thisとdstで保管処理
	Vector2 Lerp(const Vector2& dst, float t, float (*Easing)(float) = nullptr) const
	{
		return Lerp(*this, dst, t, Easing);
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

#pragma region 演算子オーバーロード
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
#pragma endregion

#pragma region 静的メンバ変数
    static constexpr DirectX::XMFLOAT3 Zero     = { 0.0f,0.0f,0.0f };
    static constexpr DirectX::XMFLOAT3 One      = { 1.0f,1.0f,1.0f };
    static constexpr DirectX::XMFLOAT3 Max      = { FLT_MAX,FLT_MAX,FLT_MAX };
    static constexpr DirectX::XMFLOAT3 Right    = { 1.0f,0.0f,0.0f };
    static constexpr DirectX::XMFLOAT3 Up       = { 0.0f,1.0f,0.0f };
    static constexpr DirectX::XMFLOAT3 Front    = { 0.0f,0.0f,1.0f };
#pragma endregion

#pragma region 静的メンバ関数
    //  VECTOR3の長さの二乗を取得
    static float   LengthSq(const Vector3&);
    //  VECTOR3の長さを取得
    static float   Length(const Vector3&);
    //  VECTOR3を単位化
    static Vector3 Normalize(const Vector3&);
    //  VECTOR3対VECTOR3の内積
    static float Dot(const Vector3& src, const Vector3& dst);
    //  VECTOR3対VECTOR3の外積
    static Vector3 Cross(const Vector3& src, const Vector3& dst);
    //  srcとdstで保管処理
    //  t   : 経過時間(0.0f ~ 1.0f)
    static Vector3 Lerp(const Vector3& src, const Vector3& dst, float t, float (*Easing)(float) = nullptr);
    //  VECTOR3を度数法に変換
    static Vector3 ConvertToDegrees(const Vector3& v);
    //  VECTOR3を弧度法に変換
    static Vector3 ConvertToRadians(const Vector3& v);
    //  回転行列からオイラー角取得(回転順ZXY XMMatrixRotationRollPitchYawはZXYの順番で回転している)
    static Vector3 FromRotationMatrix(const DirectX::XMFLOAT4X4& r);
    //  回転行列からオイラー角取得(回転順ZXY XMMatrixRotationRollPitchYawはZXYの順番で回転している)
    static Vector3 FromRotationMatrix(const DirectX::XMMATRIX& R);
    // 行列との掛け算
    static Vector3 TransformCoord(const Vector3& src, const DirectX::XMFLOAT4X4& m);
    // 行列との掛け算
    static Vector3 TransformCoord(const Vector3& src, const DirectX::XMMATRIX& M);
    // 行列との掛け算
    static Vector3 TransformNormal(const Vector3& src, const DirectX::XMFLOAT4X4& m);
    // 行列との掛け算
    static Vector3 TransformNormal(const Vector3& src, const DirectX::XMMATRIX& M);
    // 各軸の角度取得
    /// <param name="normalVec">正規化されたベクトル</param>
    /// <returns>ラジアン</returns>
    static Vector3 CalcAngle(const Vector3& normalVec);
#pragma endregion

#pragma region 関数
    //  VECTOR3の長さの二乗を取得
	float Length() const { return Vector3::Length(*this); }
    //  VECTOR3の長さを取得
	float LengthSq() const { return Vector3::LengthSq(*this); }
    //  VECTOR3対VECTOR3の内積
	float Dot(const Vector3& v) const { return Vector3::Dot(*this, v); }
    //  VECTOR3対VECTOR3の外積
	Vector3 Cross(const Vector3& rhs) const { return Vector3::Cross(*this, rhs); }
    //  VECTOR3を単位化
	Vector3 Normalize() const { return Vector3::Normalize(*this); }
    //  thisとdstで保管処理
	Vector3 Lerp(const Vector3& dst, float t, float (*Easing)(float) = nullptr) const
	{
		return Vector3::Lerp(*this, dst, t, Easing);
	}
    //  VECTOR3を度数法に変換
	Vector3 ConvertToDegrees() const { return Vector3::ConvertToDegrees(*this); }
    //  VECTOR3を弧度法に変換
	Vector3 ConvertToRadians() const { return Vector3::ConvertToRadians(*this); }
    // 行列との掛け算
	Vector3 TransformCoord(const DirectX::XMFLOAT4X4& m) const
	{
		return Vector3::TransformCoord(*this, m);
	}
    // 行列との掛け算
	Vector3 TransformCoord(const DirectX::XMMATRIX& m) const
	{
		return Vector3::TransformCoord(*this, m);
	}
    // 行列との掛け算
	Vector3 TransformNormal(const DirectX::XMFLOAT4X4& m) const
	{
		return Vector3::TransformNormal(*this, m);
	}
    // 行列との掛け算
	Vector3 TransformNormal(const DirectX::XMMATRIX& m) const
	{
		return Vector3::TransformNormal(*this, m);
	}
    // 各軸の角度取得
    /// <param name="normalVec">正規化されたベクトル</param>
    /// <returns>ラジアン</returns>
	Vector3 CalcAngle() const
	{
		return Vector3::CalcAngle(*this);
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

#pragma region 静的メンバ変数
    static constexpr DirectX::XMFLOAT4 Right    = { 1.0f, 0.0f, 0.0f, 0.0f };
    static constexpr DirectX::XMFLOAT4 Up       = { 0.0f, 1.0f, 0.0f, 0.0f };
    static constexpr DirectX::XMFLOAT4 Front    = { 0.0f, 0.0f, 1.0f, 0.0f };
    static constexpr DirectX::XMFLOAT4 Zero     = { 0.0f, 0.0f, 0.0f, 0.0f };
    static constexpr DirectX::XMFLOAT4 Black    = { 0.0f, 0.0f, 0.0f, 0.0f };
    static constexpr DirectX::XMFLOAT4 White    = { 1.0f, 1.0f, 1.0f, 1.0f };
    static constexpr DirectX::XMFLOAT4 Gray     = { 0.2f, 0.2f, 0.2f, 1.0f };
    static constexpr DirectX::XMFLOAT4 Red      = { 1.0f, 0.0f, 0.0f, 1.0f };
    static constexpr DirectX::XMFLOAT4 Green    = { 0.0f, 1.0f, 0.0f, 1.0f };
    static constexpr DirectX::XMFLOAT4 Blue     = { 0.0f, 0.0f, 1.0f, 1.0f };
    static constexpr DirectX::XMFLOAT4 Yellow   = { 1.0f, 1.0f, 0.0f, 1.0f };
#pragma endregion

#pragma region 演算子オーバーロード
    Vector4 operator*(float) const;
    Vector4 operator*(const Vector4&) const;
#pragma endregion

    const Vector3 Vec3()const { return Vector3(x, y, z); }
};