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
		if (_reflectParentScale)
		{
			_worldScale.x = parent->_worldScale.x * _localScale.x;
			_worldScale.y = parent->_worldScale.y * _localScale.y;
		}
		else
		{
			_worldScale = _localScale;
		}
		if (_reflectParentAngle)
		{
			_worldAngle = parent->_worldAngle + _localAngle;
		}
		else
		{
			_worldAngle = parent->_worldAngle + _localAngle;
		}
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
	ImGui::DragFloat2(u8"位置",			&_localPosition.x, 0.1f);
	ImGui::DragFloat2(u8"スケール",		&_localScale.x, 0.01f);
	float angleDegree = DirectX::XMConvertToDegrees(_localAngle);
	ImGui::DragFloat(u8"角度",				&angleDegree, 0.1f);
	_localAngle = DirectX::XMConvertToRadians(angleDegree);
	ImGui::Separator();
	ImGui::Checkbox(u8"親のスケールを反映", &_reflectParentScale);
	ImGui::Checkbox(u8"親の回転を反映", &_reflectParentAngle);
	if (ImGui::Button(u8"再計算"))
	{
		UpdateTransform();
	}

	if (ImGui::TreeNode(u8"ワールド情報"))
	{
		ImGui::Text(u8"ワールド位置: (%.2f, %.2f)", _worldPosition.x, _worldPosition.y);
		ImGui::Text(u8"ワールドスケール: (%.2f, %.2f)", _worldScale.x, _worldScale.y);
		ImGui::Text(u8"ワールド角度: %.2f", DirectX::XMConvertToDegrees(_worldAngle));
		angleDegree = DirectX::XMConvertToDegrees(_worldAngle);
		ImGui::TreePop();
	}
}

/// 値をリセット
void RectTransform::Reset()
{
	_localPosition = Vector2::Zero;
	_localScale = Vector2::One;
	_localAngle = 0.0f;
}
