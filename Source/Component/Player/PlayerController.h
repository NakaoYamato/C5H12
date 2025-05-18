#pragma once

#include "../../Library/Component/Component.h"
#include "../../Library/Component/CharactorController.h"
#include "../../Library/Component/Animator.h"
#include "../../Source/StateMachine/Player/PlayerStateMachine.h"

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

	// GUI描画
	void DrawGui() override;

    /// <summary>
    /// 移動方向に向く
    /// </summary>
    /// <param name="rotationSpeed">回転速度</param>
    /// <param name="elapsedTime">経過時間</param>
    void RotationMovement(float elapsedTime, float rotationSpeed = 1.0f)
    {
        auto charactorController = _charactorController.lock();
        if (charactorController == nullptr)
            return;
		charactorController->UpdateRotation(elapsedTime,
			_movement * rotationSpeed);
    }

#pragma region アクセサ
	// ステートマシン取得
    PlayerStateMachine* GetPlayerStateMachine() { return _stateMachine.get(); }

    const Vector2& GetMovement() const { return _movement; }
	bool IsMoving() const { return _isMoving; }
	bool IsDash() const { return _isDash; }
	bool IsEvade() const { return _isEvade; }
	bool IsAttack() const { return _isAttack; }
	bool IsGuard() const { return _isGuard; }
	// 受けたダメージを取得
	int GetSustainedDamage() const { return _sustainedDamage; }
	// ノックバックダメージを取得
	int GetKnockbackDamage() const { return _knockbackDamage; }
	bool IsDead() const { return _isDead; }

    void SetMovement(const Vector2& movement) { _movement = movement; }
    void SetIsMoving(bool isMoving) { _isMoving = isMoving; }
    void SetIsDash(bool isDush) { _isDash = isDush; }
    void SetIsEvade(bool isEvade) { _isEvade = isEvade; }
    void SetIsAttack(bool isAttack) { _isAttack = isAttack; }
    void SetIsGuard(bool isGuard) { _isGuard = isGuard; }
    void SetSustainedDamage(int damage) { _sustainedDamage = damage; }
    void SetKnockbackDamage(int damage) { _knockbackDamage = damage; }
    void SetIsDead(bool isDead) { _isDead = isDead; }
#pragma endregion

private:
#pragma region ステート制御
	// ステートマシン
	std::unique_ptr<PlayerStateMachine> _stateMachine;

    // 入力方向をワールド空間に変換したもの
    Vector2 _movement = { 0.0f, 0.0f };
	bool _isMoving = false;
	bool _isDash = false;
	bool _isEvade = false;
	bool _isAttack = false;
	bool _isGuard = false;
	// 受けたダメージダメージ
	int _sustainedDamage = 0;
	int _knockbackDamage = 5;
	bool _isDead = false;
#pragma endregion
    // プレイヤーの状態
    PlayerMainStates _state = PlayerMainStates::None;

	std::weak_ptr<CharactorController> _charactorController;
	std::weak_ptr<Animator> _animator;

	// 移動速度
	float _moveSpeed = 20.0f;
	// 摩擦力
	float _friction = 25.0f;
};