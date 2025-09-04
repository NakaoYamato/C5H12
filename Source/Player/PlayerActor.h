#pragma once

#include "../../Library/Actor/Actor.h"
#include "PlayerController.h"
#include "../../Source/Common/Damageable.h"

class PlayerActor : public Actor
{
public:
	static const char* GetModelFilePath()
	{
		return "./Data/Model/Player/GreatSword/Seq1.fbx";
	}
public:
    PlayerActor(bool isUserControlled = true) : _isUserControlled(isUserControlled) {}
	~PlayerActor()override {}

	// 生成時処理
	void OnCreate() override;
	// 削除処理
	void OnDeleted() override;

private:
	// ユーザーが操作するプレイヤーか
	const bool _isUserControlled = true;
    std::weak_ptr<Actor> _swordActor;
    std::weak_ptr<Actor> _shieldActor;
};