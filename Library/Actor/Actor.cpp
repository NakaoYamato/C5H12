#include "Actor.h"

#include <imgui.h>

#include "ActorManager.h"
#include "../Graphics/Graphics.h"

#include "../Component/Component.h"
#include "../Component/Collider/ColliderBase.h"

#include "../Scene/Scene.h"
#include "../../Library/DebugSupporter/DebugSupporter.h"
#include "../../External/nameof/include/nameof.hpp"
#include "../../External/magic_enum/include/magic_enum/magic_enum.hpp"

#include <ImGuizmo.h>

// 開始処理
void Actor::Start()
{
	// 行列を更新
	UpdateTransform();
	if (_model != nullptr)
	{
		// モデルの行列を更新
		_model->UpdateTransform(_transform.GetMatrix());
	}

	// 各コンポーネントのスタート処理
	for (std::shared_ptr<Component>& component : _components)
	{
		component->Start();
	}
	for (std::shared_ptr<ColliderBase>& collider : _colliders)
	{
		collider->Start();
	}
}

/// 削除時処理
void Actor::OnDestroy()
{
	// 各コンポーネントの削除処理
	for (std::shared_ptr<Component>& component : _components)
	{
		component->OnDestroy();
	}
	for (std::shared_ptr<ColliderBase>& collider : _colliders)
	{
		collider->OnDestroy();
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

	// トランスフォーム更新
	UpdateTransform();

	// 当たり判定は行列に影響されるので、トランスフォーム更新後に行う
	for (std::shared_ptr<ColliderBase>& collider : _colliders)
	{
		collider->Update(elapsedTime);
	}
}

/// 一定間隔の更新処理
void Actor::FixedUpdate()
{
	// 起動チェック
	if (!_isActive)return;

	// 各コンポーネントの一定間隔の更新処理
	for (std::shared_ptr<Component>& component : _components)
	{
		component->FixedUpdate();
	}
	for (std::shared_ptr<ColliderBase>& collider : _colliders)
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
	for (std::shared_ptr<ColliderBase>& collider : _colliders)
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
	for (std::shared_ptr<ColliderBase>& collider : _colliders)
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
			for (std::shared_ptr<ColliderBase>& collider : _colliders)
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

	// ギズモ表示
	if (_useGuizmo)
	{
		DrawGuizmo();
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
	for (std::shared_ptr<ColliderBase>& collider : _colliders)
	{
		collider->OnCollision(other, hitPosition, hitNormal, penetration);
	}
}

// 削除処理
void Actor::Destroy()
{
    _scene->GetActorManager().Remove(shared_from_this());
}

/// モデルの読み込み
std::weak_ptr<Model> Actor::LoadModel(const char* filename)
{
	_model = std::make_unique<Model>(Graphics::Instance().GetDevice(), filename);
	return _model;
}

// トランスフォーム更新
void Actor::UpdateTransform()
{
	_transform.UpdateTransform(nullptr);
}

// ギズモ描画
void Actor::DrawGuizmo()
{
	DirectX::XMFLOAT4X4 transform = _transform.GetMatrix();
	if (Debug::Guizmo(GetScene()->GetMainCamera()->GetView(), GetScene()->GetMainCamera()->GetProjection(),
		&transform))
	{
		// 単位を考慮した行列から位置、回転、スケールを取得
		DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(&transform);
		DirectX::XMMATRIX C{ DirectX::XMMatrixScaling(_transform.GetLengthScale(), _transform.GetLengthScale(),_transform.GetLengthScale()) };
		M = DirectX::XMMatrixInverse(nullptr, C) * M;
		DirectX::XMVECTOR S, R, T;
		DirectX::XMMatrixDecompose(&S, &R, &T, M);
		Vector3 s, r, t;
		DirectX::XMStoreFloat3(&s, S);
		DirectX::XMStoreFloat3(&t, T);
		r = Quaternion::ToRollPitchYaw(R);
		_transform.SetPosition(t);
		_transform.SetScale(s);
		_transform.SetRotation(r);
	}
}
