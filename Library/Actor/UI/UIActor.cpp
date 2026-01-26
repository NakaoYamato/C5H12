#include "UIActor.h"

#include "../../Library/Component/SpriteRenderer.h"
#include "../../Library/Scene/Scene.h"

#include <imgui.h>

/// 生成時処理
void UIActor::OnCreate()
{
	// スプライトレンダラー追加
	AddComponent<SpriteRenderer>();
}

/// トランスフォーム更新
void UIActor::UpdateTransform()
{
	_rectTransform.UpdateTransform(_parentRectTransform);
}

// 起動フラグが変化したときの処理
// 子供にも伝播する
void UIActor::OnChangedActive(bool isActive)
{
	if (!_propagateActiveChange)
		return;

	// 子供に伝播
	for (auto& child : GetChildren())
	{
		UIActor* uiChild = dynamic_cast<UIActor*>(child.get());
		if (uiChild)
		{
			uiChild->ChangedActive(isActive);
		}
	}
}

/// 3D描画後の描画時処理
void UIActor::OnDelayedRender(const RenderContext& rc)
{
	if (IsDrawingDebug() && !Debug::Input::IsActive(DebugInput::BTN_F7))
	{
		// 中心をデバッグ描画
		GetScene()->GetPrimitive()->Circle(rc.deviceContext,
			_rectTransform.GetWorldPosition(),
			5.0f);
		// 上方向をデバッグ描画
		Vector2 upDir = Vector2(0.0f, -20.0f);
		float sin = sinf(_rectTransform.GetWorldAngle());
		float cos = cosf(_rectTransform.GetWorldAngle());
		Vector2 rotatedUpDir = Vector2(
			upDir.x * cos - upDir.y * sin,
			upDir.x * sin + upDir.y * cos);
		GetScene()->GetPrimitive()->Line(rc.deviceContext,
			_rectTransform.GetWorldPosition(),
			_rectTransform.GetWorldPosition() + rotatedUpDir,
			Vector4::Green,
			2.0f);
		// 右方向をデバッグ描画
		Vector2 rightDir = Vector2(20.0f, 0.0f);
		Vector2 rotatedRightDir = Vector2(
			rightDir.x * cos - rightDir.y * sin,
			rightDir.x * sin + rightDir.y * cos);
		GetScene()->GetPrimitive()->Line(rc.deviceContext,
			_rectTransform.GetWorldPosition(),
			_rectTransform.GetWorldPosition() + rotatedRightDir,
			Vector4::Red,
			2.0f);
	}
}

/// 親設定
void UIActor::SetParent(Actor* parent)
{
	// 親がUIActorでなければ設定しない
	UIActor* uiParent = dynamic_cast<UIActor*>(parent);
	if (!uiParent)
		return;

	Actor::SetParent(parent);
	_parentRectTransform = &uiParent->_rectTransform;
}

/// トランスフォームGUI描画
void UIActor::DrawTransformGui()
{
	if (ImGui::CollapsingHeader("Flags"))
	{
		ImGui::Checkbox(u8"PropagateActiveChange", &_propagateActiveChange);
	}

	if (ImGui::CollapsingHeader("RectTransform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		_rectTransform.DrawGui();
	}
}
