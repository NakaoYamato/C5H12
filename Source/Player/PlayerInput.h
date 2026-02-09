#pragma once

#include "../../Source/InGame/InputManager.h"
#include "PlayerController.h"
#include "PlayerItemController.h"

#include "../../Source/Quest/QuestController.h"

class PlayerInput : public InputControllerBase
{
public:
	PlayerInput() {}
	~PlayerInput() override {}

	// 名前取得
	const char* GetName() const override { return "PlayerInput"; }
	// 開始処理
	void Start() override;
	// GUI描画
	void DrawGui() override;

	// 終了時処理
	void OnExit() override;

protected:
	// 更新時処理
	void OnUpdate(float elapsedTime)  override;

private:
	std::weak_ptr<PlayerController> _playerController;
	std::weak_ptr<PlayerItemController> _playerItemController;
	std::weak_ptr<QuestController> _questController;
};