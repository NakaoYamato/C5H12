#include "MenuUIController.h"

#include "../../Library/Actor/UI/UIActor.h"
#include "../../Library/Algorithm/Converter.h"
#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Scene/SceneManager.h"

#include "../../Source/Player/PlayerActor.h"

#include <imgui.h>

// 開始処理
void MenuUIController::Start()
{
    _spriteRenderer = this->GetActor()->GetComponent<SpriteRenderer>();

    if (auto spriteRenderer = _spriteRenderer.lock())
    {
        if (!spriteRenderer->IsLoaded())
        {
            spriteRenderer->LoadTexture(ProfileBackSpr, L"");
            spriteRenderer->LoadTexture(BackSpr,        L"Data/Texture/UI/Back.png");
            spriteRenderer->LoadTexture(ToTitleSpr, L"Data/Texture/UI/Back.png");
            spriteRenderer->LoadTexture(ToGameSpr, L"Data/Texture/UI/Back.png");
            spriteRenderer->LoadTexture(ToPlayerDebugSpr, L"Data/Texture/UI/Back.png");
            spriteRenderer->LoadTexture(QuitSpr, L"Data/Texture/UI/Back.png");
            spriteRenderer->LoadTexture(FrontSpr, L"Data/Texture/UI/Front.png");
        }
    }

    _profileText.color = Vector4::White;
    _profileText.position = Vector2(1310.0f, 160.0f);
    _hpText.color = Vector4::Green;
    _hpText.position = Vector2(1310.0f, 250.0f);
    _attackText.color = Vector4::Red;
    _attackText.position = Vector2(1310.0f, 300.0f);
    _defenseText.color = Vector4::Orange;
    _defenseText.position = Vector2(1310.0f, 350.0f);
    _skillText.color = Vector4::Yellow;
    _skillText.position = Vector2(1310.0f, 400.0f);
}

// 更新処理
void MenuUIController::Update(float elapsedTime)
{
    auto spriteRenderer = _spriteRenderer.lock();
    if (!spriteRenderer)
        return;

    // 入力処理
    if (_inputDirection & static_cast<unsigned int>(InputDirection::Up))
    {
        _selectType = static_cast<SelectType>(
            (static_cast<unsigned int>(_selectType) + 4) % 5
            );
    }
    if (_inputDirection & static_cast<unsigned int>(InputDirection::Down))
    {
        _selectType = static_cast<SelectType>(
            (static_cast<unsigned int>(_selectType) + 1) % 5
            );
    }
    switch (_inputState)
    {
    case MenuUIController::InputState::Select:
        switch (_selectType)
        {
        case MenuUIController::SelectType::Back:
            // 自身を停止
            Close();
            break;
        case MenuUIController::SelectType::ToTitle:
            SceneManager::Instance().ChangeScene(SceneMenuLevel::Game, "Title");
            break;
        case MenuUIController::SelectType::ToGame:
            SceneManager::Instance().ChangeScene(SceneMenuLevel::Game, "Game");
            break;
        case MenuUIController::SelectType::ToPlayerDebug:
            SceneManager::Instance().ChangeScene(SceneMenuLevel::Debug, "PlayerDebug");
            break;
        case MenuUIController::SelectType::Quit:
            PostMessage(Graphics::Instance().GetHwnd(), WM_CLOSE, 0, 0);
            break;
        }
        break;
    case MenuUIController::InputState::Back:
        // 自身を停止
        Close();
        break;
    }

    // 前面を選択中のスプライトの上に移動
    switch (_selectType)
    {
    case MenuUIController::SelectType::Back:
        spriteRenderer->GetRectTransform(FrontSpr).SetLocalPosition(
            spriteRenderer->GetRectTransform(BackSpr).GetWorldPosition()
        );
        break;
    case MenuUIController::SelectType::ToTitle:
        spriteRenderer->GetRectTransform(FrontSpr).SetLocalPosition(
            spriteRenderer->GetRectTransform(ToTitleSpr).GetWorldPosition()
        );
        break;
    case MenuUIController::SelectType::ToGame:
        spriteRenderer->GetRectTransform(FrontSpr).SetLocalPosition(
            spriteRenderer->GetRectTransform(ToGameSpr).GetWorldPosition()
        );
        break;
    case MenuUIController::SelectType::ToPlayerDebug:
        spriteRenderer->GetRectTransform(FrontSpr).SetLocalPosition(
            spriteRenderer->GetRectTransform(ToPlayerDebugSpr).GetWorldPosition()
        );
        break;
    case MenuUIController::SelectType::Quit:
        spriteRenderer->GetRectTransform(FrontSpr).SetLocalPosition(
            spriteRenderer->GetRectTransform(QuitSpr).GetWorldPosition()
        );
        break;
    }

    // プレイヤー情報の更新
    auto damageable = _damageable.lock();
    auto playerEquipmentController = _playerEquipmentController.lock();
    auto skillManager = ResourceManager::Instance().GetResourceAs<SkillManager>("SkillManager");
    if (!skillManager)
        return;
    if (damageable && playerEquipmentController)
    {
        _profileText.text = L"プレイヤー情報";
        _hpText.text = L"HP: " + std::to_wstring(static_cast<int>(damageable->GetHealth())) + L" / " + std::to_wstring(static_cast<int>(damageable->GetMaxHealth()));
        _attackText.text = L"攻撃力: " + std::to_wstring(static_cast<int>(playerEquipmentController->GetTotalAttack()));
        _defenseText.text = L"防御力: " + std::to_wstring(static_cast<int>(playerEquipmentController->GetTotalDefense()));
        _skillText.text = L"スキル\n";
        for (const auto& [skillType, level] : playerEquipmentController->GetCurrentSkills())
        {
            if (auto data = skillManager->GetSkillData(skillType))
            {
                std::wstring skillName = ToUtf16(data->name);
                _skillText.text += L"　　" + skillName + L"　:　レベル　" + std::to_wstring(level) + L"\n";
            }
        }
        if (playerEquipmentController->GetCurrentSkills().size() == 0)
            _skillText.text += L"\t無し";
    }
}

// 3D描画後の描画処理
void MenuUIController::DelayedRender(const RenderContext& rc)
{
    auto spriteRenderer = _spriteRenderer.lock();
    if (!spriteRenderer)
        return;

    _textData.color = Vector4::White;
    _textData.text = L"戻る";
    _textData.position = spriteRenderer->GetRectTransform(BackSpr).GetWorldPosition() + _textOffset;
    GetActor()->GetScene()->GetTextRenderer().Draw(_textData);
    _textData.text = L"タイトル";
    _textData.position = spriteRenderer->GetRectTransform(ToTitleSpr).GetWorldPosition() + _textOffset;
    GetActor()->GetScene()->GetTextRenderer().Draw(_textData);
    _textData.text = L"ゲーム開始";
    _textData.position = spriteRenderer->GetRectTransform(ToGameSpr).GetWorldPosition() + _textOffset;
    GetActor()->GetScene()->GetTextRenderer().Draw(_textData);
    _textData.text = L"プレイヤーデバッグ";
    _textData.position = spriteRenderer->GetRectTransform(ToPlayerDebugSpr).GetWorldPosition() + _textOffset;
    GetActor()->GetScene()->GetTextRenderer().Draw(_textData);
    _textData.text = L"終了";
    _textData.position = spriteRenderer->GetRectTransform(QuitSpr).GetWorldPosition() + _textOffset;
    _textData.color = Vector4::Red;
    GetActor()->GetScene()->GetTextRenderer().Draw(_textData);

    GetActor()->GetScene()->GetTextRenderer().Draw(_profileText);
    GetActor()->GetScene()->GetTextRenderer().Draw(_hpText);
    GetActor()->GetScene()->GetTextRenderer().Draw(_attackText);
    GetActor()->GetScene()->GetTextRenderer().Draw(_defenseText);
    GetActor()->GetScene()->GetTextRenderer().Draw(_skillText);

    // 入力UI描画
    InputUI::DrawInfo drawInfo;
    drawInfo.keyboardKey = 'F';
    drawInfo.gamePadKey = XINPUT_GAMEPAD_A;
    drawInfo.isActive = _INPUT_PRESSED("Select");
    switch (_selectType)
    {
    case MenuUIController::SelectType::Back:
        drawInfo.position = spriteRenderer->GetRectTransform(BackSpr).GetWorldPosition() + _inputUIOffset;
        break;
    case MenuUIController::SelectType::ToTitle:
        drawInfo.position = spriteRenderer->GetRectTransform(ToTitleSpr).GetWorldPosition() + _inputUIOffset;
        break;
    case MenuUIController::SelectType::ToGame:
        drawInfo.position = spriteRenderer->GetRectTransform(ToGameSpr).GetWorldPosition() + _inputUIOffset;
        break;
    case MenuUIController::SelectType::ToPlayerDebug:
        drawInfo.position = spriteRenderer->GetRectTransform(ToPlayerDebugSpr).GetWorldPosition() + _inputUIOffset;
        break;
    case MenuUIController::SelectType::Quit:
        drawInfo.position = spriteRenderer->GetRectTransform(QuitSpr).GetWorldPosition() + _inputUIOffset;
        break;
    }
    drawInfo.scale = Vector2::One;
    drawInfo.color = Vector4::BlueGreen;
    GetActor()->GetScene()->GetInputUI()->Draw(drawInfo);
}

// GUI描画
void MenuUIController::DrawGui()
{
    _textData.DrawGui(u8"テキスト");
    ImGui::DragFloat2(u8"テキストオフセット", &_textOffset.x, 0.1f);
    ImGui::DragFloat2(u8"入力UIオフセット", &_inputUIOffset.x, 0.1f);
    ImGui::Separator();

    _profileText.DrawGui(u8"プロフィールテキスト");
    _hpText.DrawGui(u8"HPテキスト");
    _attackText.DrawGui(u8"攻撃力テキスト");
    _defenseText.DrawGui(u8"防御力テキスト");
    _skillText.DrawGui(u8"スキルテキスト");
}

// 開く
void MenuUIController::Open()
{
    this->GetActor()->SetIsActive(true);
    _inputState = InputState::None;
    _inputDirection = 0;

    // プレイヤー情報を取得
    auto playerActor = GetActor()->GetScene()->GetActorManager().FindByClass<PlayerActor>(ActorTag::Player);
    if (playerActor)
    {
        _damageable                 = playerActor->GetComponent<Damageable>();
        _playerEquipmentController  = playerActor->GetComponent<PlayerEquipmentController>();
    }
}

// 閉じる
void MenuUIController::Close()
{
    this->GetActor()->SetIsActive(false);
    _inputState = InputState::None;
    _inputDirection = 0;
}