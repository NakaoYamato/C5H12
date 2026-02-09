#pragma once

#include "../../Library/Component/SpriteRenderer.h"

class QuestQueueUIController : public Component
{
public:
    QuestQueueUIController() {}
    ~QuestQueueUIController() override {}
    // 名前取得
    const char* GetName() const override { return "QuestQueueUIController"; }
    // 開始処理
    void Start() override;
    // 更新処理
    void Update(float elapsedTime) override;
    // GUI描画
    void DrawGui() override;

private:
    std::weak_ptr<SpriteRenderer> _spriteRenderer;

    // 各種スプライトの名前
    const std::string FrameSpr = "Frame";
};