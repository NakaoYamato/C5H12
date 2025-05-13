#pragma once

#include "ColliderBase.h"

class CapsuleCollider : public ColliderBase
{
public:
	CapsuleCollider() {}
	~CapsuleCollider() override {}
	// 名前取得
	const char* GetName() const override { return "CapsuleCollider"; }
	// 開始処理
	void Start() override;
	// デバッグ描画処理
	void DebugRender(const RenderContext& rc) override;
	// 削除時処理
	void Deleted() override;
	// GUI描画
	void DrawGui() override;
#pragma region アクセサ
	const Vector3& GetStart() const { return _start; } // 開始座標
	const Vector3& GetEnd() const { return _end; } // 終了座標
	float GetRadius() const { return _radius; }

	void SetStart(const Vector3& start) { _start = start; }
	void SetEnd(const Vector3& end) { _end = end; }
	void SetRadius(float radius) { _radius = radius; }
#pragma endregion
private:
	Vector3 _start = Vector3::Zero; // 開始座標
	Vector3 _end = Vector3::Up; // 終了座標
	float _radius = 1.0f; // 半径
};