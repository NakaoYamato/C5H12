#pragma once

#include "../../Library/Component/SpriteRenderer.h"

class TitleUI : public Component
{
public:
    TitleUI() {}
    ~TitleUI() override = default;
    // 名前取得
    const char* GetName() const override { return "TitleUI"; }
    // 開始処理
    void Start() override;
    // 更新処理
    void Update(float elapsedTime) override;
    // GUI描画
    void DrawGui() override;

private:
    // スプライトレンダラーへの参照
    std::weak_ptr<SpriteRenderer> _spriteRenderer;
    Vector2 _startButtonSize = Vector2::Zero; // スタートボタンのサイズ
};