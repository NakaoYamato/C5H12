#pragma once

#include <variant>
#include "../../Library/Component/StateController.h"
#include "../../Library/Component/EffectController.h"
#include "../../Library/Math/Vector.h"

#include "../../Source/Common/DamageSender.h"

// 前方宣言
class PlayerController;
class Animator;

// プレイヤーの状態遷移を管理するクラス
class PlayerStateMachine : public StateMachine
{
public:
    PlayerStateMachine(Actor* owner);
    ~PlayerStateMachine() {}

	// 開始処理
	void Start();
	// 実行処理
	void Execute(float elapsedTime) override;
	// Gui描画
	void DrawGui() override;

	/// <summary>
	/// 移動方向に向く
	/// </summary>
	/// <param name="elapsedTime">経過時間</param>
	/// <param name="rotationSpeed">回転速度</param>
	void RotationMovement(float elapsedTime, float rotationSpeed = 1.0f);
#pragma region アクセサ
	StateMachineBase<PlayerStateMachine>& GetStateMachine() { return _stateMachine; }
	PlayerController*	GetPlayer()			{ return _player; }
	DamageSender*		GetDamageSender()	{ return _damageSender; }
	Animator*			GetAnimator()		{ return _animator; }
	EffectController*	GetEffect()			{ return _effect; }
	// ステート変更
	void ChangeState(const char* mainStateName, const char* subStateName) override;
    // ステート名取得
	const char* GetStateName() override;
    // サブステート名取得
	const char* GetSubStateName() override;
#pragma endregion
private:
	StateMachineBase<PlayerStateMachine> _stateMachine;
	PlayerController*					_player = nullptr;
	DamageSender*						_damageSender = nullptr;
	Animator*							_animator = nullptr;
	EffectController*					_effect = nullptr;
};

// プレイヤーのヒエラルキカルステートのベースクラス
#pragma region ベースステート
// OnEnterでアニメーションを再生するだけの簡易ステート
class PlayerHSB : public HierarchicalStateBase<PlayerStateMachine>
{
public:
	PlayerHSB(PlayerStateMachine* stateMachine,
		const std::string& animationName,
		float blendSeconds,
		bool isLoop,
		bool isUsingRootMotion) :
		HierarchicalStateBase(stateMachine),
		_animationName(animationName),
		_blendSeconds(blendSeconds),
		_isLoop(isLoop),
		_isUsingRootMotion(isUsingRootMotion)
	{
	}
	~PlayerHSB() override {}
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
class Player8WayHSB : public HierarchicalStateBase<PlayerStateMachine>
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
	Player8WayHSB(PlayerStateMachine* stateMachine,
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
class PlayerSSB : public StateBase<PlayerStateMachine>
{
public:
	PlayerSSB(PlayerStateMachine* stateMachine,
		const std::string& name,
		const std::string& animationName,
		float blendSeconds,
		bool isLoop,
		bool isUsingRootMotion) :
		StateBase(stateMachine),
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
private:
	std::string _name = "";
	std::string _animationName = "";
	float		_blendSeconds = 0.2f;
	bool		_isLoop = false;
	bool		_isUsingRootMotion = false;
};
#pragma endregion
