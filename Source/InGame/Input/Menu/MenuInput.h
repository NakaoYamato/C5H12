#pragma once

#include "../../Source/InGame/InputManager.h"
#include "../../Source/InGame/UI/Menu/MenuUIController.h"

class MenuInput : public InputControllerBase
{
public:
	enum class InputType
	{
		Up		= 0b00000001,
		Down	= 0b00000010,
		Left	= 0b00000100,
		Right	= 0b00001000,
		Select	= 0b00010000,
		Back	= 0b00100000,
	};

public:
	MenuInput() {}
	~MenuInput() override {}
	// 名前取得
	const char* GetName() const override { return "MenuInput"; }
	// 開始処理
	void Start() override;
	// GUI描画
	void DrawGui() override;

	// メニュー画面入力を終了
	void CloseMenu();

	// 起動時関数
	void OnEntry() override;
	// 終了時処理
	void OnExit() override;
protected:
	// 更新時処理
	void OnUpdate(float elapsedTime) override;

private:
	// 前フレームの入力状態
	unsigned int _previousInputDirection = 0;
	// 入力の継続時間
	float _inputHoldTime = 0.0f;
	// 連続入力までの時間
	float _inputHoldThreshold = 0.25f;
	// 連続入力時の入力間隔時間
	float _inputRepeatInterval = 0.1f;

	std::weak_ptr<MenuUIController> _menuUIController;
};