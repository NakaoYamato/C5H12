#pragma once

#include "../../Source/InGame/InputMediator.h"

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
	const char* GetName() const override { return InputMediator::MenuInputName; }
	// 開始処理
	void Start() override;
	// 更新処理
	void Update(float elapsedTime) override;
	// GUI描画
	void DrawGui() override;
	// 他のInputControllerから命令を受信
	void ReceiveCommandFromOther(InputMediator::CommandType commandType, const std::string& command) override;
	// 入力状態を取得
	bool IsInput(InputType inputType) const
	{
		return (_inputFlags & static_cast<int>(inputType)) != 0;
	}
	// メニュー画面入力を終了
	void CloseMenu();
private:
	// 現在の入力状態をビットフラグで保持
	int _inputFlags = 0;
};