#pragma once

#include "../../Library/Component/SpriteRenderer.h"
#include "../../Library/Scene/Scene.h"

#include "../../Source/Common/Damageable.h"
#include "../../Source/Player/PlayerEquipmentController.h"

class MenuUIController : public Component
{
public:
    enum class SelectType
    {
        Back,
        ToTitle,
        ToGame,
        ToPlayerDebug,
        Quit
    };
    enum class InputDirection : unsigned int
    {
        None = 0b0000,
        Up = 0b0001,
        Down = 0b0010,
    };
    enum class InputState
    {
        None,

        Select,
        Back,
        L3,
        R3,
    };

public:
    MenuUIController() {}
    ~MenuUIController() override {}

    // 名前取得
    const char* GetName() const override { return "MenuUIController"; }
    // 開始処理
    void Start() override;
    // 更新処理
    void Update(float elapsedTime) override;
    // 3D描画後の描画処理
    void DelayedRender(const RenderContext& rc) override;
    // GUI描画
    void DrawGui() override;

    // 開く
    void Open();
    // 閉じる
    void Close();

    // 入力値設定
    void SetInputState(InputState inputState) { _inputState = inputState; }
    // 入力値設定
    void SetInputDirection(int inputDirection) { _inputDirection = inputDirection; }
private:
    std::weak_ptr<SpriteRenderer> _spriteRenderer;

    // 各種スプライトの名前
    const std::string ProfileBackSpr    = "ProfileBack";
    const std::string BackSpr           = "Back";
    const std::string ToTitleSpr        = "ToTitle";
    const std::string ToGameSpr         = "ToGame";
    const std::string ToPlayerDebugSpr  = "ToPlayerDebug";
    const std::string QuitSpr           = "Quit";
    const std::string FrontSpr          = "Front";

    SelectType _selectType = SelectType::Back;

    // 入力値
    InputState _inputState = InputState::None;
    int _inputDirection = 0;

    TextRenderer::TextDrawData _textData{};
    Vector2 _textOffset = Vector2(-245.0f, -20.0f);
    Vector2 _inputUIOffset = Vector2(-300.0f, 0.0f);

    std::weak_ptr<Damageable> _damageable;
    std::weak_ptr<PlayerEquipmentController> _playerEquipmentController;

    TextRenderer::TextDrawData _profileText{};
    TextRenderer::TextDrawData _hpText{};
    TextRenderer::TextDrawData _attackText{};
    TextRenderer::TextDrawData _defenseText{};
    TextRenderer::TextDrawData _skillText{};
};