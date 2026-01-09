#pragma once

#include "../../Library/Component/Component.h"
#include "../../Library/Scene/Scene.h"

#include "Menu/QuestBoardSelectMenuController.h"
#include "Menu/QuestBoardNewOrderMenuController.h"
#include "Menu/QuestBoardAcceptMenuController.h"

class QuestBoardUIController : public Component
{
public:
	enum class State
	{
		SelectMenu,

		NewOrderMenu,
		AcceptMenu,
	};

	enum class InputDirection : unsigned int
	{
		None = 0b0000,
		Up = 0b0001,
		Down = 0b0010,
		Left = 0b0100,
		Right = 0b1000,
	};
	enum class InputState
	{
		None,

		Select,
		Back,
		L3,
		R3,
	};

public:
	QuestBoardUIController() {}
	~QuestBoardUIController() override {}
	// 名前取得
	const char* GetName() const override { return "QuestBoardUIController"; }
	// 開始処理
	void Start() override;
	// 更新処理
	void Update(float elapsedTime) override;
	// GUI描画
	void DrawGui() override;

	// ステート初期化
	void InitState();

	// 開く
	void Open();
	// 閉じる
	void Close();

	// 入力値設定
	void SetInputState(InputState inputState) { _inputState = inputState; }
	// 入力値設定
	void SetInputDirection(int inputDirection) { _inputDirection = inputDirection; }

	void SetSelectMenu(const std::shared_ptr<QuestBoardSelectMenuController>& menu) { _selectMenu = menu; }
	void SetNewOrderMenu(const std::shared_ptr<QuestBoardNewOrderMenuController>& menu) { _newOrderMenu = menu; }
	void SetAcceptMenu(const std::shared_ptr<QuestBoardAcceptMenuController>& menu) { _acceptMenu = menu; }
private:
	// Selectメニューの入力処理
	void SelectInput();
	// クエスト新規受注メニューの入力処理
	void NewOrderMenuInput();
	// クエスト参加メニューの入力処理
	void AcceptMenuInput();

private:
	std::weak_ptr<QuestBoardSelectMenuController> _selectMenu;
	std::weak_ptr<QuestBoardNewOrderMenuController> _newOrderMenu;
	std::weak_ptr<QuestBoardAcceptMenuController> _acceptMenu;

	// メニュー状態
	State _state = State::SelectMenu;
	// 入力値
	InputState _inputState = InputState::None;
	int _inputDirection = 0;
};