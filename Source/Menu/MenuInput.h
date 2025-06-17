#pragma once

#include "../../Source/Mediator/InputMediator.h"

class MenuInput : public InputControllerBase
{
public:
	MenuInput() {}
	~MenuInput() override {}
	// 名前取得
	const char* GetName() const override { return InputMediator::MenuInputName; }
	// 開始処理
	void Start() override;
	// 更新処理
	void Update(float elapsedTime) override;
	// GUI描画
	void DrawGui() override;
	// 他のInputControllerから命令を受信
	void ReceiveCommandFromOther(InputMediator::CommandType commandType, const std::string& command) override;
private:

};