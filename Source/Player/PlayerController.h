#pragma once

#include "../../Library/Component/Component.h"
#include "../../Library/Component/CharactorController.h"
#include "../../Library/Component/Animator.h"
#include "StateMachine/PlayerStateMachine.h"
#include "../../Source/Interface/Common/IDamagable.h"
#include "../../Library/Component/Effekseer/EffekseerEffectController.h"

#include <PlayerDefine.h>

class PlayerController : public Component, public IDamagable
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
	// GUI描画
	void DrawGui() override;
	// 接触時処理
	void OnContactEnter(CollisionData& collisionData) override;

	// ダメージを受けた時の処理
	void OnDamage(float damage, Vector3 hitPosition) override;

#pragma region アクセサ
	// ステートマシン取得
    PlayerStateMachine* GetPlayerStateMachine() { return _stateMachine.get(); }
	// 攻撃力のセット
	void SetATK(float atk) 
	{ 
		_ATK = atk; 
	}
#pragma endregion

private:
#pragma region ステート制御
	// ステートマシン
	std::unique_ptr<PlayerStateMachine> _stateMachine;
	float _ATK = 1.0f;
#pragma endregion

	std::weak_ptr<CharactorController> _charactorController;
	std::weak_ptr<EffekseerEffectController> _hitEffectController;
	std::weak_ptr<Animator> _animator;
};