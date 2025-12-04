#pragma once

#include "../../Library/Component/SpriteRenderer.h"
#include "../../Library/Scene/Scene.h"

class TitleUIcontroller : public Component
{
public:
    enum class SelectType
    {
        ToGame,
        ToPlayerDebug,
        Quit
    };

public:
    TitleUIcontroller() {}
    ~TitleUIcontroller() override {}

    // 名前取得
    const char* GetName() const override { return "TitleUIcontroller"; }
    // 開始処理
    void Start() override;
    // 更新処理
    void Update(float elapsedTime) override;
    // GUI描画
    void DrawGui() override;

private:
	RectTransform* _myRectTransform = nullptr;
	std::weak_ptr<SpriteRenderer> _spriteRenderer;

    const std::string ToGameSpr = "ToGame";
    const std::string ToPlayerDebugSpr = "ToPlayerDebug";
    const std::string QuitSpr = "Quit";
    const std::string FrontSpr = "Front";

    SelectType _selectType = SelectType::ToGame;

    TextRenderer::TextDrawData _textData{};
    Vector2 _textOffset = Vector2(-245.0f, -20.0f);
    Vector2 _inputUIOffset = Vector2(-300.0f, 0.0f);
};