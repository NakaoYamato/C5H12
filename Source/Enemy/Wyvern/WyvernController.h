#pragma once

#include "../../Library/Component/Component.h"
#include "../EnemyController.h"
#include "../../Source/AI/MetaAI.h"
#include "../../Library/Component/BehaviorController.h"

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

	void SetNearAttackRadian(float nearAttackRadian) { _nearAttackRadian = nearAttackRadian; }
#pragma endregion
private:
	std::weak_ptr<EnemyController> _enemyController;
	// ビヘイビアコントローラー
	std::weak_ptr<BehaviorController> _behaviorController;
	// メタAI
	std::weak_ptr<MetaAI> _metaAI;
	// 近接攻撃ができる角度
	float _nearAttackRadian = DirectX::XMConvertToRadians(40.0f);
};