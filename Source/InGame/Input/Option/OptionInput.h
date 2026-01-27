#pragma once

#include "../../Source/InGame/InputManager.h"
#include "../../UI/Option/OptionUIActor.h"

class OptionInput : public InputControllerBase
{
public:
	OptionInput() {}
	~OptionInput() override {}
	// 名前取得
	const char* GetName() const override { return "OptionInput"; }
	// 開始処理
	void Start() override;
	// GUI描画
	void DrawGui() override;
	// 起動時関数
	void OnEntry() override;
	// 終了時関数
	void OnExit() override;
	// 更新時処理
	void OnUpdate(float elapsedTime)  override;

private:
	std::weak_ptr<OptionUIActor> _optionUIActor;
};