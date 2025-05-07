#pragma once

#include "../../Library/Component/Component.h"
#include "../../Library/Component/CharactorController.h"
#include "PlayerInput.h"

class PlayerController : public Component
{
public:
	PlayerController() {}
	~PlayerController() override {}

	// 名前取得
	const char* GetName() const override { return "PlayerController"; }

	// 開始処理
	void Start() override;

	// 更新処理
	void Update(float elapsedTime) override;

	// GUI描画
	void DrawGui() override;

private:
	std::weak_ptr<CharactorController> _charactorController;
	std::weak_ptr<PlayerInput> _playerInput;

	float _moveSpeed = 10.0f;
};