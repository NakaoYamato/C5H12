#include "QuestBoardUIActor.h"

// 生成時処理
void QuestBoardUIActor::OnCreate()
{
	MenuUIActor::OnCreate();

	GetRectTransform().SetLocalPosition(Vector2(200.0f, 200.0f));

	// コールバック登録
	RegisterOptionSelectedCallback("PopPage", [this](MenuUIActor* owner) -> void
		{
			owner->PopPage();
		});

	SetFilePath("./Data/Resource/Actor/QuestBoardUIActor/MenuUI.json");
	LoadFromFile();
}
