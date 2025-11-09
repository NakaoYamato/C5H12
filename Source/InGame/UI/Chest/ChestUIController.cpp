#include "ChestUIController.h"

#include "../../Library/Actor/UI/UIActor.h"
#include "../../Library/Scene/Scene.h"

#include <imgui.h>

// 開始処理
void ChestUIController::Start()
{
    _spriteRenderer = this->GetActor()->GetComponent<SpriteRenderer>();

    if (auto spriteRenderer = _spriteRenderer.lock())
    {
        spriteRenderer->LoadTexture(ItemBackSpr, L"Data/Texture/UI/Chest/Back.png");
        spriteRenderer->LoadTexture(ArmorBackSpr, L"Data/Texture/UI/Chest/Back.png");
        spriteRenderer->LoadTexture(TestBackSpr, L"Data/Texture/UI/Chest/Back.png");
        spriteRenderer->LoadTexture(FrontSpr, L"Data/Texture/UI/Chest/Front.png");

        spriteRenderer->GetRectTransform(ItemBackSpr).SetLocalScale(Vector2(0.5f, 0.4f));
		spriteRenderer->SetColor(ItemBackSpr, Vector4(1.0f, 1.0f, 1.0f, 0.6f));

        spriteRenderer->GetRectTransform(ArmorBackSpr).SetLocalScale(Vector2(0.5f, 0.4f));
        spriteRenderer->GetRectTransform(ArmorBackSpr).SetLocalPosition(Vector2(0.0f, BackPositionInterval));
        spriteRenderer->SetColor(ArmorBackSpr, Vector4(1.0f, 1.0f, 1.0f, 0.6f));

        spriteRenderer->GetRectTransform(TestBackSpr).SetLocalScale(Vector2(0.5f, 0.4f));
        spriteRenderer->GetRectTransform(TestBackSpr).SetLocalPosition(Vector2(0.0f, BackPositionInterval * 2.0f));
        spriteRenderer->SetColor(TestBackSpr, Vector4(1.0f, 1.0f, 1.0f, 0.6f));

        spriteRenderer->GetRectTransform(FrontSpr).SetLocalScale(Vector2(0.5f, 0.4f));
        spriteRenderer->SetColor(FrontSpr, Vector4(1.0f, 1.0f, 1.0f, 0.8f));
    }
}

// 更新処理
void ChestUIController::Update(float elapsedTime)
{
    auto spriteRenderer = _spriteRenderer.lock();
	if (!spriteRenderer)
		return;

    switch (_state)
    {
    case ChestUIController::SelectMenu:
        // indexの制限
        if (_index < 0)
            _index = SelectMenuOption::TestOption;
		else if (_index >= SelectMenuOption::MaxOption)
			_index = SelectMenuOption::ItemOption;

		// 背景位置更新
		spriteRenderer->GetRectTransform(FrontSpr).SetLocalPosition(Vector2(0.0f, _index * BackPositionInterval));
        break;
    case ChestUIController::ItemMenu:
        break;
    case ChestUIController::ArmorMenu:
        break;
    default:
        break;
    }
}

// 3D描画後の描画処理
void ChestUIController::DelayedRender(const RenderContext& rc)
{
    auto spriteRenderer = _spriteRenderer.lock();
    if (!spriteRenderer)
        return;
    auto& textRenderer = GetActor()->GetScene()->GetTextRenderer();


    switch (_state)
    {
    case ChestUIController::SelectMenu:
		// 各背景に描画
		textRenderer.Draw(FontType::MSGothic, 
            "アイテム",
            spriteRenderer->GetRectTransform(ItemBackSpr).GetWorldPosition() + _textOffset,
            Vector4::White,
            0.0f,
            _textOrigin,
            _textScale);
		textRenderer.Draw(FontType::MSGothic, 
            "防具",
            spriteRenderer->GetRectTransform(ArmorBackSpr).GetWorldPosition() + _textOffset,
            Vector4::White,
            0.0f,
            _textOrigin,
            _textScale);
		textRenderer.Draw(FontType::MSGothic, 
            "テスト",
            spriteRenderer->GetRectTransform(TestBackSpr).GetWorldPosition() + _textOffset,
            Vector4::White,
            0.0f,
            _textOrigin,
            _textScale);

        break;
    case ChestUIController::ItemMenu:
        break;
    case ChestUIController::ArmorMenu:
        break;
    default:
        break;
    }
}

// GUI描画
void ChestUIController::DrawGui()
{
    ImGui::Text(u8"テキスト");
	ImGui::DragFloat2(u8"オフセット", &_textOffset.x, 1.0f);
	ImGui::DragFloat2(u8"原点位置", &_textOrigin.x, 0.1f);
	ImGui::DragFloat2(u8"スケール", &_textScale.x, 0.1f);
    ImGui::Separator();

}

// ステート初期化
void ChestUIController::InitState()
{
    _state = State::SelectMenu;
    _index = 0;
}

// 次へ進む
void ChestUIController::NextState()
{
    switch (_state)
    {
    case ChestUIController::SelectMenu:
		if (_index == SelectMenuOption::ItemOption)
			_state = ChestUIController::ItemMenu;
		else if (_index == SelectMenuOption::ArmorOption)
			_state = ChestUIController::ArmorMenu;
        _index = 0;
        break;
    case ChestUIController::ItemMenu:
        break;
    case ChestUIController::ArmorMenu:
        break;
    default:
        break;
    }
}

// 前の状態へ戻る
void ChestUIController::PreviousState()
{
}
