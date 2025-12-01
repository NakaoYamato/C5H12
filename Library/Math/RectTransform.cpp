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
	ImGui::Separator();

	ImGui::DragFloat2(u8"ワールド位置", &_worldPosition.x, 0.1f);
	ImGui::DragFloat2(u8"ワールドスケール", &_worldScale.x, 0.01f);
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

#pragma region 入出力
// ファイル読み込み
bool RectTransform::LoadFromFile(nlohmann::json_abi_v3_12_0::json& json)
{
	if (json.contains("RectTransform"))
	{
		auto& sub = json["RectTransform"];
		_localPosition.x = sub.value("localPositionX", 0.0f);
		_localPosition.y = sub.value("localPositionY", 0.0f);
		_localScale.x = sub.value("localScaleX", 1.0f);
		_localScale.y = sub.value("localScaleY", 1.0f);
		_localAngle = sub.value("localAngle", 0.0f);
		_reflectParentScale = sub.value("reflectParentScale", true);
		_reflectParentAngle = sub.value("reflectParentAngle", true);
	}

	return true;
}

// ファイル保存
bool RectTransform::SaveToFile(nlohmann::json_abi_v3_12_0::json& json)
{
	auto& sub = json["RectTransform"];
	sub["localPositionX"] = _localPosition.x;
	sub["localPositionY"] = _localPosition.y;
	sub["localScaleX"] = _localScale.x;
	sub["localScaleY"] = _localScale.y;
	sub["localAngle"] = _localAngle;
	sub["reflectParentScale"] = _reflectParentScale;
	sub["reflectParentAngle"] = _reflectParentAngle;
	return true;
}
#pragma endregion
