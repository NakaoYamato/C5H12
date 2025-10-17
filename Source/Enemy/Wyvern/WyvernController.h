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
	// オブジェクトとの接触した瞬間時の処理
	void OnContactEnter(CollisionData& collisionData) override;

#pragma region アクセサ
	float GetNearAttackRadian() const { return _nearAttackRadian; }
	float GetAirborneSkinWidth() const { return _airborneSkinWidth; }
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
	float _airborneSkinWidth = 3.5f;
	// 初期スキン幅
	float _initialSkinWidth = 0.02f;
	// 飛行中か
	bool _isDuringFlight = false;
};