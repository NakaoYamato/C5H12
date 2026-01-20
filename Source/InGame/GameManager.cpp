#include "GameManager.h"

#include "../../Source/AI/MetaAI.h"
#include "../../Source/Quest/QuestOrderController.h"

// 生成時処理
void GameManager::OnCreate()
{
	Actor::OnCreate();

	// コンポーネント追加
	auto metaAI = AddComponent<MetaAI>();
	auto questOrderController = AddComponent<QuestOrderController>();
}
