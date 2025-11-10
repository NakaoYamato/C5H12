#include "ChestArmorMenuController.h"

#include "../../Library/Actor/UI/UIActor.h"
#include "../../Library/Scene/Scene.h"

#include <imgui.h>

// 開始処理
void ChestArmorMenuController::Start()
{
    _spriteRenderer = this->GetActor()->GetComponent<SpriteRenderer>();

    if (auto spriteRenderer = _spriteRenderer.lock())
    {
        spriteRenderer->LoadTexture(WeaponBackSpr, L"Data/Texture/UI/Chest/ArmorBack.png");
        spriteRenderer->LoadTexture(HeadBackSpr, L"Data/Texture/UI/Chest/ArmorBack.png");
        spriteRenderer->LoadTexture(ChestBackSpr, L"Data/Texture/UI/Chest/ArmorBack.png");
        spriteRenderer->LoadTexture(ArmBackSpr, L"Data/Texture/UI/Chest/ArmorBack.png");
        spriteRenderer->LoadTexture(WaistBackSpr, L"Data/Texture/UI/Chest/ArmorBack.png");
        spriteRenderer->LoadTexture(LegBackSpr, L"Data/Texture/UI/Chest/ArmorBack.png");
        spriteRenderer->LoadTexture(FrontSpr, L"Data/Texture/UI/Chest/ArmorFront.png");

        auto InitSpr = [&](const std::string& name, float index)
            {
                spriteRenderer->GetRectTransform(name).SetLocalScale(Vector2(0.5f, 0.5f));
                spriteRenderer->GetRectTransform(name).SetLocalPosition(SprStartPos + Vector2(0.0f, BackPositionInterval * index));
            };

		InitSpr(WeaponBackSpr,   0.0f);
		InitSpr(HeadBackSpr,     1.0f);
		InitSpr(ChestBackSpr,    2.0f);
		InitSpr(ArmBackSpr,      3.0f);
		InitSpr(WaistBackSpr,    4.0f);
		InitSpr(LegBackSpr,      5.0f);

		spriteRenderer->GetRectTransform(FrontSpr).SetLocalScale(Vector2(0.5f, 0.5f));
    }
}

// 3D描画後の描画処理
void ChestArmorMenuController::DelayedRender(const RenderContext& rc)
{
    auto spriteRenderer = _spriteRenderer.lock();
    if (!spriteRenderer)
        return;
    auto& textRenderer = GetActor()->GetScene()->GetTextRenderer();

    // 各背景に描画
	auto DrawSprText = [&](const std::string & name, const std::string & text)
	{
		textRenderer.Draw(FontType::MSGothic,
			text.c_str(),
			spriteRenderer->GetRectTransform(name).GetWorldPosition() + _textOffset,
            _textColor,
			0.0f,
			_textOrigin,
			_textScale);
	};
	DrawSprText(WeaponBackSpr, "武器");
	DrawSprText(HeadBackSpr, "頭");
	DrawSprText(ChestBackSpr, "胴");
	DrawSprText(ArmBackSpr, "腕");
	DrawSprText(WaistBackSpr, "腰");
	DrawSprText(LegBackSpr, "脚");
}

// GUI描画
void ChestArmorMenuController::DrawGui()
{
    ImGui::DragFloat2(u8"テキストオフセット", &_textOffset.x, 1.0f);
    ImGui::DragFloat2(u8"テキスト原点位置", &_textOrigin.x, 0.1f);
    ImGui::DragFloat2(u8"テキストスケール", &_textScale.x, 0.1f);
    ImGui::ColorEdit4(u8"テキスト通常色", &_textColor.x);
}

// インデックス追加
void ChestArmorMenuController::AddIndex(int val)
{
    _index = static_cast<AromrTypeSelectState>(static_cast<int>(_index) + val);

    // indexの制限
    if (_index < AromrTypeSelectState::Weapon)
        _index = AromrTypeSelectState::Leg;
    else if (_index >= AromrTypeSelectState::MaxType)
        _index = AromrTypeSelectState::Weapon;

    auto spriteRenderer = _spriteRenderer.lock();
    if (!spriteRenderer)
        return;

    // 背景位置更新
    Vector2 position{};
    switch (_index)
    {
    case AromrTypeSelectState::Weapon:
        position = spriteRenderer->GetRectTransform(WeaponBackSpr).GetLocalPosition();
        break;
    case AromrTypeSelectState::Head:
        position = spriteRenderer->GetRectTransform(HeadBackSpr).GetLocalPosition();
        break;
    case AromrTypeSelectState::Chest:
        position = spriteRenderer->GetRectTransform(ChestBackSpr).GetLocalPosition();
        break;
    case AromrTypeSelectState::Arm:
        position = spriteRenderer->GetRectTransform(ArmBackSpr).GetLocalPosition();
        break;
    case AromrTypeSelectState::Waist:
		position = spriteRenderer->GetRectTransform(WaistBackSpr).GetLocalPosition();
        break;
    case AromrTypeSelectState::Leg:
		position = spriteRenderer->GetRectTransform(LegBackSpr).GetLocalPosition();
        break;
    }

    spriteRenderer->GetRectTransform(FrontSpr).SetLocalPosition(position);
}
