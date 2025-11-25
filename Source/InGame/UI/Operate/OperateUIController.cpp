#include "OperateUIController.h"

#include <imgui.h>

// 開始処理
void OperateUIController::Start()
{
}

// 更新処理
void OperateUIController::Update(float elapsedTime)
{
	auto currentInputDevice = Input::Instance().GetCurrentInputDevice();

	// UI表示
	Vector2 position = _descPosition;
	for (auto& [description, info] : _descriptionMap)
	{
		Vector4 color = Vector4::White;

		// 生存処理
		if (!info.isActive)
		{
			info.lifeTime -= elapsedTime;
			color.w = info.lifeTime / 0.2f;
			if (info.lifeTime <= 0.0f)
			{
				// 削除
				continue;
			}
		}
		info.isActive = false;

		// 説明文描画
		std::string desc = description;
		GetActor()->GetScene()->GetTextRenderer().Draw(
			FontType::MSGothic,
			desc.c_str(),
			position,
			color,
			0.0f,
			_descOrigin,
			_descScale);

		Vector2 inputUIPos = position;
		std::vector<int>* keyList = nullptr;
		switch (currentInputDevice)
		{
		case Input::InputType::Keyboard:
		case Input::InputType::Mouse:
			keyList = &info.keyboardKeys;
			break;
		case Input::InputType::XboxPad:
			keyList = &info.gamePadKeys;
			break;
		case Input::InputType::DirectPad:
			break;
		}

		// キーが存在しなければスキップ
		if (keyList == nullptr || keyList->size() == 0)
		{
			position.y += _descInterval;
			continue;
		}

		for (const auto& key : *keyList)
		{
			// 入力UI描画
			InputUI::DrawInfo drawInfo;
			switch (currentInputDevice)
			{
			case Input::InputType::Keyboard:
			case Input::InputType::Mouse:
				drawInfo.keyboardKey = key;
				break;
			case Input::InputType::XboxPad:
				drawInfo.gamePadKey = key;
				break;
			case Input::InputType::DirectPad:
				break;
			}
			drawInfo.position = inputUIPos + _inputUIOffset;
			drawInfo.scale = _inputUIScale;
			drawInfo.color = color;
			inputUIPos.x += _inputUIInterval;
			GetActor()->GetScene()->GetInputUI()->Draw(drawInfo);
			if (&key != &keyList->back())
			{
				GetActor()->GetScene()->GetTextRenderer().Draw(
					FontType::MSGothic,
					"or",
					inputUIPos,
					color,
					0.0f,
					Vector2::Zero,
					_descScale);
				inputUIPos.x += _orInterval;
			}
		}

		position.y += _descInterval;
	}
}

// GUI描画
void OperateUIController::DrawGui()
{
	ImGui::DragFloat2(u8"説明文位置", &_descPosition.x, 1.0f);
	ImGui::DragFloat2(u8"説明文スケール", &_descScale.x, 0.01f);
	ImGui::DragFloat2(u8"説明文原点", &_descOrigin.x, 0.01f);
	ImGui::DragFloat(u8"説明文間隔", &_descInterval, 1.0f);
	ImGui::DragFloat(u8"入力UI間隔", &_inputUIInterval, 1.0f);
	ImGui::DragFloat(u8"or間隔", &_orInterval, 1.0f);

	if (ImGui::Button(u8"追加"))
	{
		std::string description = "テスト";
		description += std::to_string(std::rand());
		std::vector<std::string> inputActionNames;
		inputActionNames.push_back("Action1");
		inputActionNames.push_back("Action2");
		AddDescription(description, inputActionNames);
	}
	ImGui::DragFloat2(u8"入力UIオフセット", &_inputUIOffset.x, 1.0f);
	ImGui::DragFloat2(u8"入力UIスケール", &_inputUIScale.x, 0.01f);
}

void OperateUIController::AddDescription(
	const std::string& description,
	const std::vector<std::string>& inputActionNames)
{
	if (_descriptionMap.find(description) != _descriptionMap.end())
	{
		// 既に存在する場合は更新
		_descriptionMap[description].isActive = true;
		_descriptionMap[description].lifeTime = 0.2f;
		return;
	}

	DescriptionData info;
	// inputActionNamesからキーコードを取得
	auto& bottonMap = Input::Instance().GetButtonActionMap();
	auto& valueMap	= Input::Instance().GetValueActionMap();
	for (const auto& inputAction : inputActionNames)
	{
		// アクション名が存在する場合
		if (bottonMap.find(inputAction) != bottonMap.end())
		{
			for (auto& actionName : bottonMap.at(inputAction))
			{
				switch (actionName.type)
				{
				case Input::InputType::Keyboard:
				case Input::InputType::Mouse:
					info.keyboardKeys.push_back(actionName.buttonID);
					break;
				case Input::InputType::XboxPad:
					info.gamePadKeys.push_back(actionName.buttonID);
					break;
				default:
					break;
				}
			}
		}

		if (valueMap.find(inputAction) != valueMap.end())
		{
			for (auto& actionName : valueMap.at(inputAction))
			{
				switch (actionName.type)
				{
				case Input::InputType::Keyboard:
				case Input::InputType::Mouse:
					info.keyboardKeys.push_back(actionName.buttonID);
					break;
				case Input::InputType::XboxPad:
					info.gamePadKeys.push_back(actionName.buttonID);
					break;
				default:
					break;
				}
			}
		}
	}
	_descriptionMap[description] = info;
}
