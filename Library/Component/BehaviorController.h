#pragma once

#include "../../Library/Component/Component.h"
#include "../../Library/Algorithm/BehaviorTree/BehaviorTree.h"

class BehaviorTree
{
public:
	BehaviorTree() {}
	~BehaviorTree() {}
	// 開始処理
	virtual void Start() {}
	// 実行処理
	virtual void Execute(float elapsedTime) {}
	// GUI描画
	virtual void DrawGui() {}
};

class BehaviorController : public Component
{
public:
	BehaviorController(std::shared_ptr<BehaviorTree> behaviorTree);
	~BehaviorController() override {}
	// 名前取得
	const char* GetName() const override { return "BehaviorController"; }
	// 開始処理
	void Start() override;
	// 更新処理
	void Update(float elapsedTime) override;
	// GUI描画
	void DrawGui() override;
	// ビヘイビアツリーを実行するかどうか
	bool IsExecute() const { return _isExecute; }
	// 実行フラグ取得
	void SetIsExecute(bool execute) { _isExecute = execute; }
	// ビヘイビアツリーを取得
	std::shared_ptr<BehaviorTree> GetBehaviorTree() const { return _behaviorTree; }
private:
	std::shared_ptr<BehaviorTree> _behaviorTree = nullptr;
	// ビヘイビアツリーを実行するかどうか
	bool _isExecute = true;
};