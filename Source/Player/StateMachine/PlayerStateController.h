#pragma once

#include "../../Library/Component/StateController.h"
#include "../../Library/Component/EffectController.h"

#include "../../Source/Common/DamageSender.h"
#include "../../Source/Common/StaminaController.h"
#include "../../Source/InGame/UI/Operate/OperateUIController.h"
#include "../../Source/Player/PlayerItemController.h"

// 前方宣言
class PlayerController;
class Animator;

// プレイヤーの状態遷移を管理するクラス
class PlayerStateController : public StateController2<PlayerStateController>
{
public:
	PlayerStateController() : StateController2<PlayerStateController>() {}
	~PlayerStateController() override {}

	// 名前取得
	const char* GetName() const override { return "PlayerStateController"; }
	// オブジェクトとの接触時の処理
	void OnContact(CollisionData& collisionData) override;

	// 抜刀移行
	void ChangeToCombatState(const std::string& mainStateName);
	// 納刀移行
	void ChangeToNonCombatState(const std::string& mainStateName);

	/// <summary>
	/// 移動方向に向く
	/// </summary>
	/// <param name="elapsedTime">経過時間</param>
	/// <param name="rotationSpeed">回転速度</param>
	void RotationMovement(float elapsedTime, float rotationSpeed = 1.0f);
#pragma region アクセサ
	// ステートマシン取得
	std::shared_ptr<SM> GetStateMachine() override;
	PlayerController* GetPlayer() { return _player; }
	DamageSender* GetDamageSender() { return _damageSender; }
	StaminaController* GetStaminaController() { return _staminaController; }
	Animator* GetAnimator() { return _animator; }
	EffectController* GetEffect() { return _effect; }
	OperateUIController* GetOperateUIController() { return _operateUIController; }
	PlayerItemController* GetItemController() { return _itemController; }

	bool IsCombatState() const { return _isCombatState; }
#pragma endregion

	// 汎用遷移
	void ChangeItemState();

	void SetNextSubStateName(const std::string& subStateName) { _nextSubStateName = subStateName; }
	void SetNextBranchSubStateName(const std::string& subStateName) { _nextBranchSubStateName = subStateName; }
	std::string GetNextSubStateName() const { return _nextSubStateName; }
	std::string GetNextBranchSubStateName() const { return _nextBranchSubStateName; }

#pragma region 武器種ごとのステートマシン設定
	void SetGreatSwordStateMachine();
#pragma endregion

protected:
	// 開始時処理
	void OnStart() override;
	// 遅延更新処理
	void OnLateUpdate(float elapsedTime) override;
	// GUI描画
	void OnDrawGui() override;

private:
	// 抜刀時のステートマシン
	std::shared_ptr<SM> _combatStateMachine = nullptr;

	// 抜刀状態か
	bool _isCombatState = false;

	PlayerController*		_player				= nullptr;
	DamageSender*			_damageSender		= nullptr;
	StaminaController*		_staminaController	= nullptr;
	Animator*				_animator			= nullptr;
	EffectController*		_effect				= nullptr;
	OperateUIController*	_operateUIController = nullptr;
	PlayerItemController*	_itemController		= nullptr;

	// 先行入力遷移先
	std::string _nextSubStateName = "";
	std::string _nextBranchSubStateName = "";
};

// プレイヤーのヒエラルキカルステートのベースクラス
#pragma region ベースステート
using PlayerHSBBase = HierarchicalStateBase<PlayerStateController>;
using PlayerSSBBase = StateBase<PlayerStateController>;

// OnEnterでアニメーションを再生するだけの簡易ステート
class PlayerAnimationHSB : public PlayerHSBBase
{
public:
	PlayerAnimationHSB(PlayerStateController* owner,
		const std::string& animationName,
		float blendSeconds,
		bool isLoop,
		bool isUsingRootMotion) :
		HierarchicalStateBase(owner),
		_animationName(animationName),
		_blendSeconds(blendSeconds),
		_isLoop(isLoop),
		_isUsingRootMotion(isUsingRootMotion)
	{
	}
	~PlayerAnimationHSB() override {}
	virtual void OnEnter() override;
	virtual void OnExecute(float elapsedTime) override {}
	virtual void OnExit() override {}
private:
	std::string _animationName = "";
	float		_blendSeconds = 0.2f;
	bool		_isLoop = false;
	bool		_isUsingRootMotion = false;
};

// 8方向のサブステートを持つヒエラルキカルステート
class Player8WayHSB : public PlayerHSBBase
{
public:
	enum Direction
	{
		Front = 0,
		FrontRight,
		Right,
		BackRight,
		Back,
		BackLeft,
		Left,
		FrontLeft,

		NumDirections,
	};

public:
	Player8WayHSB(PlayerStateController* owner,
		std::vector<std::string> animationNames,
		float blendSeconds,
		bool isUsingRootMotion);
	~Player8WayHSB() override {}
	virtual void OnEnter() override {}
	virtual void OnExecute(float elapsedTime) override {}
	virtual void OnExit() override {}
	void ChangeSubState(Direction animationIndex);
};

// アニメーション再生のみの簡易サブステート
class PlayerSSB : public PlayerSSBBase
{
public:
	PlayerSSB(PlayerStateController* owner,
		const std::string& name,
		const std::string& animationName,
		float blendSeconds,
		bool isLoop,
		bool isUsingRootMotion) :
		StateBase(owner),
		_name(name),
		_animationName(animationName),
		_blendSeconds(blendSeconds),
		_isLoop(isLoop),
		_isUsingRootMotion(isUsingRootMotion)
	{
	}
	const char* GetName() const override { return _name.c_str(); }
	void OnEnter() override;
	void OnExecute(float elapsedTime) override {}
	void OnExit() override {}
protected:
	std::string _name = "";
	std::string _animationName = "";
	float		_blendSeconds = 0.2f;
	bool		_isLoop = false;
	bool		_isUsingRootMotion = false;
};
#pragma endregion