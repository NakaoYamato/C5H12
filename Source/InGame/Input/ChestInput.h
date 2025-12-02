#pragma once

#include "../../Source/InGame/InputManager.h"
#include "../UI/Chest/ChestUIController.h"

class ChestInput : public InputControllerBase
{
public:
	ChestInput() {}
	~ChestInput() override {}
	// 名前取得
	const char* GetName() const override { return "ChestInput"; }
	// 開始処理
	void Start() override;
	// GUI描画
	void DrawGui() override;

	// 起動時関数
	void OnEntry() override;
	// 終了時関数
	void OnExit() override;

	std::weak_ptr<ChestUIController> GetChestUIController() const { return _chestUIController; }
protected:
	// 更新時処理
	void OnUpdate(float elapsedTime)  override;

private:
	std::weak_ptr<ChestUIController> _chestUIController;

	// 前フレームの入力状態
	unsigned int _previousInputDirection = 0;
	// 入力の継続時間
	float _inputHoldTime = 0.0f;
	// 連続入力までの時間
	float _inputHoldThreshold = 0.25f;
	// 連続入力時の入力間隔時間
	float _inputRepeatInterval = 0.1f;
};
