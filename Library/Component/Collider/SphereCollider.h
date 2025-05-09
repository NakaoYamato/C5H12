#pragma once

#include "ColliderBase.h"

class SphereCollider : public ColliderBase
{
public:
	SphereCollider() {}
	~SphereCollider() override {}
	// 名前取得
	const char* GetName() const override { return "SphereCollider"; }
	// 開始処理
	void Start() override;
	// 削除時処理
	void OnDestroy() override;
	// GUI描画
	void DrawGui() override;
#pragma region アクセサ
	Vector3 GetPosition() const { return _center; }
	float GetRadius() const { return _radius; }
	void SetPosition(const Vector3& position) { _center = position; }
	void SetRadius(float radius) { _radius = radius; }
#pragma endregion
private:
	Vector3 _center = _VECTOR3_ZERO; // 中心座標
	float	_radius = 1.0f; // 半径
};