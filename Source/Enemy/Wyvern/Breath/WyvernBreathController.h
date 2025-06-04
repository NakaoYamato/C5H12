#pragma once

#include "../../Library/Component/Component.h"
#include "../../Library/Component/Collider/CapsuleCollider.h"

class WyvernBreathController : public Component
{
public:
	WyvernBreathController(std::shared_ptr<CapsuleCollider> capsuleCollider) :
		_capsuleCollider(capsuleCollider){}
	~WyvernBreathController() override {}
	// 名前取得
	const char* GetName() const override { return "WyvernBreathController"; }
	// 更新処理
	void Update(float elapsedTime) override;
	// GUI描画
	void DrawGui() override;
	// 接触時処理
	void OnContactEnter(CollisionData& collisionData) override;

	// ブレスを撃ったアクターを設定
	void SetBreathActor(std::shared_ptr<Actor> actor) { _breathActor = actor; }
	// ブレスの方向を設定
	void SetBreathDirection(const Vector3& direction) { _breathDirection = direction; }
private:
	// ブレスを撃ったアクター
	std::weak_ptr<Actor> _breathActor;
	// カプセルコライダー
	std::weak_ptr<CapsuleCollider> _capsuleCollider;
	// ブレスの方向
	Vector3 _breathDirection = Vector3::Front;

	// ブレスの速度
	float _breathSpeed = 100.0f;
	// ブレスの距離
	float _breathRange = 0.0f;
	// ブレスの最大距離
	float _breathRangeMax = 10.0f;
	// 攻撃力
	float _ATK = 2.0f;

	// パーティクル生成数
	int _particleCount = 10;
	// パーティクルの生存時間
	float _particleLifeTime = 2.0f;
	// パーティクルの速度
	float _particleSpeed = 30.0f;
	// パーティクルの分散率
	Vector3 _particleSpread = Vector3(0.5f, 0.2f, 0.5f);
	// パーティクルのスケール
	float _particleScale = 1.0f;
};