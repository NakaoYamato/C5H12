#pragma once

#include "../EnemyController.h"
#include "../../Source/StateMachine/Enemy/Wyvern/WyvernStateMachine.h" 
#include "../../Source/BehaviorTree/Enemy/Wyvern/WyvernBehaviorTree.h"

class WyvernEnemyController : public EnemyController
{
public:
	WyvernEnemyController() {}
	~WyvernEnemyController() override {}
	// 名前取得
	const char* GetName() const override { return "WyvernEnemyController"; }
	// 開始処理
	void Start() override;
	// 更新処理
	void Update(float elapsedTime) override;
	// GUI描画
	void DrawGui() override;

	// ダメージを与える
	void AddDamage(float damage, Vector3 hitPosition) override;

private:
	// ステートマシン
	std::unique_ptr<WyvernStateMachine> _stateMachine;
	// ビヘイビアツリー
	std::unique_ptr<WyvernBehaviorTree> _behaviorTree;
};