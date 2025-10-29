#pragma once

#include "../../Library/Component/SpriteRenderer.h"

class TimerUIController : public Component
{
    public:
    TimerUIController() {}
    ~TimerUIController() override {}
    // 名前取得
    const char* GetName() const override { return "TimerUIController"; }
    // 開始処理
    void Start() override;
    // 更新処理
    void Update(float elapsedTime) override;
    // GUI描画
    void DrawGui() override;

#pragma region アクセサ
    void SetEndTimer(float endTimer) { _endTimer = endTimer; }
#pragma endregion

private:
    std::weak_ptr<SpriteRenderer> _spriteRenderer;

    // 各種スプライトの名前
    const std::string FrameSpr = "Frame";
    const std::string HandSpr = "Hand";
    const std::string EndHandSpr = "EndHand";

    RectTransform* _handRectTransform = nullptr;

    // 経過時間(s)
    float _timer = 0.0f;
    // 終了時間(s)
    float _endTimer = 30.0f * 60.0f;
};