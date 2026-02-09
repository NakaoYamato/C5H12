#include "QuestQueueUIController.h"

#include "../../Library/Actor/UI/UIActor.h"

#include <imgui.h>

// 開始処理
void QuestQueueUIController::Start()
{
    _spriteRenderer = this->GetActor()->GetComponent<SpriteRenderer>();

    if (auto spriteRenderer = _spriteRenderer.lock())
    {
        if (!spriteRenderer->IsLoaded())
        {
            spriteRenderer->LoadTexture(FrameSpr, L"Data/Texture/UI/Timer/Frame.png");
        }
    }
}

// 更新処理
void QuestQueueUIController::Update(float elapsedTime)
{
}

// GUI描画
void QuestQueueUIController::DrawGui()
{
}
