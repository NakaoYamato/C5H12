#include "RectTransform.h"

#include <imgui.h>

/// 更新
void RectTransform::UpdateTransform(const RectTransform* parent)
{
	if (parent != nullptr)
	{
		float sin = sinf(parent->_worldAngle);
		float cos = cosf(parent->_worldAngle);
		_worldPosition.x = parent->_worldPosition.x + _localPosition.x * cos - _localPosition.y * sin;
		_worldPosition.y = parent->_worldPosition.y + _localPosition.x * sin + _localPosition.y * cos;
		_worldScale.x = parent->_worldScale.x * _localScale.x;
		_worldScale.y = parent->_worldScale.y * _localScale.y;
		_worldAngle = parent->_worldAngle + _localAngle;
	}
	else
	{
		_worldPosition = _localPosition;
		_worldScale = _localScale;
		_worldAngle = _localAngle;
	}
}

/// デバッグGUI表示
void RectTransform::DrawGui()
{
	ImGui::DragFloat2(u8"位置",			&_localPosition.x, 0.1f, -1000.0f, 1000.0f);
	ImGui::DragFloat2(u8"スケール",		&_localScale.x, 0.01f, 0.01f, 1000.0f);
	float angleDegree = DirectX::XMConvertToDegrees(_localAngle);
	ImGui::DragFloat(u8"角度",				&angleDegree, 1.0f);
	_localAngle = DirectX::XMConvertToRadians(angleDegree);
	ImGui::Separator();

	ImGui::DragFloat2(u8"ワールド位置", &_worldPosition.x, 0.1f, -1000.0f, 1000.0f);
	ImGui::DragFloat2(u8"ワールドスケール", &_worldScale.x, 0.01f, 0.01f, 1000.0f);
	angleDegree = DirectX::XMConvertToDegrees(_worldAngle);
	ImGui::DragFloat(u8"ワールド角度", &angleDegree, 1.0f);
}

/// 値をリセット
void RectTransform::Reset()
{
	_localPosition = Vector2::Zero;
	_localScale = Vector2::One;
	_localAngle = 0.0f;
}
