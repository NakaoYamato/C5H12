#include "StageController.h"

#include "../../Library/Scene/Scene.h"
#include "../../Source/AI/MetaAI.h"

#include <imgui.h>

// 開始時処理
void StageController::Start()
{
	// メタAIを取得
	auto gameManager = GetActor()->GetScene()->GetActorManager().FindByName("GameManager", ActorTag::System);
	if (gameManager)
	{
		// メタAIに登録
		auto metaAI = gameManager->GetComponent<MetaAI>();
		if (metaAI)
		{
			metaAI->RegisterStageController(shared_from_this());
		}
	}
	// 子供に含まれるエントリーゾーン取得
	_entryZones.clear();
	for (auto& child : GetActor()->GetChildren())
	{
		if (auto entryZones = child->GetComponent<EntryZone>())
		{
			_entryZones.push_back(entryZones);
		}
	}
}

// GUI描画
void StageController::DrawGui()
{
	ImGui::DragInt(u8"エリア番号", &_areaNumber, 1, -1, 1000);
	if (ImGui::TreeNode(u8"エントリーゾーンリスト"))
	{
		for (size_t i = 0; i < _entryZones.size(); i++)
		{
			auto entryZone = _entryZones[i].lock();
			if (ImGui::TreeNode((u8"エントリーゾーン" + std::to_string(i)).c_str()))
			{
				if (entryZone)
					ImGui::Text(u8"名前: %s", entryZone->GetActor()->GetName());
				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}
}
