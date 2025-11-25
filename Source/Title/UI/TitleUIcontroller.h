#pragma once

#include "../../Library/Component/SpriteRenderer.h"
#include "../../Library/Scene/Scene.h"

class TitleUIcontroller : public Component
{
public:
    TitleUIcontroller() {}
    ~TitleUIcontroller() override {}

    // 名前取得
    const char* GetName() const override { return "TitleUIcontroller"; }
    // 開始処理
    void Start() override;
    // 更新処理
    void Update(float elapsedTime) override;

private:
	RectTransform* _myRectTransform = nullptr;
	std::weak_ptr<SpriteRenderer> _spriteRenderer;

    const std::string BoxSpr = "Box";

    Vector2 _textOffset = Vector2(-150.0f, 0.0f);
};