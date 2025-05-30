#pragma once

#include "../../Library/Actor/Actor.h"

#include "../../Library/Component/CharactorController.h"
#include "../../Source/Common/Damageable.h"
class EnemyActor : public Actor
{
public:
	EnemyActor() {}
	~EnemyActor() override {}
	// 生成時処理
	void OnCreate() override;

#pragma region アクセサ
	std::weak_ptr<CharactorController> GetCharactorController() const
	{
		return _charactorController;
	}
	std::weak_ptr<Damageable> GetDamageable() const
	{
		return _damageable;
	}

	void SetIsExecuteBehaviorTree(bool execute) { _isExecuteBehavior = execute; }
#pragma endregion
protected:
	std::weak_ptr<CharactorController> _charactorController;
	std::weak_ptr<Damageable> _damageable;

	// 行動遷移処理を処理するかどうか
	bool _isExecuteBehavior = true;
};