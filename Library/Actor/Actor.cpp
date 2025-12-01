#include "Actor.h"

#include <imgui.h>

#include "ActorManager.h"
#include "../Graphics/Graphics.h"

#include "../Component/Component.h"
#include "../Component/Collider/ColliderBase.h"

#include "../Scene/Scene.h"

#include <ImGuizmo.h>

static bool ActorProfilerIsPause = false;

#pragma region ActorManagerで呼ぶ関数
/// 生成処理
void Actor::Create(ActorTag tag)
{
	_profiler.Initialize(&ActorProfilerIsPause, [](bool pause) {ActorProfilerIsPause = pause; }, 1);
	_profiler._threads[0].name = "Actor";

	// タグを設定
	_tag = tag;

	// 生成時処理
	OnCreate();
}
/// 削除時処理
void Actor::Deleted()
{
	_profiler.Shutdown();

	// 各コンポーネントの削除処理
	for (std::shared_ptr<Component>& component : _components)
	{
		component->OnDelete();
	}
	for (std::shared_ptr<ColliderBase>& collider : _colliders)
	{
		collider->OnDelete();
	}

	OnDeleted();

	// 親との関連解除
	if (auto parent = GetParent())
	{
		auto iter = std::find(parent->_children.begin(), parent->_children.end(), shared_from_this());
		if (iter != parent->_children.end())
		{
			parent->_children.erase(iter);
		}
	}
	// 子供の削除
	for (std::shared_ptr<Actor>& child : _children)
	{
		if (child)
			child->Deleted();
	}
}
/// 開始処理
void Actor::Start()
{
	// Transform内のmatrixを生成するために行列を更新
	UpdateTransform();

	// モデルのトランスフォーム更新
	UpdateModelTransform();

	if (_model != nullptr)
	{
		// モデルの行列を更新
		_model->UpdateTransform(_model->GetPoseNodes(), _transform.GetMatrix());
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

	OnStart();
}
///	更新処理
void Actor::Update(float elapsedTime)
{
	_profiler.NewFrame();
	ImGuiControl::ProfileScope profileScope(0, "Update", ImGuiControl::Profiler::Color::Dark, __FILE__, __LINE__, &_profiler);

	// 起動チェック
	if (!_isActive)return;

	OnPreUpdate(elapsedTime);

	// モデルのトランスフォーム更新
	UpdateModelTransform();

	// 各コンポーネントの更新処理
	for (std::shared_ptr<Component>& component : _components)
	{
		ImGuiControl::ProfileScope profileScope(0, component->GetName(), ImGuiControl::Profiler::Color::Blue, __FILE__, __LINE__, &_profiler);
		component->Update(elapsedTime);
	}
	for (std::shared_ptr<ColliderBase>& collider : _colliders)
	{
		ImGuiControl::ProfileScope profileScope(0, collider->GetName(), ImGuiControl::Profiler::Color::Green, __FILE__, __LINE__, &_profiler);
		collider->Update(elapsedTime);
	}

	OnUpdate(elapsedTime);

	// トランスフォーム更新
	UpdateTransform();
}
/// Update後更新処理
void Actor::LateUpdate(float elapsedTime)
{
	ImGuiControl::ProfileScope profileScope(0, "LateUpdate", ImGuiControl::Profiler::Color::Dark, __FILE__, __LINE__, &_profiler);

	// 起動チェック
	if (!_isActive)return;

	for (std::shared_ptr<Component>& component : _components)
	{
		ImGuiControl::ProfileScope profileScope(0, component->GetName(), ImGuiControl::Profiler::Color::Purple, __FILE__, __LINE__, &_profiler);
		component->LateUpdate(elapsedTime);
	}
	for (std::shared_ptr<ColliderBase>& collider : _colliders)
	{
		ImGuiControl::ProfileScope profileScope(0, collider->GetName(), ImGuiControl::Profiler::Color::Yellow, __FILE__, __LINE__, &_profiler);
		collider->LateUpdate(elapsedTime);
	}
	OnLateUpdate(elapsedTime);
}
/// 固定間隔更新処理
void Actor::FixedUpdate()
{
	ImGuiControl::ProfileScope profileScope(0, "FixedUpdate", ImGuiControl::Profiler::Color::Dark, __FILE__, __LINE__, &_profiler);

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

	OnFixedUpdate();
}
/// 描画処理
void Actor::Render(const RenderContext& rc)
{
	ImGuiControl::ProfileScope profileScope(0, "Render", ImGuiControl::Profiler::Color::Dark, __FILE__, __LINE__, &_profiler);

	// 起動チェック
	if (!_isActive)return;
	// 表示チェック
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

	// 描画時処理
	OnRender(rc);
}
/// デバッグ表示
void Actor::DebugRender(const RenderContext& rc)
{
	ImGuiControl::ProfileScope profileScope(0, "DebugRender", ImGuiControl::Profiler::Color::Dark, __FILE__, __LINE__, &_profiler);

	// 起動チェック
	if (!_isActive)return;
	// デバッグ表示チェック
	if (!_isDrawingDebug)return;

	// 各コンポーネントの描画処理
	for (std::shared_ptr<Component>& component : _components)
	{
		component->DebugRender(rc);
	}
	for (std::shared_ptr<ColliderBase>& collider : _colliders)
	{
		collider->DebugRender(rc);
	}

	// デバッグ表示時処理
	OnDebugRender(rc);
}
/// 影描画
void Actor::CastShadow(const RenderContext& rc)
{
	ImGuiControl::ProfileScope profileScope(0, "CastShadow", ImGuiControl::Profiler::Color::Dark, __FILE__, __LINE__, &_profiler);

	// 起動チェック
	if (!_isActive)return;

	for (std::shared_ptr<Component>& component : _components)
	{
		component->CastShadow(rc);
	}
}
/// 3D描画後の描画処理
void Actor::DelayedRender(const RenderContext& rc)
{
	ImGuiControl::ProfileScope profileScope(0, "DelayedRender", ImGuiControl::Profiler::Color::Dark, __FILE__, __LINE__, &_profiler);

	// 起動チェック
	if (!_isActive)return;

	for (std::shared_ptr<Component>& component : _components)
	{
		component->DelayedRender(rc);
	}

	// 3D描画後の描画時処理
	OnDelayedRender(rc);
}
/// Gui描画
void Actor::DrawGui()
{
	// プロファイラー表示
	if (_isOpenProfiler)
	{
		std::string title = std::string(GetName()) + " Profiler";
		if (ImGui::Begin(title.c_str()))
		{
			_profiler.DrawUI();
		}
		ImGui::End();
	}

	// 親の名前表示
	if (GetParent())
	{
		ImGui::Text(GetParentName().c_str());
		ImGui::Text((std::string(u8"└") + GetName()).c_str());
	}
	else
	{
		ImGui::Text(GetName());
	}

	// フラグ
	if (ImGui::CollapsingHeader("Flags"))
	{
		ImGui::Checkbox(u8"Active", &_isActive);
		ImGui::Checkbox(u8"Show", &_isShowing);
		ImGui::Checkbox(u8"DrawDebug", &_isDrawingDebug);
		ImGui::Checkbox(u8"UseGuizmo", &_isUsingGuizmo);
		ImGui::Checkbox(u8"InheritParentTransform", &_isInheritParentTransform);
		ImGui::Checkbox(u8"OpenProfiler", &_isOpenProfiler);
	}

	// トランスフォーム
	DrawTransformGui();

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

				ImGui::PushID(&component);

				bool isOpen = ImGui::CollapsingHeader(component->GetName(), ImGuiTreeNodeFlags_DefaultOpen);

				// 右クリックされたとき
				if (ImGui::BeginPopupContextItem())
				{
					if (ImGui::MenuItem("Save"))
					{
						component->SaveToFile();
					}

					if (ImGui::MenuItem("Load"))
					{
						component->LoadFromFile();
					}

					ImGui::EndPopup();
				}

				if (isOpen)
				{
					component->DrawGui();
				}

				ImGui::PopID();
			}
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem(u8"コライダー"))
		{
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
	if (_isUsingGuizmo)
	{
		DrawGuizmo();
	}

	OnDrawGui();
}
#pragma endregion

#pragma region 当たり判定
/// 接触時の処理
void Actor::Contact(CollisionData& collisionData)
{
	// 各コンポーネントの接触処理
	for (std::shared_ptr<Component>& component : _components)
	{
		component->OnContact(collisionData);
	}
	for (std::shared_ptr<ColliderBase>& collider : _colliders)
	{
		collider->OnContact(collisionData);
	}

	// 親にも通知
	if (GetParent())
	{
		GetParent()->Contact(collisionData);
	}

	OnContact(collisionData);
}
/// 接触した瞬間の処理
void Actor::ContactEnter(CollisionData& collisionData)
{
	// 各コンポーネントの接触処理
	for (std::shared_ptr<Component>& component : _components)
	{
		component->OnContactEnter(collisionData);
	}
	for (std::shared_ptr<ColliderBase>& collider : _colliders)
	{
		collider->OnContactEnter(collisionData);
	}

	// 親にも通知
	if (GetParent())
	{
		GetParent()->ContactEnter(collisionData);
	}

	OnContactEnter(collisionData);
}
#pragma endregion
/// 削除処理
void Actor::Remove()
{
	_isActive = false;
    _scene->GetActorManager().Remove(shared_from_this());
}
/// モデルの読み込み
std::weak_ptr<Model> Actor::LoadModel(const char* filename)
{
	if (filename == "")
	{
		_model.reset();
		return std::weak_ptr<Model>();
	}

	_model = std::make_unique<Model>(Graphics::Instance().GetDevice(), filename);
	return _model;
}
void Actor::SetIsActive(bool b)
{
	this->_isActive = b;
	ChangedActive(b);
}
#pragma region 親子関係
/// 親設定
void Actor::SetParent(Actor* parent)
{
	if (parent)
	{
		_parent = parent;
		// 親の子供に自分がいるか確認
		for (const std::shared_ptr<Actor>& child : parent->_children)
		{
			if (child.get() == this)
			{
				return;
			}
		}
		// 親の子リストに自分を追加
		parent->AddChild(shared_from_this());
	}
}
/// 子追加
void Actor::AddChild(std::shared_ptr<Actor> child)
{
	_children.emplace_back(child);
	child->SetParent(this);
}
#pragma endregion
#pragma region 仮想関数
/// モデルのトランスフォーム更新
void Actor::UpdateModelTransform()
{
	if (_model != nullptr)
	{
		// モデルの行列を更新
		_model->UpdateTransform(_model->GetPoseNodes(), _transform.GetMatrix());
	}
}
/// トランスフォーム更新
void Actor::UpdateTransform()
{
	_transform.UpdateTransform((GetParent() && _isInheritParentTransform) ? &GetParent()->GetTransform().GetMatrix() : nullptr);
}
/// トランスフォームGUI描画
void Actor::DrawTransformGui()
{
	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		_transform.DrawGui();
	}
}
/// ギズモ描画
void Actor::DrawGuizmo()
{
	DirectX::XMFLOAT4X4 transform = _transform.GetMatrix();
	if (Debug::Guizmo(GetScene()->GetMainCamera()->GetView(), GetScene()->GetMainCamera()->GetProjection(),
		&transform))
	{
		// 単位を考慮した行列から位置、回転、スケールを取得
		DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(&transform);
		DirectX::XMMATRIX InvC = DirectX::XMMatrixInverse(nullptr, DirectX::XMMatrixScaling(_transform.GetLengthScale(), _transform.GetLengthScale(),_transform.GetLengthScale()));
		DirectX::XMMATRIX InvP = DirectX::XMMatrixInverse(nullptr, GetParent() ? DirectX::XMLoadFloat4x4(&GetParent()->GetTransform().GetMatrix()) : DirectX::XMMatrixIdentity());
		M = InvC * M * InvP;
		DirectX::XMVECTOR S, R, T;
		DirectX::XMMatrixDecompose(&S, &R, &T, M);
		Vector3 s, r, t;
		DirectX::XMStoreFloat3(&s, S);
		DirectX::XMStoreFloat3(&t, T);
		r = Quaternion::ToRollPitchYaw(R);
		_transform.SetPosition(t);
		_transform.SetScale(s);
		_transform.SetAngle(r);
	}
}
/// 起動フラグが変化したときの処理
void Actor::ChangedActive(bool isActive)
{
	// 各コンポーネントの起動フラグ変化処理
	for (std::shared_ptr<Component>& component : _components)
	{
		component->OnChangedActive(isActive);
	}
	for (std::shared_ptr<ColliderBase>& collider : _colliders)
	{
		collider->OnChangedActive(isActive);
	}
	OnChangedActive(isActive);
}
#pragma endregion
