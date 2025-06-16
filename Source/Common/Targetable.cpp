#include "Targetable.h"

#include "../../Library/Scene/Scene.h"
#include "../../Source/AI/MetaAI.h"

#include "../../External/nameof/include/nameof.hpp"
#include <imgui.h>

// 開始処理
void Targetable::Start()
{
	// メタAIを取得
	auto metaAIActor = GetActor()->GetScene()->GetActorManager().FindByName("MetaAI", ActorTag::DrawContextParameter);
	if (metaAIActor)
	{
		// メタAIに登録
		auto metaAI = metaAIActor->GetComponent<MetaAI>();
		if (metaAI)
		{
			metaAI->RegisterTargetable(shared_from_this());
		}
	}
}

// Gui描画処理
void Targetable::DrawGui()
{
	ImGui::Text(u8"陣営: %s", nameof::nameof_enum(_faction).data());
	ImGui::Text(u8"ヘイト値: %.2f", _hateValue);
	ImGui::Checkbox(u8"ターゲット可能か", &_isTargetable);
}
