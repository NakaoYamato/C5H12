#pragma once

#include "../../Library/Component/Component.h"
#include "../../Library/Component/Collider/SphereCollider.h"
#include "../../Library/Component/EffectController.h"

class WyvernBallController : public Component
{
public:
	WyvernBallController() {}
	~WyvernBallController() override {}
	// 名前取得
	const char* GetName() const override { return "WyvernBallController"; }
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
	// 火球を撃ったアクターを設定
	void SetBallActor(std::shared_ptr<Actor> actor) { _ballActor = actor; }

private:
	// 火球を撃ったアクター
	std::weak_ptr<Actor> _ballActor;
	// エフェクトコントローラー
	std::weak_ptr<EffectController> _effectController;
	// 球コライダー
	std::weak_ptr<SphereCollider> _ballCollider;
	Vector3 _collisionOffset = Vector3(0.0f, -1.2f, 1.2f);
	// 火球の速度
	float _ballSpeed = 12.0f;
	// 火球の進む時間
	float _ballTime = 1.0f;
	// 火球の生存時間
	float _lifeTimer = 1.0f;
	// 爆発フラグ
	bool _isExploded = false;
	// 攻撃力
	float _ATK = 20.0f;
};