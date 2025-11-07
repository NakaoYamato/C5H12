#include "InGameCanvasActor.h"

#include "../../Library/Graphics/Graphics.h"

#include "../../Source/InGame/UI/TimerUIController.h"
#include "../../Source/InGame/UI/ChestUIController.h"

// 生成時処理
void InGameCanvasActor::OnCreate()
{
    // タイマー生成
    auto timerUIActor = GetScene()->RegisterActor<UIActor>("TimerUI", ActorTag::UI);
    timerUIActor->SetParent(this);
    auto timerUIController = timerUIActor->AddComponent<TimerUIController>();

	// チェスト生成
	auto chestUIActor = GetScene()->RegisterActor<UIActor>("ChestUI", ActorTag::UI);
	chestUIActor->SetParent(this);
	chestUIActor->GetRectTransform().SetLocalPosition(Vector2(1500.0f, 170.0f));
	auto chestUIController = chestUIActor->AddComponent<ChestUIController>();
}
