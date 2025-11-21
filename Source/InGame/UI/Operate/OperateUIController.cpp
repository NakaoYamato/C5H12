#include "OperateUIController.h"

#include "../../Library/Scene/Scene.h"

#include <imgui.h>

// 開始処理
void OperateUIController::Start()
{
}

// 更新処理
void OperateUIController::Update(float elapsedTime)
{
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
		for (int i = 0;;)
		{
			const std::string& inputActionName = info.inputActionNames[i];

			// 入力UI描画
			GetActor()->GetScene()->GetInputUI()->Draw(
				inputActionName,
				_INPUT_PRESSED(inputActionName),
				inputUIPos + _inputUIOffset,
				_inputUIScale,
				color);
			inputUIPos.x += _inputUIInterval;

			if (++i < static_cast<int>(info.inputActionNames.size()))
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
			else
			{
				break;
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

void OperateUIController::AddDescription(const std::string& description, const std::vector<std::string>& inputActionNames)
{
	if (_descriptionMap.find(description) != _descriptionMap.end())
	{
		// 既に存在する場合は更新
		_descriptionMap[description].isActive = true;
		return;
	}

	DescriptionData info;
	info.inputActionNames = inputActionNames;
	_descriptionMap[description] = info;
}
