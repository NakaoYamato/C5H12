#include "Actor.h"

#include <imgui.h>

#include "ActorManager.h"

#include "../../External/nameof/include/nameof.hpp"
#include "../../External/magic_enum/include/magic_enum/magic_enum.hpp"

#include "../Component/Component.h"
#include "../Component/Collider/ColliderComponent.h"

/// <summary>
/// enumをstringに変換
/// </summary>
/// <param name="index"></param>
/// <returns>string</returns>
#define GET_ENUM_NAME(index) nameof::nameof_enum(magic_enum::enum_value<ActorTag>(index)).data()

// 開始処理
void Actor::Start()
{
	// 各コンポーネントのスタート処理
	for (std::shared_ptr<Component>& component : components_)
	{
		component->Start();
	}
	for (std::shared_ptr<ColliderComponent>& collider : colliderComponents_)
	{
		collider->Start();
	}
}

// 更新処理
void Actor::Update(float elapsedTime)
{
	// 起動チェック
	if (!isActive_)return;

	// 各コンポーネントの更新処理
	for (std::shared_ptr<Component>& component : components_)
	{
		component->Update(elapsedTime);
	}
	for (std::shared_ptr<ColliderComponent>& collider : colliderComponents_)
	{
		collider->Update(elapsedTime);
	}

	const DirectX::XMFLOAT4X4* ParentMatrix = nullptr;
	// 親がいるときの処理
	// トランスフォーム更新
	if (!parent_.expired())
	{
		// 親の起動チェック
		if (!parent_.lock()->IsActive())
			this->isActive_ = false;

		ParentMatrix = &parent_.lock()->GetTransform().GetMatrix();
	}
	// トランスフォーム更新
	transform_.UpdateTransform(ParentMatrix);
}

// 1秒ごとの更新処理
void Actor::FixedUpdate()
{
	// 起動チェック
	if (!isActive_)return;

	// 各コンポーネントの1秒ごとの更新処理
	for (std::shared_ptr<Component>& component : components_)
	{
		component->FixedUpdate();
	}
	for (std::shared_ptr<ColliderComponent>& collider : colliderComponents_)
	{
		collider->FixedUpdate();
	}
}

// 描画の前処理
void Actor::RenderPreprocess(RenderContext& rc)
{
	// 起動チェック
	if (!isActive_)return;

	// 各コンポーネントの描画処理
	for (std::shared_ptr<Component>& component : components_)
	{
		component->RenderPreprocess(rc);
	}
	for (std::shared_ptr<ColliderComponent>& collider : colliderComponents_)
	{
		collider->RenderPreprocess(rc);
	}
}

// 描画処理
void Actor::Render(const RenderContext& rc)
{
	// 起動チェック
	if (!isActive_)return;
	if (!isShowing_)return;

	// 各コンポーネントの描画処理
	for (std::shared_ptr<Component>& component : components_)
	{
		component->Render(rc);
	}
	for (std::shared_ptr<ColliderComponent>& collider : colliderComponents_)
	{
		collider->Render(rc);
	}
}

// デバッグ表示
void Actor::DebugRender(const RenderContext& rc)
{
	// 起動チェック
	if (!isActive_)return;
	if (!drawDebug_)return;

	// 各コンポーネントの描画処理
	for (std::shared_ptr<Component>& component : components_)
	{
		component->DebugRender(rc);
	}
	for (std::shared_ptr<ColliderComponent>& collider : colliderComponents_)
	{
		collider->DebugRender(rc);
	}
}

// 影描画
void Actor::CastShadow(const RenderContext& rc)
{
	// 起動チェック
	if (!isActive_)return;

	for (std::shared_ptr<Component>& component : components_)
	{
		component->CastShadow(rc);
	}
}

// 3D描画後の描画処理
void Actor::DelayedRender(const RenderContext& rc)
{
	// 起動チェック
	if (!isActive_)return;

	for (std::shared_ptr<Component>& component : components_)
	{
		component->DelayedRender(rc);
	}
}

// Gui描画
void Actor::DrawGui()
{
	if (ImGui::CollapsingHeader("Flags", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox(u8"Active", &isActive_);
		ImGui::Checkbox(u8"Show", &isShowing_);
		ImGui::Checkbox(u8"DrawDebug", &drawDebug_);
	}

	// トランスフォーム
	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		transform_.DrawGui();
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
			for (std::shared_ptr<Component>& component : components_)
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
			for (auto& [tag, flag] : judgeTags_)
			{
				ImGui::Checkbox(nameof::nameof_enum(tag).data(), &flag);
			}
			ImGui::Separator();

			int index = 0;
			for (std::shared_ptr<ColliderComponent>& collider : colliderComponents_)
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
}

// 当たり判定処理
void Actor::Judge(Actor* other)
{
	// 起動チェック
	if (!isActive_)return;

	Vector3 hitPosition{};
	Vector3 hitNormal{};
	float penetration{};
	// 当たり判定コンポーネントの検索
	for (std::shared_ptr<ColliderComponent>& collider : colliderComponents_)
	{
		// 接触対象の当たり判定コンポーネントの検索
		const size_t otherComponentSize = other->GetColliderComponentSize();
		for (size_t i = 0; i < otherComponentSize; ++i)
		{
			std::shared_ptr<ColliderComponent> otherComponent = other->GetCollider(i);

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
	for (std::shared_ptr<Component>& component : components_)
	{
		component->OnCollision(other, hitPosition, hitNormal, penetration);
	}
	for (std::shared_ptr<ColliderComponent>& collider : colliderComponents_)
	{
		collider->OnCollision(other, hitPosition, hitNormal, penetration);
	}
}
