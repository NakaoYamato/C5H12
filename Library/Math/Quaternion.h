#pragma once

#include <DirectXMath.h>
//**************************************************************
// クォータニオン の便利化
//**************************************************************
//------< プロトタイプ宣言 >-------------------------
class Quaternion;

//--------------------------------------------------
// 関数
//--------------------------------------------------
// 軸を指定して指定量回転
Quaternion QuaternionRotationAxis(const DirectX::XMFLOAT3& axis, float radian);
Quaternion QuaternionRotationAxisDegree(const DirectX::XMFLOAT3& axis, float degree);
Quaternion QuaternionRotationAxis(const DirectX::XMVECTOR& axis, float radian);
Quaternion QuaternionRotationAxisDegree(const DirectX::XMVECTOR& axis, float degree);
//--------------------------------------------------
// クォータニオンの掛け算
Quaternion QuaternionMultiply(const Quaternion& src, const Quaternion& dst);
//--------------------------------------------------
// 指定方向に向くクォータニオン(改装中)
// position	: ワールド座標
// front	: 「正規化された」前方向
// target	: 向く方向
Quaternion QuaternionLookAt(const DirectX::XMFLOAT3& position,
	const DirectX::XMFLOAT3& front,
	const DirectX::XMFLOAT3& target);
//--------------------------------------------------
// 指定方向に向くクォータニオン
// q	: 回転させる対象
// position	: ワールド座標
// front	: 前方向
// target	: 向く方向
Quaternion QuaternionLookAt(const Quaternion& q,
	const DirectX::XMFLOAT3& position,
	const DirectX::XMFLOAT3& front,
	const DirectX::XMFLOAT3& target);
//--------------------------------------------------
// クォータニオンからオイラー角に変換
DirectX::XMFLOAT3 QuaternionToRollPitchYaw(const Quaternion& q);
DirectX::XMFLOAT3 QuaternionToRollPitchYaw(const DirectX::XMVECTOR& q);
//--------------------------------------------------
// オイラー角からクォータニオンに変換
Quaternion QuaternionFromRollPitchYaw(const DirectX::XMFLOAT3& angle);
//--------------------------------------------------
// クォータニオンの保管
Quaternion QuaternionSlerp(const Quaternion& src, const Quaternion& dst, float t);

// クォータニオン
class Quaternion : public DirectX::XMFLOAT4
{
public:
	// 各軸のベクトル
	static constexpr DirectX::XMFLOAT3 AxisX = { 1.0f,0.0f,0.0f };
	static constexpr DirectX::XMFLOAT3 AxisY = { 0.0f,1.0f,0.0f };
	static constexpr DirectX::XMFLOAT3 AxisZ = { 0.0f,0.0f,1.0f };

public:
	Quaternion() : DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) {}
	// オイラー角からクォータニオンを生成
	Quaternion(float roll, float pitch, float yaw);
	// 回転行列からクォータニオンを生成
	Quaternion(const DirectX::XMFLOAT4X4& r);
	Quaternion(const DirectX::XMMATRIX& r);

	Quaternion(const DirectX::XMFLOAT4& v) { x = v.x; y = v.y; z = v.z; w = v.w; }
	~Quaternion() {}

	// 回転行列に変換
	DirectX::XMFLOAT4X4 ToMatrix()const;
};