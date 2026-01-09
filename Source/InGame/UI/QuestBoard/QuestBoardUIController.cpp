#include "QuestBoardUIController.h"

#include "../../Library/Actor/UI/UIActor.h"

#include <imgui.h>

// 開始処理
void QuestBoardUIController::Start()
{
	// 各メニューUIをoff
	if (auto selectMenu = _selectMenu.lock())
		selectMenu->GetActor()->SetIsActive(false);
	if (auto newOrderMenu = _newOrderMenu.lock())
		newOrderMenu->GetActor()->SetIsActive(false);
	if (auto acceptMenu = _acceptMenu.lock())
		acceptMenu->GetActor()->SetIsActive(false);
}

// 更新処理
void QuestBoardUIController::Update(float elapsedTime)
{
	switch (_state)
	{
	case State::SelectMenu:
		SelectInput();
		break;
	case State::NewOrderMenu:
		NewOrderMenuInput();
		break;
	case State::AcceptMenu:
		AcceptMenuInput();
		break;
	}

	_inputState = InputState::None;
}

// GUI描画
void QuestBoardUIController::DrawGui()
{
}

// ステート初期化
void QuestBoardUIController::InitState()
{
	_state = State::SelectMenu;
}

// 開く
void QuestBoardUIController::Open()
{
	this->GetActor()->SetIsActive(true);
	InitState();
	if (auto selectMenu = _selectMenu.lock())
	{
		selectMenu->GetActor()->SetIsActive(true);
		selectMenu->Reset();
	}
	if (auto newOrderMenu = _newOrderMenu.lock())
	{
		newOrderMenu->GetActor()->SetIsActive(false);
		newOrderMenu->Reset();
	}
	if (auto acceptMenu = _acceptMenu.lock())
	{
		acceptMenu->GetActor()->SetIsActive(false);
		acceptMenu->Reset();
	}
}

// 閉じる
void QuestBoardUIController::Close()
{
	InitState();
	if (auto selectMenu = _selectMenu.lock())
		selectMenu->GetActor()->SetIsActive(false);
	if (auto newOrderMenu = _newOrderMenu.lock())
		newOrderMenu->GetActor()->SetIsActive(false);
	if (auto acceptMenu = _acceptMenu.lock())
		acceptMenu->GetActor()->SetIsActive(false);
	this->GetActor()->SetIsActive(false);
}

// Selectメニューの入力処理
void QuestBoardUIController::SelectInput()
{
	auto selectMenu = _selectMenu.lock();
	if (!selectMenu)
		return;
	selectMenu->AddIndex(_inputDirection);
	switch (_inputState)
	{
	case InputState::Select:
		switch (selectMenu->GetSelectIndex())
		{
		case QuestBoardSelectMenuController::SelectMenuOption::NewOrderOption:
			selectMenu->GetActor()->SetIsActive(false);
			if (auto newOrderMenu = _newOrderMenu.lock())
				newOrderMenu->GetActor()->SetIsActive(true);
			_state = State::NewOrderMenu;
			break;
		case QuestBoardSelectMenuController::SelectMenuOption::AcceptOption:
			selectMenu->GetActor()->SetIsActive(false);
			if (auto acceptMenu = _acceptMenu.lock())
				acceptMenu->GetActor()->SetIsActive(true);
			_state = State::AcceptMenu;
			break;
		}
		break;
	case InputState::Back:
		// UIを閉じる
		Close();
		break;
	}
}

// クエスト新規受注メニューの入力処理
void QuestBoardUIController::NewOrderMenuInput()
{
	auto newOrderMenu = _newOrderMenu.lock();
	if (!newOrderMenu)
		return;
	newOrderMenu->AddIndex(_inputDirection);
	switch (_inputState)
	{
	case InputState::Select:
		newOrderMenu->NextState();
		break;
	case InputState::Back:
		if (newOrderMenu->PreviousState())
		{
			newOrderMenu->GetActor()->SetIsActive(false);
			if (auto selectMenu = _selectMenu.lock())
				selectMenu->GetActor()->SetIsActive(true);
			_state = State::SelectMenu;
		}
		break;
	}
}

// クエスト参加メニューの入力処理
void QuestBoardUIController::AcceptMenuInput()
{
	auto acceptMenu = _acceptMenu.lock();
	if (!acceptMenu)
		return;
	acceptMenu->AddIndex(_inputDirection);
	switch (_inputState)
	{
	case InputState::Select:
		acceptMenu->NextState();
		break;
	case InputState::Back:
		if (acceptMenu->PreviousState())
		{
			acceptMenu->GetActor()->SetIsActive(false);
			if (auto selectMenu = _selectMenu.lock())
				selectMenu->GetActor()->SetIsActive(true);
			_state = State::SelectMenu;
		}
		break;
	}
}
