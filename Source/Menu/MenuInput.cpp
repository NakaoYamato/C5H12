#include "MenuInput.h"

#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Scene/Scene.h"
#include "../../Library/Scene/SceneManager.h"

#include <imgui.h>

// 更新処理
void MenuInput::OnUpdate(float elapsedTime)
{
	// 入力状態を更新
	_inputFlags = 0;
	//if (_INPUT_TRIGGERD("Up"))
	//	_inputFlags |= static_cast<int>(InputType::Up);
	//if (_INPUT_TRIGGERD("Down"))
	//	_inputFlags |= static_cast<int>(InputType::Down);
	//if (_INPUT_TRIGGERD("Left"))
	//	_inputFlags |= static_cast<int>(InputType::Left);
	//if (_INPUT_TRIGGERD("Right"))
	//	_inputFlags |= static_cast<int>(InputType::Right);
	//if (_INPUT_TRIGGERD("Select"))
	//	_inputFlags |= static_cast<int>(InputType::Select);
	//if (_INPUT_TRIGGERD("Back"))
	//	_inputFlags |= static_cast<int>(InputType::Back);

    TextRenderer::TextDrawData textData;
    textData.type = FontType::MSGothic;
    if (Input::Instance().GetCurrentInputDevice() == Input::InputType::XboxPad)
		textData.text = L"ゲームを終了しますか？ A->終了, Back->再開";
	else
		textData.text = L"ゲームを終了しますか？ Space->終了, Esc->再開";
	textData.text += L"\n1:タイトルシーン、2:ゲームシーン、3:プレイヤーデバッグシーン";
    textData.position = Vector2(100.0f, 100.0f);
    textData.color = Vector4::Black;
    textData.scale = Vector2(1.0f, 1.0f);
    GetActor()->GetScene()->GetTextRenderer().Draw(textData);

	// 戻る
	if (_INPUT_TRIGGERD("Menu"))
	{
        _inputManager->SwitchPreviousInput();
	}
	// 終了
	if (_INPUT_TRIGGERD("Evade"))
	{
		PostMessage(Graphics::Instance().GetHwnd(), WM_CLOSE, 0, 0);
    }
	// シーン切り替え
	if (_INPUT_TRIGGERD("1"))
	{
		SceneManager::Instance().ChangeScene(SceneMenuLevel::Game, "Title");
	}
	if (_INPUT_TRIGGERD("2"))
	{
		SceneManager::Instance().ChangeScene(SceneMenuLevel::Game, "Game");
	}
	if (_INPUT_TRIGGERD("3"))
	{
		SceneManager::Instance().ChangeScene(SceneMenuLevel::Debug, "PlayerDebug");
	}
}
// GUI描画
void MenuInput::DrawGui()
{
	bool flag = false;
	flag = IsInput(InputType::Up);
	ImGui::Checkbox("Up", &flag);
	flag = IsInput(InputType::Down);
	ImGui::Checkbox("Down", &flag);
	flag = IsInput(InputType::Left);
	ImGui::Checkbox("Left", &flag);
	flag = IsInput(InputType::Right);
	ImGui::Checkbox("Right", &flag);
	flag = IsInput(InputType::Select);
	ImGui::Checkbox("Select", &flag);
	flag = IsInput(InputType::Back);
	ImGui::Checkbox("Back", &flag);
}

void MenuInput::CloseMenu()
{
	// 直前の入力コントローラーに戻す
	_inputManager->SwitchPreviousInput();
}
