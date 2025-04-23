#include "Actor.h"

#include <imgui.h>

#include "ActorManager.h"

#include "../../External/nameof/include/nameof.hpp"
#include "../../External/magic_enum/include/magic_enum/magic_enum.hpp"

#include "../Component/Component.h"
#include "../Component/Collider/ColliderBaseComponent.h"

#include "../Camera/Camera.h"
#include "../../Library/DebugSupporter/DebugSupporter.h"
#include <ImGuizmo.h>

// 開始処理
void Actor::Start()
{
	// 各コンポーネントのスタート処理
	for (std::shared_ptr<Component>& component : _components)
	{
		component->Start();
	}
	for (std::shared_ptr<ColliderBaseComponent>& collider : _colliders)
	{
		collider->Start();
	}
}

// 更新処理
void Actor::Update(float elapsedTime)
{
	// 起動チェック
	if (!_isActive)return;

	// 各コンポーネントの更新処理
	for (std::shared_ptr<Component>& component : _components)
	{
		component->Update(elapsedTime);
	}
	for (std::shared_ptr<ColliderBaseComponent>& collider : _colliders)
	{
		collider->Update(elapsedTime);
	}

	// トランスフォーム更新
	UpdateTransform();
}

// 1秒ごとの更新処理
void Actor::FixedUpdate()
{
	// 起動チェック
	if (!_isActive)return;

	// 各コンポーネントの1秒ごとの更新処理
	for (std::shared_ptr<Component>& component : _components)
	{
		component->FixedUpdate();
	}
	for (std::shared_ptr<ColliderBaseComponent>& collider : _colliders)
	{
		collider->FixedUpdate();
	}
}

// 描画処理
void Actor::Render(const RenderContext& rc)
{
	// 起動チェック
	if (!_isActive)return;
	if (!_isShowing)return;

	// 各コンポーネントの描画処理
	for (std::shared_ptr<Component>& component : _components)
	{
		component->Render(rc);
	}
	for (std::shared_ptr<ColliderBaseComponent>& collider : _colliders)
	{
		collider->Render(rc);
	}
}

// デバッグ表示
void Actor::DebugRender(const RenderContext& rc)
{
	// 起動チェック
	if (!_isActive)return;
	if (!_drawDebug)return;

	// 各コンポーネントの描画処理
	for (std::shared_ptr<Component>& component : _components)
	{
		component->DebugRender(rc);
	}
	for (std::shared_ptr<ColliderBaseComponent>& collider : _colliders)
	{
		collider->DebugRender(rc);
	}
}

// 影描画
void Actor::CastShadow(const RenderContext& rc)
{
	// 起動チェック
	if (!_isActive)return;

	for (std::shared_ptr<Component>& component : _components)
	{
		component->CastShadow(rc);
	}
}

// 3D描画後の描画処理
void Actor::DelayedRender(const RenderContext& rc)
{
	// 起動チェック
	if (!_isActive)return;

	for (std::shared_ptr<Component>& component : _components)
	{
		component->DelayedRender(rc);
	}
}

// Gui描画
void Actor::DrawGui()
{
	if (ImGui::CollapsingHeader("Flags"))
	{
		ImGui::Checkbox(u8"Active", &_isActive);
		ImGui::Checkbox(u8"Show", &_isShowing);
		ImGui::Checkbox(u8"DrawDebug", &_drawDebug);
		ImGui::Checkbox(u8"UseGuizmo", &_useGuizmo);
	}

	// トランスフォーム
	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		_transform.DrawGui();
	}

	static ImGuiTabBarFlags tab_bar_flags =
		ImGuiTabBarFlags_AutoSelectNewTabs |
		ImGuiTabBarFlags_Reorderable |
		ImGuiTabBarFlags_FittingPolicyResizeDown;
	if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
	{
		if (ImGui::BeginTabItem(u8"コンポーネント"))
		{
			// 各コンポーネントのGUI
			for (std::shared_ptr<Component>& component : _components)
			{
				ImGui::Spacing();
				ImGui::Separator();

				if (ImGui::CollapsingHeader(component->GetName(), ImGuiTreeNodeFlags_DefaultOpen))
				{
					component->DrawGui();
				}
			}
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem(u8"コライダー"))
		{
			ImGui::Separator();
			ImGui::Text(u8"当たり判定を除く対象");
			for (auto& [tag, flag] : _judgeTags)
			{
				ImGui::Checkbox(nameof::nameof_enum(tag).data(), &flag);
			}
			ImGui::Separator();

			int index = 0;
			for (std::shared_ptr<ColliderBaseComponent>& collider : _colliders)
			{
				ImGui::Spacing();
				ImGui::Separator();
				if (ImGui::TreeNodeEx(&collider, ImGuiTreeNodeFlags_DefaultOpen, std::to_string(index++).c_str()))
				{
					ImGui::Separator();
					ImGui::Text(collider->GetName());
					collider->DrawGui();
					ImGui::Separator();
					ImGui::TreePop();
				}
			}
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	if (_useGuizmo)
	{
		DirectX::XMFLOAT4X4 transform = _transform.GetMatrix();
		if (Debug::Guizmo(Camera::Instance().GetView(), Camera::Instance().GetProjection(),
			&transform))
		{
			DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(&transform);
			DirectX::XMMATRIX C{ DirectX::XMLoadFloat4x4(&COORDINATE_SYSTEM_TRANSFORMS[_transform.GetCoordinateType()]) *
				DirectX::XMMatrixScaling(_transform.GetLengthScale(), _transform.GetLengthScale(),_transform.GetLengthScale()) };
			M = DirectX::XMMatrixInverse(nullptr, C) * M;
			DirectX::XMVECTOR S, R, T;
			DirectX::XMMatrixDecompose(&S, &R, &T, M);
			Vector3 s, r, t;
			DirectX::XMStoreFloat3(&s, S);
			DirectX::XMStoreFloat3(&t, T);
			r = QuaternionToRollPitchYaw(R);
			_transform.SetPosition(t);
			_transform.SetScale(s);
			_transform.SetAngle(r);
		}
	}
}

// 当たり判定処理
void Actor::Judge(Actor* other)
{
	// 起動チェック
	if (!_isActive)return;

	Vector3 hitPosition{};
	Vector3 hitNormal{};
	float penetration{};
	// 当たり判定コンポーネントの検索
	for (std::shared_ptr<ColliderBaseComponent>& collider : _colliders)
	{
		// 接触対象の当たり判定コンポーネントの検索
		const size_t otherComponentSize = other->GetColliderComponentSize();
		for (size_t i = 0; i < otherComponentSize; ++i)
		{
			std::shared_ptr<ColliderBaseComponent> otherComponent = other->GetCollider(i);

			// 当たり判定処理
			const bool result = collider->Judge(other, otherComponent.get(),
				hitPosition, hitNormal, penetration);
			if (result)
			{
				// 各コンポーネントの接触処理
				this->OnCollision(other, hitPosition, hitNormal, penetration);
				other->OnCollision(this, hitPosition, -hitNormal, penetration);
			}
		}
	}
}

// 接触時の処理
void Actor::OnCollision(Actor* other, const Vector3& hitPosition, const Vector3& hitNormal, const float& penetration)
{
	// 各コンポーネントの接触処理
	for (std::shared_ptr<Component>& component : _components)
	{
		component->OnCollision(other, hitPosition, hitNormal, penetration);
	}
	for (std::shared_ptr<ColliderBaseComponent>& collider : _colliders)
	{
		collider->OnCollision(other, hitPosition, hitNormal, penetration);
	}
}

// トランスフォーム更新
void Actor::UpdateTransform()
{
	_transform.UpdateTransform(nullptr);
}
