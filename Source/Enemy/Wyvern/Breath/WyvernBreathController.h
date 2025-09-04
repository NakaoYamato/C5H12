#pragma once

#include "../../Library/Component/Component.h"
#include "../../Library/Component/Collider/CapsuleCollider.h"
#include "../../Library/Component/ParticleController.h"

class WyvernBreathController : public Component
{
public:
	WyvernBreathController() {}
	~WyvernBreathController() override {}
	// 名前取得
	const char* GetName() const override { return "WyvernBreathController"; }
	// 開始処理
	void Start() override;
	// 削除処理
	void OnDelete() override;
	// 更新処理
	void Update(float elapsedTime) override;
	// GUI描画
	void DrawGui() override;
	// 接触時処理
	void OnContactEnter(CollisionData& collisionData) override;

	// ブレスを撃ったアクターを設定
	void SetBreathActor(std::shared_ptr<Actor> actor) { _breathActor = actor; }
private:
	// ブレスを撃ったアクター
	std::weak_ptr<Actor> _breathActor;
	// パーティクルコントローラー
	std::weak_ptr<ParticleController> _particleController;
	// カプセルコライダー群
	std::vector<std::weak_ptr<CapsuleCollider>> _capsuleColliders;

	Vector3 _collisionOffset = Vector3(0.0f, -1.2f, 1.2f);

	// ブレスの速度
	float _breathSpeed = 15.0f;
	// ブレスの半径
	float _breathRadius = 0.7f;
	// ブレスの距離
	float _breathRange = 0.0f;
	// ブレスの最大距離
	float _breathRangeMax = 13.0f;
	// ブレスのブレ
	float _breathWaver = 0.1f;
	// 攻撃力
	float _ATK = 2.0f;
};