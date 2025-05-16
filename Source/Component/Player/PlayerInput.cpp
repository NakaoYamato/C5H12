#include "PlayerInput.h"

#include "../../Library/Input/Input.h"
#include "../../Library/Scene/Scene.h"

#include <imgui.h>

// 開始処理
void PlayerInput::Start()
{
}

// 更新処理
void PlayerInput::Update(float elapsedTime)
{
	// 入力処理
	{
		_lAxisValue = Vector2(_INPUT_IS_AXIS("AxisLX"), _INPUT_IS_AXIS("AxisLY"));

		// カメラの正面方向と右方向を取得
		Vector3 frontVec = GetActor()->GetScene()->GetMainCamera()->GetFront();
		Vector3 rightVec = GetActor()->GetScene()->GetMainCamera()->GetRight();

		// Y軸の成分を0にして正規化
		frontVec.y = 0.0f;
		rightVec.y = 0.0f;
		frontVec = Vector3::Normalize(frontVec);
		rightVec = Vector3::Normalize(rightVec);

		// 移動ベクトルを計算
		_movement = {};
		_movement.x = frontVec.x * _lAxisValue.y + rightVec.x * _lAxisValue.x;
		_movement.z = frontVec.z * _lAxisValue.y + rightVec.z * _lAxisValue.x;
		_movement = Vector3::Normalize(_movement);
	}

	auto InputPressed = [&](std::string action, Inputs data)
		{
			if (_INPUT_IS_PRESSED(action))
				_inputFlag |= data;
			else
				_inputFlag &= ~data;
		};
	auto InputTriggerd = [&](std::string action, Inputs data)
		{
			if (_INPUT_IS_TRIGGERD(action))
				_inputFlag |= data;
			else
				_inputFlag &= ~data;
		};

	InputPressed("Dash", Inputs::Dash);

	InputTriggerd("Attack1", Inputs::Attack);
	InputTriggerd("Evade", Inputs::Evade);
}

// GUI描画
void PlayerInput::DrawGui()
{
	ImGui::DragFloat3("Movement", &_movement.x, 0.01f, -1.0f, 1.0f);
	ImGui::Text("InputFlag: %d", _inputFlag);
}
