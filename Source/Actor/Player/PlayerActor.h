#pragma once

#include "../../Library/Actor/Actor.h"

class PlayerActor : public Actor
{
public:
    PlayerActor(bool isUserControlled = true) : _isUserControlled(isUserControlled) {}
	~PlayerActor()override {}

	// 生成時処理
	void OnCreate() override;

	// 削除処理
	void Destroy() override;

private:
	// ユーザーが操作するプレイヤーか
	const bool _isUserControlled = true;

    std::weak_ptr<Actor> _swordActor;
    std::weak_ptr<Actor> _shieldActor;
};