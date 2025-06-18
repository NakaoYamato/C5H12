#pragma once

#include "../../Source/Mediator/InputMediator.h"
#include "PlayerController.h"

class PlayerInput : public InputControllerBase
{
public:
	PlayerInput() {}
	~PlayerInput() override {}

	// 名前取得
	const char* GetName() const override { return InputMediator::PlayerInputName; }
	// 開始処理
	void Start() override;
	// 更新処理
	void Update(float elapsedTime) override;
	// GUI描画
	void DrawGui() override;

	// 他のInputControllerから命令を受信
	void ReceiveCommandFromOther(InputMediator::CommandType commandType, const std::string& command) override;
private:
	std::weak_ptr<PlayerController> _playerController;
};