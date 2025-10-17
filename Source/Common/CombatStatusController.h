#pragma once

#include "../../Library/Component/Component.h"
#include "../../Source/AI/MetaAI.h"
#include "Targetable.h"

class CombatStatusController : public Component
{
public:
	enum class Status
	{
		Normal,		// 通常
		Alert,		// 警戒
		Combat,		// 戦闘
	};

public:
	CombatStatusController() {}
	~CombatStatusController() override {}
	// 名前取得
	const char* GetName() const override { return "CombatStatusController"; }
	// 開始処理
	void Start() override;
	// 更新処理
	void Update(float elapsedTime) override;
	// GUI描画
	void DrawGui() override;

	// 現在位置からターゲットまでのベクトル取得
	Vector3 GetToTargetVec(const Vector3& target);
	// 現在位置からターゲットまでのベクトル取得
	Vector3 GetToTargetVec() { return GetToTargetVec(_targetPosition); };
	// ターゲットの範囲内かどうか
	bool IsInTargetRange(const Vector3& targetPosition, float targetRadius = 1.0f);
	// ターゲットの範囲内かどうか
	bool IsInTargetRange() { return IsInTargetRange(_targetPosition, _targetRadius); }
#pragma region アクセサ
	// 現在の状態を取得
	Status GetCurrentStatus() const { return _currentStatus; }
	// ターゲット陣営を取得	
	Targetable::Faction GetTargetFaction() const { return _targetFaction; }
	// ターゲット座標を取得
	const Vector3& GetTargetPosition() const { return _targetPosition; }
	// ターゲットの半径を取得
	float GetTargetRadius() const { return _targetRadius; }
	// 検索範囲を取得
	float GetSearchRange() const { return _searchRange; }
	// 戦闘継続範囲を取得
	float GetCombatRange() const { return _combatRange; }
	// 状態継続タイマーを取得
	float GetStatusTimer() const { return _statusTimer; }

	// 状態を設定
	void SetStatus(Status status) { 
		if (_currentStatus == status) return;
		_currentStatus = status;
		_statusTimer = 0.0f;
	}
	// ターゲット陣営を設定	
	void SetTargetFaction(Targetable::Faction faction) { _targetFaction = faction; }
	// ターゲット座標を設定	
	void SetTargetPosition(const Vector3& position) { _targetPosition = position; }
	// ターゲットの半径を設定	
	void SetTargetRadius(float radius) { _targetRadius = radius; }
	// 検索範囲を設定
	void SetSearchRange(float range) { _searchRange = range; }
	// 戦闘継続範囲を設定	
	void SetCombatRange(float range) { _combatRange = range; }
	// 更新するかどうかを設定
	void SetIsUpdate(bool isUpdate) { _isUpdate = isUpdate; }
#pragma endregion

private:
	// 通常状態の更新
	void UpdateNormalStatus(float elapsedTime);
	// 警戒状態の更新
	void UpdateAlertStatus(float elapsedTime);
	// 戦闘状態の更新
	void UpdateCombatStatus(float elapsedTime);

private:
	// メタAI
	std::weak_ptr<MetaAI> _metaAI;
	// 現在の状態
	Status _currentStatus = Status::Normal;
	// ターゲット陣営
	Targetable::Faction _targetFaction = Targetable::Faction::Player;
	// ターゲット
	Vector3 _targetPosition = Vector3::Zero;
	// ターゲットの半径
	float _targetRadius = 1.0f;
	// 検索範囲
	float _searchRange = 30.0f;
	// 戦闘継続範囲
	float _combatRange = 60.0f;
	// 状態継続タイマー
	float _statusTimer = 0.0f;

	// 更新するかどうか
	bool _isUpdate = true;
};