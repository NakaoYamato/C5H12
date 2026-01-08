#include "RespawnZone.h"

#include "../../Library/Scene/Scene.h"
#include "../../Source/AI/MetaAI.h"

#include <imgui.h>

// 開始処理
void RespawnZone::Start()
{
	// メタAIを取得
	auto metaAIActor = GetActor()->GetScene()->GetActorManager().FindByName("MetaAI", ActorTag::System);
	if (metaAIActor)
	{
		// メタAIに登録
		auto metaAI = metaAIActor->GetComponent<MetaAI>();
		if (metaAI)
		{
			metaAI->RegisterRespawnZone(shared_from_this());
		}
	}
}

// GUI描画
void RespawnZone::DrawGui()
{
	ImGui::DragFloat3(u8"中心位置", &_center.x, 0.1f, -1000.0f, 1000.0f);
	Vector3 deg = Vector3::ToDegrees(_angle);
	ImGui::DragFloat3(u8"向き", &deg.x, 1.0f, 0.0f, 360.0f);
	_angle = Vector3::ToRadians(deg);
	ImGui::DragFloat(u8"半径", &_radius, 0.1f, 0.0f, 1000.0f);
	ImGui::Checkbox(u8"デバッグレンダリング", &_debugRender);
}

// デバッグ表示
void RespawnZone::DebugRender(const RenderContext& rc)
{
	if (!_debugRender)
		return;
	Debug::Renderer::DrawSphere(
		GetActor()->GetTransform().GetWorldPosition() + _center,
		_radius,
		Vector4::Purple);
}
