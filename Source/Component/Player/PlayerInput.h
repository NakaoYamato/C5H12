#pragma once

#include "../../Library/Component/Component.h"
#include "../../Source/Component/Player/PlayerController.h"

class PlayerInput : public Component
{
public:
	enum Inputs : int
	{
		Attack		= 0b00000001,
		Dash		= 0b00000010,
		Evade		= 0b00000100,
		Guard		= 0b00001000,
	};

public:
	PlayerInput() {}
	~PlayerInput() override {}

	// 名前取得
	const char* GetName() const override { return "PlayerInput"; }
	// 開始処理
	void Start() override;
	// 更新処理
	void Update(float elapsedTime) override;
	// GUI描画
	void DrawGui() override;
private:
	std::weak_ptr<PlayerController> _playerController;
};