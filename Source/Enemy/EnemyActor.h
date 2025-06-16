#pragma once

#include "../../Library/Actor/Actor.h"

#include "../../Library/Component/CharactorController.h"
#include "../../Source/Common/Damageable.h"
#include "../../Source/Common/Targetable.h"

class EnemyActor : public Actor
{
public:
	EnemyActor() {}
	~EnemyActor() override {}
	// 生成時処理
	void OnCreate() override;

#pragma region アクセサ
	// ビヘイビアツリーを実行するかどうか
	virtual void SetIsExecuteBehaviorTree(bool execute) { _isExecuteBehavior = execute; }
#pragma endregion
protected:
	std::weak_ptr<CharactorController> _charactorController;
	std::weak_ptr<Damageable> _damageable;
	std::weak_ptr<Targetable> _targetable;

	// 行動遷移処理を処理するかどうか
	bool _isExecuteBehavior = true;
};