#pragma once

#include "../../Library/Component/Component.h"
#include "../../Library/Component/CharactorController.h"
#include "../../Library/Component/Animator.h"
#include "PlayerInput.h"
#include "../../Source/StateMachine/Player/PlayerStateMachine.h"

#include <PlayerDefine.h>

class PlayerController : public Component
{
public:
	PlayerController(bool isUserControlled) : _isUserControlled(isUserControlled) {}
	~PlayerController() override {}

	// 名前取得
	const char* GetName() const override { return "PlayerController"; }

	// 開始処理
	void Start() override;

	// 更新処理
	void Update(float elapsedTime) override;

	// GUI描画
	void DrawGui() override;

#pragma region アクセサ
    // プレイヤーの状態を取得
    PlayerMainStates GetState() const { return _state; }
    // プレイヤーの状態を設定
    void SetState(PlayerMainStates state) { _state = state; }
	std::shared_ptr<PlayerInput> GetPlayerInput() const { return _playerInput.lock(); }

	bool IsMoving() const { return _isMoving; }
	bool IsDush() const { return _isDush; }
	bool IsEvade() const { return _isEvade; }
	bool IsAttack() const { return _isAttack; }
	bool IsGuard() const { return _isGuard; }
	// 受けたダメージを取得
	int GetSustainedDamage() const { return _sustainedDamage; }
	// ノックバックダメージを取得
	int GetKnockbackDamage() const { return _knockbackDamage; }
	bool IsDead() const { return _isDead; }
#pragma endregion

private:
	// ユーザーが操作するプレイヤーか
	const bool _isUserControlled = true;
    // プレイヤーの状態
    PlayerMainStates _state = PlayerMainStates::None;
	// ステートマシン
    std::unique_ptr<PlayerStateMachine> _stateMachine;

	std::weak_ptr<CharactorController> _charactorController;
	std::weak_ptr<PlayerInput> _playerInput;
	std::weak_ptr<Animator> _animator;

	// 移動速度
	float _moveSpeed = 20.0f;
	// 摩擦力
	float _friction = 25.0f;

	bool _isMoving = false;
	bool _isDush = false;
	bool _isEvade = false;
	bool _isAttack = false;
	bool _isGuard = false;
	// 受けたダメージダメージ
	int _sustainedDamage = 0;
	int _knockbackDamage = 5;
	bool _isDead = false;
};