#pragma once

#include "../../Library/Component/Component.h"

#include "WyvernEnemyController.h"
#include "BehaviorTree/WyvernBehaviorTree.h"
#include "../../Source/AI/MetaAI.h"

class WyvernBehaviorController : public Component
{
public:
	WyvernBehaviorController() {}
	~WyvernBehaviorController() override {}
	// 名前取得
	const char* GetName() const override { return "WyvernBehaviorController"; }
	// 開始処理
	void Start() override;
	// 更新処理
	void Update(float elapsedTime) override;
	// GUI描画
	void DrawGui() override;

	// ビヘイビアツリーを実行するかどうか
	void SetIsExecuteBehaviorTree(bool execute) { _isExecuteBehavior = execute; }
private:
	std::weak_ptr<WyvernEnemyController> _wyvernEnemyController;
	// ビヘイビアツリー
	std::unique_ptr<WyvernBehaviorTree> _behaviorTree;
	// ビヘイビアツリーを実行するかどうか
	bool _isExecuteBehavior = true;
	// メタAI
	std::weak_ptr<MetaAI> _metaAI;
};