#include "WeaponActor.h"

#include "../../Library/Scene/Scene.h"
#include "../../Library/DebugSupporter/DebugSupporter.h"

#include <imgui.h>

// 生成時処理
void WeaponActor::OnCreate()
{
	_locusRenderer = this->AddComponent<LocusRenderer>();
}
// 開始時処理
void WeaponActor::OnStart()
{
	// 親のモデルコライダーを取得
	_ownerModelCollider = GetParent()->GetCollider<ModelCollider>();
}
// 遅延更新時処理
void WeaponActor::OnLateUpdate(float elapsedTime)
{
	Vector3 velocity = (_transform.GetPosition() - _oldPosition);
	_oldPosition = _transform.GetPosition();


	auto locusRenderer = _locusRenderer.lock();
	if (locusRenderer)
	{
		auto ownerModelCollider = _ownerModelCollider.lock();
		if (ownerModelCollider && ownerModelCollider->IsCollAttackEvent())
		{
			_locusPushTimer += elapsedTime;
			// 生成判定
			if (_locusPushTimer >= _locusPushInterval)
			{
				_locusPushTimer -= _locusPushInterval; // タイマーをリセット
				locusRenderer->PushFrontVertex(
					_locusRootLocalPosition.TransformCoord(_transform.GetMatrix()),
					_locusTipLocalPosition.TransformCoord(_transform.GetMatrix()),
					velocity,
					_locusLifeTime);
			}
		}
		else
		{
			locusRenderer->ResetTrialPos();
			_locusPushTimer = 0.0f; // タイマーをリセット
		}
	}

	if (_isDrawingLocusPosition)
	{
		// 軌跡の位置を描画
		Debug::Renderer::DrawSphere(
			_locusRootLocalPosition.TransformCoord(_transform.GetMatrix()),
			0.1f, Vector4::Red);
		Debug::Renderer::DrawSphere(
			_locusTipLocalPosition.TransformCoord(_transform.GetMatrix()),
			0.1f, Vector4::Green);
	}
}

// GUI描画時処理
void WeaponActor::OnDrawGui()
{
	static ImGuiTabBarFlags tab_bar_flags =
		ImGuiTabBarFlags_AutoSelectNewTabs |
		ImGuiTabBarFlags_Reorderable |
		ImGuiTabBarFlags_FittingPolicyResizeDown;
	if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
	{
		if (ImGui::BeginTabItem(u8"武器"))
		{
			ImGui::Checkbox(u8"軌跡の位置を描画", &_isDrawingLocusPosition);
			ImGui::DragFloat3(u8"軌跡のルート位置", &_locusRootLocalPosition.x, 0.01f);
			ImGui::DragFloat3(u8"軌跡の先端位置", &_locusTipLocalPosition.x, 0.01f);
            ImGui::DragFloat(u8"軌跡のライフタイム", &_locusLifeTime, 0.01f, 0.01f, 10.0f);

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
}
