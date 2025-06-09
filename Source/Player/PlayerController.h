#pragma once

#include "../../Library/Component/Component.h"
#include "../../Library/Component/CharactorController.h"
#include "../../Library/Component/Animator.h"
#include "StateMachine/PlayerStateMachine.h"
#include "../../Source/Common/Damageable.h"
#include "../../Library/Component/Effekseer/EffekseerEffectController.h"

#include <PlayerDefine.h>

class PlayerController : public Component
{
public:
	PlayerController() {}
	~PlayerController() override {}

	// 名前取得
	const char* GetName() const override { return "PlayerController"; }

	// 開始処理
	void Start() override;
	// 更新処理
	void Update(float elapsedTime) override;
	// 3D描画後の描画処理
	void DelayedRender(const RenderContext& rc) override;
	// GUI描画
	void DrawGui() override;
	// 接触時処理
	void OnContactEnter(CollisionData& collisionData) override;

#pragma region アクセサ
	// ステートマシン取得
    PlayerStateMachine* GetPlayerStateMachine() { return _stateMachine.get(); }
	// キャラクターコントローラー取得
	std::shared_ptr<CharactorController> GetCharactorController()
	{
		return _charactorController.lock();
	}
	std::shared_ptr<Damageable> GetDamageable()
	{
		return _damageable.lock();
	}
	// 攻撃力のセット
	void SetATK(float atk) { _ATK = atk; }
	// 基本攻撃力のセット
	void SetBaseATK(float baseATK) { _BaseATK = baseATK; }
	// ダメージ倍率のセット
	void SetATKFactor(float atkFactor) { _ATKFactor = atkFactor; }
#pragma endregion

private:
#pragma region ステート制御
	// ステートマシン
	std::unique_ptr<PlayerStateMachine> _stateMachine;
#pragma endregion

	std::weak_ptr<CharactorController> _charactorController;
	std::weak_ptr<Animator> _animator;
	std::weak_ptr<EffekseerEffectController> _hitEffectController;
	std::weak_ptr<Damageable> _damageable;

#pragma region 攻撃関係
	// 最終攻撃力
	float _ATK = 1.0f;
	// 基本攻撃力
	float _BaseATK = 1.0f;
	// ダメージ倍率
	float _ATKFactor = 1.0f;
#pragma endregion
};