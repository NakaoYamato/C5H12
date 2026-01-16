#include "SphereCollider.h"

#include <imgui.h>

#include "../../Scene/Scene.h"

void SphereCollider::Start()
{
	// コライダーの登録
	GetActor()->GetScene()->GetCollisionManager().RegisterSphereCollider(this);
}

// デバッグ描画処理
void SphereCollider::DebugRender(const RenderContext& rc)
{
	// 有効でなければ描画しない
	if (!IsActive())
		return;

	Debug::Renderer::DrawSphere(
		GetPosition().TransformCoord(GetActor()->GetTransform().GetMatrix()),
		GetRadius(),
		IsTrigger() ? Vector4::Blue : Vector4::Green);
}

// 削除時処理
void SphereCollider::OnDelete()
{
	// コライダーの削除
	GetActor()->GetScene()->GetCollisionManager().UnregisterSphereCollider(this);
}

// GUI描画
void SphereCollider::DrawGui()
{
	ColliderBase::DrawGui();
	ImGui::DragFloat3(u8"中心座標", &_center.x, 0.01f);
	ImGui::DragFloat(u8"半径", &_radius, 0.01f, 0.0f, 100.0f);
}

#pragma region 入出力
// ファイル読み込み
void SphereCollider::OnLoad(nlohmann::json* json)
{
	// 各データ読み込み
	_center = json->value("center", Vector3(0.0f, 0.0f, 0.0f));
	_radius = json->value("radius", 1.0f);
}

// ファイル保存
void SphereCollider::OnSave(nlohmann::json* json)
{
	(*json)["center"]		= _center;
	(*json)["radius"]		= _radius;
}
#pragma endregion
