#pragma once

#include <DirectXMath.h>

// クォータニオン
class Quaternion : public DirectX::XMFLOAT4
{
public:
	Quaternion() : DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) {}
	// オイラー角からクォータニオンを生成
	Quaternion(float roll, float pitch, float yaw);
	// 回転行列からクォータニオンを生成
	Quaternion(const DirectX::XMFLOAT4X4& r);
	Quaternion(const DirectX::XMMATRIX& r);

	Quaternion(const DirectX::XMFLOAT4& v) { x = v.x; y = v.y; z = v.z; w = v.w; }
	~Quaternion() {}

#pragma region 静的メンバ変数
	static constexpr DirectX::XMFLOAT4 Identity = { 0.0f,0.0f,0.0f,1.0f };
	static constexpr DirectX::XMFLOAT3 AxisX = { 1.0f,0.0f,0.0f };
	static constexpr DirectX::XMFLOAT3 AxisY = { 0.0f,1.0f,0.0f };
	static constexpr DirectX::XMFLOAT3 AxisZ = { 0.0f,0.0f,1.0f };
#pragma endregion

#pragma region 静的メンバ関数
	// 軸を指定して指定量回転
	static Quaternion RotationAxis(const DirectX::XMFLOAT3& axis, float radian);
	// 軸を指定して指定量回転
	static Quaternion RotationAxis(const DirectX::XMVECTOR& axis, float radian);
	// 軸を指定して指定量回転
	static Quaternion RotationAxisDegree(const DirectX::XMFLOAT3& axis, float radian);
	// 軸を指定して指定量回転
	static Quaternion RotationAxisDegree(const DirectX::XMVECTOR& axis, float radian);
	// クォータニオンの掛け算
	static Quaternion Multiply(const Quaternion& src, const Quaternion& dst);
	/// <summary>
	/// 指定方向に向くクォータニオン
	/// </summary>
	/// <param name="q">回転させる対象</param>
	/// <param name="position">ワールド座標</param>
	/// <param name="front">前方向</param>
	/// <param name="target">向く方向</param>
	/// <returns></returns>
	static Quaternion LookAt(const Quaternion& q,
		const DirectX::XMFLOAT3& position,
		const DirectX::XMFLOAT3& front,
		const DirectX::XMFLOAT3& target);
	// 指定方向を向く
	static Quaternion LookAt(const DirectX::XMFLOAT3& position,
		const DirectX::XMFLOAT3& front,
		const DirectX::XMFLOAT3& target);
	// クォータニオンからオイラー角に変換
	static DirectX::XMFLOAT3 ToRollPitchYaw(const Quaternion& q);
	// クォータニオンからオイラー角に変換
	static DirectX::XMFLOAT3 ToRollPitchYaw(const DirectX::XMVECTOR& q);
	// オイラー角からクォータニオンに変換
	static Quaternion FromRollPitchYaw(const DirectX::XMFLOAT3& angle);
	// クォータニオンの保管
	static Quaternion Slerp(const Quaternion& src, const Quaternion& dst, float t);
#pragma endregion


#pragma region メンバ関数
	// 回転行列に変換
	DirectX::XMFLOAT4X4 ToMatrix()const;
    // オイラー角に変換
	DirectX::XMFLOAT3 ToRollPitchYaw() const { return ToRollPitchYaw(*this); }

    // クォータニオンの掛け算
	Quaternion operator*(const Quaternion& q) const;
    // クォータニオンの掛け算
	Quaternion Multiply(const Quaternion& q) const;
	// 補完処理
    Quaternion Slerp(const Quaternion& q, float t) const
    {
        return Slerp(*this, q, t);
    }
#pragma endregion


};