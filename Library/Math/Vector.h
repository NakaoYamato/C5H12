#pragma once

#include "Easing.h"
#include "MathF.h"

#include <DirectXMath.h>

#pragma region プロトタイプ宣言
class Vector2;
class Vector3;
class Vector4;
#pragma endregion

// 角度の正規化（-180～180度／-π～πラジアン）をする
static float NormalizeAngle(float radian)
{
    //角度の正規化（-180～180度／-π～πラジアン）をする
    return radian > DirectX::XM_PI ? NormalizeAngle(radian - DirectX::XM_2PI) :
        radian < -DirectX::XM_PI ? NormalizeAngle(radian + DirectX::XM_2PI) :
        radian;
}

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
    //  Vector2の長さの二乗を取得
    static float   LengthSq(const Vector2&);
    //  Vector2の長さを取得
    static float   Length(const Vector2&);
    //  Vector2を単位化
    static Vector2 Normalize(const Vector2&);
    //  Vector2対Vector2の内積
    static float Dot(const Vector2& src, const Vector2& dst);
    //  Vector2対Vector2の外積
    static Vector2 Cross(const Vector2& src, const Vector2& dst);
    //  srcとdstで保管処理
    //  t   : 経過時間(0.0f ~ 1.0f)
    static Vector2 Lerp(const Vector2& src, const Vector2& dst, float t, float (*Easing)(float) = nullptr);
#pragma endregion

#pragma region 関数
    //  Vector2の長さの二乗を取得
	float Length() const { return Length(*this); }
    //  Vector2の長さを取得
	float LengthSq() const { return LengthSq(*this); }
    //  Vector2対Vector2の内積
	float Dot(const Vector2& v) const { return Dot(*this, v); }
    //  Vector2対Vector2の外積
    Vector2 Cross(const Vector2& rhs) const { return Cross(*this, rhs); }
    //  Vector2を単位化
	Vector2 Normalize() const { return Normalize(*this); }
    //  thisとdstで保管処理
	Vector2 Lerp(const Vector2& dst, float t, float (*Easing)(float) = nullptr) const
	{
		return Lerp(*this, dst, t, Easing);
	}
#pragma endregion
};
//--------------------------------------------------
// Vector3
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
    static constexpr DirectX::XMFLOAT3 Left     = { -1.0f,0.0f,0.0f };
    static constexpr DirectX::XMFLOAT3 Down     = { 0.0f,-1.0f,0.0f };
    static constexpr DirectX::XMFLOAT3 Back     = { 0.0f,0.0f,-1.0f };
#pragma endregion

#pragma region 静的メンバ関数
    //  Vector3の長さの二乗を取得
    static float   LengthSq(const Vector3&);
    //  Vector3の長さを取得
    static float   Length(const Vector3&);
    //  Vector3を単位化
    static Vector3 Normalize(const Vector3&);
    //  Vector3対Vector3の内積
    static float Dot(const Vector3& src, const Vector3& dst);
    //  Vector3対Vector3の外積
    static Vector3 Cross(const Vector3& src, const Vector3& dst);
    // Vector3対Vector3の各項目掛け算
	static Vector3 Multiply(const Vector3& src, const Vector3& dst);
    //  srcとdstで保管処理
    //  t   : 経過時間(0.0f ~ 1.0f)
    static Vector3 Lerp(const Vector3& src, const Vector3& dst, float t, float (*Easing)(float) = nullptr);
    //  Vector3をオイラー角で単位化
	static Vector3 NormalizeEuler(const Vector3& v);
    //  Vector3を度数法に変換
    static Vector3 ToDegrees(const Vector3& v);
    //  Vector3を弧度法に変換
    static Vector3 ToRadians(const Vector3& v);
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
    // 乱数取得
    static Vector3 Random(const Vector3& min, const Vector3& max);
	// 0~1の範囲でランダムな値を取得
    static Vector3 RandomBias();
	// -1~1の範囲でランダムな値を取得
    static Vector3 RandomNormal();
	// Vector3の値をminとmaxで制限
    static Vector3 Clamp(const Vector3& v, const Vector3& min, const Vector3& max);
	// Vector3の値を中心と半径で制限
    static Vector3 ClampSphere(const Vector3& v, const Vector3& center, float radius);
#pragma endregion

#pragma region 関数
    //  Vector3の長さの二乗を取得
	float Length() const { return Vector3::Length(*this); }
    //  Vector3の長さを取得
	float LengthSq() const { return Vector3::LengthSq(*this); }
    //  Vector3対Vector3の内積
	float Dot(const Vector3& v) const { return Vector3::Dot(*this, v); }
    //  Vector3対Vector3の外積
	Vector3 Cross(const Vector3& rhs) const { return Vector3::Cross(*this, rhs); }
    //  Vector3を単位化
	Vector3 Normalize() const { return Vector3::Normalize(*this); }
    //  thisとdstで保管処理
	Vector3 Lerp(const Vector3& dst, float t, float (*Easing)(float) = nullptr) const
	{
		return Vector3::Lerp(*this, dst, t, Easing);
	}
    //  Vector3をオイラー角で単位化
	Vector3 NormalizeEuler() const { return Vector3::NormalizeEuler(*this); }
    //  Vector3を度数法に変換
	Vector3 ToDegrees() const { return Vector3::ToDegrees(*this); }
    //  Vector3を弧度法に変換
	Vector3 ToRadians() const { return Vector3::ToRadians(*this); }
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
    // Vector3の値をminとmaxで制限
    Vector3 Clamp(const Vector3& min, const Vector3& max) const
    {
		return Vector3::Clamp(*this, min, max);
    }
    // Vector3の値を中心と半径で制限
    Vector3 ClampSphere(const Vector3& center, float radius) const
    {
		return Vector3::ClampSphere(*this, center, radius);
    }
#pragma endregion
};

//--------------------------------------------------
// Vector4
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
    static constexpr DirectX::XMFLOAT4 One      = { 1.0f, 1.0f, 1.0f, 1.0f };
    static constexpr DirectX::XMFLOAT4 Black    = { 0.0f, 0.0f, 0.0f, 0.0f };
    static constexpr DirectX::XMFLOAT4 White    = { 1.0f, 1.0f, 1.0f, 1.0f };
    static constexpr DirectX::XMFLOAT4 Gray     = { 0.2f, 0.2f, 0.2f, 1.0f };
    static constexpr DirectX::XMFLOAT4 Red      = { 1.0f, 0.0f, 0.0f, 1.0f };
    static constexpr DirectX::XMFLOAT4 Green    = { 0.0f, 1.0f, 0.0f, 1.0f };
    static constexpr DirectX::XMFLOAT4 Blue     = { 0.0f, 0.0f, 1.0f, 1.0f };
    static constexpr DirectX::XMFLOAT4 Yellow   = { 1.0f, 1.0f, 0.0f, 1.0f };
#pragma endregion

#pragma region 静的メンバ関数
    // Vector4対Vector4の各項目掛け算
    static Vector4 Multiply(const Vector4& src, const Vector4& dst);
    // 乱数取得
    static Vector4 Random(const Vector4& min, const Vector4& max);
    // 0~1の範囲でランダムな値を取得
    static Vector4 RandomBias();
    // -1~1の範囲でランダムな値を取得
    static Vector4 RandomNormal();
#pragma endregion

#pragma region 演算子オーバーロード
    Vector4 operator+(const Vector4&) const;
    Vector4 operator*(float) const;
    Vector4 operator*(const Vector4&) const;
#pragma endregion

    const Vector3 Vec3()const { return Vector3(x, y, z); }
};