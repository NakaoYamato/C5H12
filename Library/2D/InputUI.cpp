#include "InputUI.h"

#include "../../Library/Algorithm/Converter.h"
//#include "../../Library/2D/Canvas.h"
//#include "../../Library/Graphics/Graphics.h"

#include <Xinput.h>

#include <Mygui.h>

//std::unique_ptr<Canvas> g_inputUICanvas = nullptr;

// デバイスインデックス取得関数
static int GetDeviceIndex(Input::InputType type)
{
	switch (type)
	{
	case Input::InputType::Keyboard:
	case Input::InputType::Mouse:
		return 0;
	case Input::InputType::XboxPad:
		return 1;
	default:
		return -1;
	}
};

// 初期化
void InputUI::Initialize()
{
	//if (!g_inputUICanvas)
	//{
	//	g_inputUICanvas = std::make_unique<Canvas>(Graphics::Instance().GetDevice(),
	//		DirectX::XMUINT2(1280, 1280 * 2));
 //       g_inputUICanvas->SetFilePath("Data/Texture/UI/Input/KeyboardInputUI.png");
 //       g_inputUICanvas->Deserialize(Graphics::Instance().GetDevice(),
 //           Graphics::Instance().GetDeviceContext());
 //   }

	_gamePadSprite.LoadTexture(L"Data/Texture/UI/Input/XboxInputUI.png", Sprite::CenterCenter);
	_keybordSprite.LoadTexture(L"Data/Texture/UI/Input/KeyboardInputUI.png", Sprite::CenterCenter);

	auto SetData = [&](
		std::unordered_map<int, SprData>& sprData,
		int buttonID,
		int x,
		int y,
		int activeX,
		int activeY)
		{
			float fx = static_cast<float>(x);
			float fy = static_cast<float>(y);
			float afx = static_cast<float>(activeX);
			float afy = static_cast<float>(activeY);
			sprData[buttonID] =
			{
				{fx * SpriteWidth, fy * SpriteWidth},
				{afx * SpriteWidth, afy * SpriteWidth},
				{SpriteWidth,SpriteWidth}
			};
		};
	// ゲームパッド用スプライトデータ登録
	SetData(_gamePadSprData, XINPUT_GAMEPAD_DPAD_UP, 2, 0, 2, 0);
	SetData(_gamePadSprData, XINPUT_GAMEPAD_DPAD_DOWN, 3, 0, 3, 0);
	SetData(_gamePadSprData, XINPUT_GAMEPAD_DPAD_LEFT, 4, 0, 4, 0);
	SetData(_gamePadSprData, XINPUT_GAMEPAD_DPAD_RIGHT, 5, 0, 5, 0);
	SetData(_gamePadSprData, XINPUT_GAMEPAD_A, 8, 0, 9, 0);
	SetData(_gamePadSprData, XINPUT_GAMEPAD_B, 0, 1, 1, 1);
	SetData(_gamePadSprData, XINPUT_GAMEPAD_X, 2, 1, 3, 1);
	SetData(_gamePadSprData, XINPUT_GAMEPAD_Y, 4, 1, 5, 1);
	SetData(_gamePadSprData, XINPUT_GAMEPAD_LEFT_SHOULDER, 6, 1, 7, 1);
	SetData(_gamePadSprData, XINPUT_GAMEPAD_RIGHT_SHOULDER, 8, 1, 9, 1);
	SetData(_gamePadSprData, XBOXPAD_TRIGGER_L, 0, 2, 1, 2);
	SetData(_gamePadSprData, XBOXPAD_TRIGGER_R, 2, 2, 3, 2);
	SetData(_gamePadSprData, XINPUT_GAMEPAD_LEFT_THUMB, 4, 2, 5, 2);
	SetData(_gamePadSprData, XINPUT_GAMEPAD_RIGHT_THUMB, 6, 2, 7, 2);
	SetData(_gamePadSprData, XINPUT_GAMEPAD_START, 8, 2, 9, 2);
	SetData(_gamePadSprData, XINPUT_GAMEPAD_BACK, 0, 3, 1, 3);
	SetData(_gamePadSprData, XBOXPAD_AXIS_LX, 0, 4, 0, 4);
	SetData(_gamePadSprData, XBOXPAD_AXIS_LY, 9, 3, 9, 3);
	SetData(_gamePadSprData, XBOXPAD_AXIS_RX, 8, 4, 8, 4);
	SetData(_gamePadSprData, XBOXPAD_AXIS_RY, 7, 4, 7, 4);

	// キーボード、マウス用スプライトデータ登録
	// 数字
	for (int s = '0'; s <= '9'; s++)
	{
		int index = s - '0';
		int x = index % 5;
		int y = index / 5;
		SetData(_keybordSprData,
			s,
			x * 2,
			y,
			x * 2 + 1,
			y);
	}
	// ローマ字
	for (int s = 'A'; s <= 'Z'; s++)
	{
		int index = s - 'A';
		int x = index % 5;
		int y = index / 5 + 2;
		SetData(_keybordSprData,
			s,
			x * 2,
			y,
			x * 2 + 1,
			y);
	}
	SetData(_keybordSprData, VK_UP, 2, 7, 3, 7);
	SetData(_keybordSprData, VK_DOWN, 4, 7, 5, 7);
	SetData(_keybordSprData, VK_LEFT, 6, 7, 7, 7);
	SetData(_keybordSprData, VK_RIGHT, 8, 7, 9, 7);
	SetData(_keybordSprData, VK_BACK, 0, 8, 1, 8);
	SetData(_keybordSprData, VK_TAB, 2, 8, 3, 8);
	SetData(_keybordSprData, VK_RETURN, 4, 8, 5, 8);
	SetData(_keybordSprData, VK_SHIFT, 6, 8, 7, 8);
	SetData(_keybordSprData, VK_CONTROL, 8, 8, 9, 8);
	SetData(_keybordSprData, VK_ESCAPE, 0, 9, 1, 9);
	SetData(_keybordSprData, VK_MENU, 2, 9, 3, 9);
	SetData(_keybordSprData, VK_SPACE, 8, 9, 9, 9);
	SetData(_keybordSprData, KEYBORD_AXIS_LX, 2, 3, 7, 3);
	SetData(_keybordSprData, KEYBORD_AXIS_LY, 5, 6, 7, 6);
	SetData(_keybordSprData, KEYBORD_AXIS_RX, 7, 7, 9, 7);
	SetData(_keybordSprData, KEYBORD_AXIS_RY, 3, 7, 5, 7);

	SetData(_keybordSprData, VK_LBUTTON, 2, 10, 2, 10);
	SetData(_keybordSprData, VK_MBUTTON, 4, 10, 4, 10);
	SetData(_keybordSprData, VK_RBUTTON, 3, 10, 3, 10);
	SetData(_keybordSprData, VK_XBUTTON1, 1, 11, 1, 11);
	SetData(_keybordSprData, VK_XBUTTON2, 0, 11, 0, 11);

	SetData(_keybordSprData, MOUSE_OLD_WHEEL, 5, 10, 6, 10);
}

// 描画
void InputUI::Render(const RenderContext& rc, TextureRenderer& renderer)
{
	int currentInputDevice = GetDeviceIndex(Input::Instance().GetCurrentInputDevice());
	auto& inputActionMap = Input::Instance().GetButtonActionMap();
    auto& valueActionMap = Input::Instance().GetValueActionMap();
	// 描画スプライト、スプライトデータ選択
	Sprite* spr = nullptr;
	std::unordered_map<int, SprData>* sprData = nullptr;
	switch (currentInputDevice)
	{
	case 0:
		spr = &_keybordSprite;
		sprData = &_keybordSprData;
		break;
	case 1:
		spr = &_gamePadSprite;
		sprData = &_gamePadSprData;
		break;
	default: 
		return;
	}

	// 描画処理
	auto Draw = [&](const std::vector<DrawInfo>& drawInfos, const Input::InputActionMap& actionMap)
		{
			for (const auto& drawInfo : drawInfos)
			{
				// アクション名が存在しなければスキップ
				if (actionMap.find(drawInfo.actionName) == actionMap.end())
					return;
				for (const auto& inputMapInfo : actionMap.at(drawInfo.actionName))
				{
					if (GetDeviceIndex(inputMapInfo.type) == currentInputDevice)
					{
						// ボタンIDが存在しなければスキップ
						if (sprData->find(inputMapInfo.buttonID) == sprData->end())
							continue;

						spr->GetRectTransform().SetLocalPosition(drawInfo.position);
						spr->GetRectTransform().SetLocalScale(drawInfo.scale);
						if (drawInfo.isActive)
							spr->SetTexPos(sprData->at(inputMapInfo.buttonID).activateTexPos);
						else
							spr->SetTexPos(sprData->at(inputMapInfo.buttonID).texPos);
						spr->SetTexSize(sprData->at(inputMapInfo.buttonID).texSize);
						spr->SetCenterAlignment(spr->GetCenterAlignment());
						spr->UpdateTransform();
						spr->SetColor(drawInfo.color);
						spr->Render(rc, renderer);
						break;
					}
				}
			}
        };
    Draw(_drawInfos, inputActionMap);
    Draw(_drawValueInfos, valueActionMap);

	_drawInfos.clear();
	_drawValueInfos.clear();
}

// GUI描画
void InputUI::DrawGui()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu(u8"デバッグ"))
		{
			ImGui::Checkbox(u8"入力UI", &_showGui);
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	if (_showGui)
	{
		if (ImGui::Begin(u8"入力UI設定", &_showGui))
		{
			_gamePadSprite.DrawGui();
		}

		//if (ImGui::Button("add"))
		//{
		//	// ダイアログを開く
		//	std::string filepath;
		//	std::string currentDirectory;
		//	Debug::Dialog::DialogResult result = Debug::Dialog::OpenFileName(filepath, currentDirectory, ImGui::TextureFilter);
		//	// ファイルを選択したら
		//	if (result == Debug::Dialog::DialogResult::Yes || result == Debug::Dialog::DialogResult::OK)
		//	{
		//		ToWString(filepath);
		//		g_inputUICanvas->Load(
		//			Graphics::Instance().GetDevice(),
		//			Graphics::Instance().GetDeviceContext(),
		//			ToWString(filepath).c_str(),
		//			{ 1,1 });
		//	}
		//}
		//g_inputUICanvas->DrawGui(Graphics::Instance().GetDevice(),
		//	Graphics::Instance().GetDeviceContext());
		//	_keybordSprite.DrawGui();

		ImGui::End();
	}
}

// 描画登録
void InputUI::Draw(
	const std::string& actionName,
	bool isActive, 
	const Vector2& position,
	const Vector2& scale,
	const Vector4& color)
{
	DrawInfo info;
	info.actionName = actionName;
	info.isActive = isActive;
	info.position = position;
	info.scale = scale;
	info.color = color;

	int currentInputDevice = GetDeviceIndex(Input::Instance().GetCurrentInputDevice());
	auto& inputActionMap = Input::Instance().GetButtonActionMap();
	auto& valueActionMap = Input::Instance().GetValueActionMap();
    // 入力値がどちらに存在するか確認
	if (inputActionMap.find(actionName) != inputActionMap.end())
	{
		for (const auto& inputMapInfo : inputActionMap.at(actionName))
		{
			if (GetDeviceIndex(inputMapInfo.type) == currentInputDevice)
			{
				_drawInfos.push_back(info);
				return;
			}
        }
	}
	if (valueActionMap.find(actionName) != valueActionMap.end())
	{
		for (const auto& inputMapInfo : valueActionMap.at(actionName))
		{
			if (GetDeviceIndex(inputMapInfo.type) == currentInputDevice)
			{
				_drawValueInfos.push_back(info);
				return;
			}
        }
	}
}

// 描画登録
void InputUI::DrawValue(
	const std::string& actionName,
	float value, 
	const Vector2& position,
	const Vector2& scale,
	const Vector4& color)
{
	Draw(
		actionName,
		value > 0.0f,
		position,
		scale,
        color);
}
