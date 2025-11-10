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
	void OnExecute() override;
	// 終了時関数
	void OnEnd() override;

	std::weak_ptr<ChestUIController> GetChestUIController() const { return _chestUIController; }
protected:
	// 更新時処理
	void OnUpdate(float elapsedTime)  override;

private:
	std::weak_ptr<ChestUIController> _chestUIController;
};
