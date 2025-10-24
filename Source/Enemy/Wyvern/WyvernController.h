#pragma once

#include "../../Library/Component/Component.h"
#include "../../Library/Component/BehaviorController.h"
#include "../EnemyController.h"
#include "../../Source/AI/MetaAI.h"
#include "../../Source/Common/CombatStatusController.h"

class WyvernController : public Component
{
public:
	WyvernController() {}
	~WyvernController() override {}
	// 名前取得
	const char* GetName() const override { return "WyvernController"; }
	// 開始処理
	void Start() override;
	// 更新処理
	void Update(float elapsedTime) override;
	// GUI描画
	void DrawGui() override;

#pragma region アクセサ
	float GetNearAttackRadian() const { return _nearAttackRadian; }
	float GetFlightSkinWidth() const { return _flightSkinWidth; }
	float GetFlightNearRange() const { return _flightNearRange; }
	float GetFlightMoveSpeed() const { return _flightMoveSpeed; }
	float GetFlightTimer() const { return _flightTimer; }
	float GetFlightDuration() const { return _flightDuration; }
	float GetChargeAttackChargeTime() const { return _chargeAttackChargeTime; }
	bool IsDuringFlight() const { return _isDuringFlight; }

	void SetNearAttackRadian(float nearAttackRadian) { _nearAttackRadian = nearAttackRadian; }
	void SetIsDuringFlight(bool f) { _isDuringFlight = f;; }
#pragma endregion
private:
	std::weak_ptr<CharactorController> _charactorController;
	// エネミーコントローラー
	std::weak_ptr<EnemyController> _enemyController;
	// ビヘイビアコントローラー
	std::weak_ptr<BehaviorController> _behaviorController;
	// 戦闘状態
	std::weak_ptr<CombatStatusController> _combatStatus;
	// メタAI
	std::weak_ptr<MetaAI> _metaAI;
	// 近接攻撃ができる角度
	float _nearAttackRadian = DirectX::XMConvertToRadians(40.0f);
	// 空中でのスキン幅
	float _flightSkinWidth = 3.5f;
	// 初期スキン幅
	float _initialSkinWidth = 0.02f;
	// 飛行中か
	bool _isDuringFlight = false;
	// 飛行中の近距離判定距離
	float _flightNearRange = 10.0f;
	// 飛行中の移動速度
	float _flightMoveSpeed = 8.0f;
	// 空中時のタイマー
	float _flightTimer = 0.0f;
	// 空中継続時間
	float _flightDuration = 20.0f;

	// 突進攻撃の溜め時間
	float _chargeAttackChargeTime = 1.0f;
};