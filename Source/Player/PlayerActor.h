#pragma once

#include "../../Library/Actor/Actor.h"
#include "PlayerController.h"
#include "../../Source/Common/Damageable.h"

class PlayerActor : public Actor
{
public:
    PlayerActor(bool isUserControlled = true) : _isUserControlled(isUserControlled) {}
	~PlayerActor()override {}

	// 生成時処理
	void OnCreate() override;

	// 削除処理
	void OnRemove() override;

#pragma region アクセサ
	std::shared_ptr<CharactorController> GetCharactorController()
	{
		return _charactorController.lock();
	}
	std::shared_ptr<PlayerController> GetPlayerController()
	{
		return _playerController.lock();
	}
	std::shared_ptr<Damageable> GetDamageable()
	{
		return _damageable.lock();
	}
#pragma endregion

private:
	// ユーザーが操作するプレイヤーか
	const bool _isUserControlled = true;
    std::weak_ptr<CharactorController>	_charactorController;
    std::weak_ptr<PlayerController>		_playerController;
	std::weak_ptr<Damageable>			_damageable;

    std::weak_ptr<Actor> _swordActor;
    std::weak_ptr<Actor> _shieldActor;
};