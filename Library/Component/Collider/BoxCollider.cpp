#include "BoxCollider.h"

#include <imgui.h>

#include "../../Scene/Scene.h"

// 開始処理
void BoxCollider::Start()
{
	// コライダーの登録
	GetActor()->GetScene()->GetCollisionManager().RegisterBoxCollider(this);
}

// デバッグ描画処理
void BoxCollider::DebugRender(const RenderContext& rc)
{
	// 有効でなければ描画しない
	if (!IsActive())
		return;

	Debug::Renderer::DrawBox(
		GetPosition().TransformCoord(GetActor()->GetTransform().GetMatrix()),
		GetActor()->GetTransform().GetWorldAngle(),
		GetHalfSize(),
		IsTrigger() ? Vector4::Blue : Vector4::Green);
}

// 削除時処理
void BoxCollider::OnDelete()
{
	// コライダーの削除
	GetActor()->GetScene()->GetCollisionManager().UnregisterBoxCollider(this);
}

// 削除時処理
void BoxCollider::DrawGui()
{
	ColliderBase::DrawGui();
	ImGui::DragFloat3(u8"中心座標", &_center.x, 0.01f);
	ImGui::DragFloat3(u8"半辺長", &_halfSize.x, 0.01f);
}

#pragma region 入出力
// ファイル読み込み
void BoxCollider::OnLoad(nlohmann::json* json)
{
	// 各データ読み込み
	_center = json->value("center", Vector3(0.0f, 0.0f, 0.0f));
	_halfSize = json->value("halfSize", Vector3(1.0f, 1.0f, 1.0f));
}

// ファイル保存
void BoxCollider::OnSave(nlohmann::json* json)
{
	(*json)["center"]		= _center;
	(*json)["halfSize"]	= _halfSize;
}
#pragma endregion
