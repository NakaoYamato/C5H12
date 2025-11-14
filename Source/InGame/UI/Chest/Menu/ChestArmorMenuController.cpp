#include "ChestArmorMenuController.h"

#include "../../Library/Actor/UI/UIActor.h"
#include "../../Library/Scene/Scene.h"
#include "../../Library/Algorithm/Converter.h"

#include <imgui.h>

// 開始処理
void ChestArmorMenuController::Start()
{
    _userDataManager = ResourceManager::Instance().GetResourceAs<UserDataManager>("UserDataManager");

    _spriteRenderer = this->GetActor()->GetComponent<SpriteRenderer>();

    if (auto spriteRenderer = _spriteRenderer.lock())
    {
        spriteRenderer->LoadTexture(BackSpr, L"");
        spriteRenderer->LoadTexture(WeaponBackSpr, L"Data/Texture/UI/Chest/ArmorBack.png");
        spriteRenderer->LoadTexture(HeadBackSpr, L"Data/Texture/UI/Chest/ArmorBack.png");
        spriteRenderer->LoadTexture(ChestBackSpr, L"Data/Texture/UI/Chest/ArmorBack.png");
        spriteRenderer->LoadTexture(ArmBackSpr, L"Data/Texture/UI/Chest/ArmorBack.png");
        spriteRenderer->LoadTexture(WaistBackSpr, L"Data/Texture/UI/Chest/ArmorBack.png");
        spriteRenderer->LoadTexture(LegBackSpr, L"Data/Texture/UI/Chest/ArmorBack.png");

        spriteRenderer->LoadTexture(WeaponSpr, L"");
        spriteRenderer->LoadTexture(HeadSpr, L"Data/Texture/UI/Chest/ArmorHead.png");
        spriteRenderer->LoadTexture(ChestSpr, L"Data/Texture/UI/Chest/ArmorChest.png");
        spriteRenderer->LoadTexture(ArmSpr, L"Data/Texture/UI/Chest/ArmorArm.png");
        spriteRenderer->LoadTexture(WaistSpr, L"Data/Texture/UI/Chest/ArmorWaist.png");
        spriteRenderer->LoadTexture(LegSpr, L"Data/Texture/UI/Chest/ArmorLeg.png");

        spriteRenderer->LoadTexture(FrontSpr, L"Data/Texture/UI/Chest/ArmorFront.png");
        spriteRenderer->LoadTexture(BoxBackSpr, L"");

        spriteRenderer->SetCenterAlignment(BackSpr, Sprite::LeftUp);
        spriteRenderer->GetRectTransform(BackSpr).SetLocalPosition(Vector2(80.0f, 110.0f));
        spriteRenderer->GetRectTransform(BackSpr).SetLocalScale(Vector2(67.0f, 50.0f));
        spriteRenderer->SetColor(BackSpr, Vector4(0.0f, 0.0f, 0.0f, 90.0f / 255.0f));

        spriteRenderer->SetCenterAlignment(BoxBackSpr, Sprite::LeftUp);
        spriteRenderer->GetRectTransform(BoxBackSpr).SetLocalPosition(Vector2(450.0f, 120.0f));
        spriteRenderer->GetRectTransform(BoxBackSpr).SetLocalScale(Vector2(43.0f, 49.0f));
        spriteRenderer->SetColor(BoxBackSpr, Vector4(0.0f, 0.0f, 0.0f, 0.0f));
        

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

		auto InitArmorSpr = [&](const std::string& name, const std::string& parent, SelectType type)
			{
				spriteRenderer->GetRectTransform(name).SetLocalScale(Vector2(0.25f, 0.25f));
				spriteRenderer->GetRectTransform(name).SetLocalPosition(spriteRenderer->GetRectTransform(parent).GetLocalPosition());
			};
		InitArmorSpr(WeaponSpr, WeaponBackSpr, SelectType::Weapon);
		InitArmorSpr(HeadSpr, HeadBackSpr, SelectType::Head);
		InitArmorSpr(ChestSpr, ChestBackSpr, SelectType::Chest);
		InitArmorSpr(ArmSpr, ArmBackSpr, SelectType::Arm);
		InitArmorSpr(WaistSpr, WaistBackSpr, SelectType::Waist);
		InitArmorSpr(LegSpr, LegBackSpr, SelectType::Leg);

		spriteRenderer->GetRectTransform(FrontSpr).SetLocalScale(Vector2(0.5f, 0.5f));
    }

	_armorBackSprite = std::make_unique<Sprite>(L"Data/Texture/UI/Chest/ArmorBack.png", Sprite::LeftUp);
    _armorSprites[static_cast<int>(SelectType::Weapon)] = 
        std::make_unique<Sprite>(L"", Sprite::LeftUp);
    _armorSprites[static_cast<int>(SelectType::Head)] =
        std::make_unique<Sprite>(L"Data/Texture/UI/Chest/ArmorHead.png", Sprite::LeftUp);
    _armorSprites[static_cast<int>(SelectType::Chest)] =
        std::make_unique<Sprite>(L"Data/Texture/UI/Chest/ArmorChest.png", Sprite::LeftUp);
    _armorSprites[static_cast<int>(SelectType::Arm)] =
        std::make_unique<Sprite>(L"Data/Texture/UI/Chest/ArmorArm.png", Sprite::LeftUp);
    _armorSprites[static_cast<int>(SelectType::Waist)] =
        std::make_unique<Sprite>(L"Data/Texture/UI/Chest/ArmorWaist.png", Sprite::LeftUp);
    _armorSprites[static_cast<int>(SelectType::Leg)] =
        std::make_unique<Sprite>(L"Data/Texture/UI/Chest/ArmorLeg.png", Sprite::LeftUp);
    _armorFrontSprite = std::make_unique<Sprite>(L"Data/Texture/UI/Chest/ArmorFront.png", Sprite::LeftUp);

	_armorBackSprite->GetRectTransform().SetReflectParentScale(false);
	_armorBackSprite->GetRectTransform().SetLocalScale(Vector2(0.5f, 0.5f));
    for (auto& armorSpr : _armorSprites)
    {
		armorSpr->GetRectTransform().SetReflectParentScale(false);
        armorSpr->GetRectTransform().SetLocalScale(Vector2(0.25f, 0.25f));
    }
	_armorFrontSprite->GetRectTransform().SetReflectParentScale(false);
    _armorFrontSprite->GetRectTransform().SetLocalScale(Vector2(0.5f, 0.5f));
}

// 更新処理
void ChestArmorMenuController::Update(float elapsedTime)
{
	auto playerArmorController = _playerArmorController.lock();
    if (!playerArmorController)
        return;
    auto spriteRenderer = _spriteRenderer.lock();
    if (!spriteRenderer)
        return;

    // 防具の色を設定
    auto SetColor = [&](const std::string& name, SelectType type)
        {
            ArmorType armorType = ArmorType::Head;
            switch (type)
            {
            case SelectType::Head:
                armorType = ArmorType::Head;
                break;
            case SelectType::Chest:
                armorType = ArmorType::Chest;
                break;
            case SelectType::Arm:
                armorType = ArmorType::Arm;
                break;
            case SelectType::Waist:
                armorType = ArmorType::Waist;
                break;
            case SelectType::Leg:
                armorType = ArmorType::Leg;
                break;
            }
            int armorIndex = playerArmorController->GetArmorIndex(armorType);
			auto userDataManager = _userDataManager.lock();
            if (!userDataManager)
                return;
            auto armorUserData = userDataManager->GetAcquiredArmorData(armorType, armorIndex);
            if (!armorUserData || !armorUserData->GetBaseData())
            {
                // データがないなら透明にする
                spriteRenderer->SetColorAlpha(name, 0.0f);
            }
            else
            {
                spriteRenderer->SetColor(name, armorUserData->GetBaseData()->GetRarityColor(armorUserData->GetBaseData()->rarity));
            }
        };
    SetColor(HeadSpr, SelectType::Head);
    SetColor(ChestSpr, SelectType::Chest);
    SetColor(ArmSpr, SelectType::Arm);
    SetColor(WaistSpr, SelectType::Waist);
    SetColor(LegSpr, SelectType::Leg);

    // 選択中の防具を光らせる

}

// 3D描画後の描画処理
void ChestArmorMenuController::DelayedRender(const RenderContext& rc)
{
    auto spriteRenderer = _spriteRenderer.lock();
    if (!spriteRenderer)
        return;
    auto userDataManager = _userDataManager.lock();
    if (!userDataManager)
        return;
    auto& textureRenderer = GetActor()->GetScene()->GetTextureRenderer();

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

    switch (_state)
    {
    case ChestArmorMenuController::State::SelectArmorType:
        DrawSprText(WeaponBackSpr, "武器");
        DrawSprText(HeadBackSpr, "頭");
        DrawSprText(ChestBackSpr, "胴");
        DrawSprText(ArmBackSpr, "腕");
        DrawSprText(WaistBackSpr, "腰");
        DrawSprText(LegBackSpr, "脚");
        break;
    case ChestArmorMenuController::State::SelectArmor:
        for (int c = 0; c < _armorSprColumnNum; ++c)
        {
			for (int r = 0; r < _armorSprRowNum; ++r)
			{
                int index = r * _armorSprColumnNum + c;
				UserDataManager::ArmorUserData* armorUserData = nullptr;
                switch (_selectType)
                {
                case ChestArmorMenuController::SelectType::Weapon:
                    break;
                case ChestArmorMenuController::SelectType::Head:
                    armorUserData = userDataManager->GetAcquiredArmorData(ArmorType::Head, index);
                    break;
                case ChestArmorMenuController::SelectType::Chest:
                    armorUserData = userDataManager->GetAcquiredArmorData(ArmorType::Chest, index);
                    break;
                case ChestArmorMenuController::SelectType::Arm:
                    armorUserData = userDataManager->GetAcquiredArmorData(ArmorType::Arm, index);
                    break;
                case ChestArmorMenuController::SelectType::Waist:
                    armorUserData = userDataManager->GetAcquiredArmorData(ArmorType::Waist, index);
                    break;
                case ChestArmorMenuController::SelectType::Leg:
                    armorUserData = userDataManager->GetAcquiredArmorData(ArmorType::Leg, index);
                    break;
                }

                Vector2 position{};
                position.x = _armorSprPositionInterval.x * c;
                position.y = _armorSprPositionInterval.y * r;
                _armorBackSprite->GetRectTransform().SetLocalPosition(position);
                _armorBackSprite->GetRectTransform().UpdateTransform(&spriteRenderer->GetRectTransform(BoxBackSpr));

                ArmorData* armorData = armorUserData ? armorUserData->GetBaseData() : nullptr;
                if (armorData)
                {
                    _armorBackSprite->SetColorAlpha(1.0f);
                    _armorBackSprite->Render(rc, textureRenderer);

                    _armorSprites[static_cast<int>(_selectType)]->SetColor(armorData->GetRarityColor(armorData->rarity));
                    _armorSprites[static_cast<int>(_selectType)]->GetRectTransform().SetLocalPosition(position);
                    _armorSprites[static_cast<int>(_selectType)]->GetRectTransform().UpdateTransform(&spriteRenderer->GetRectTransform(BoxBackSpr));
                    _armorSprites[static_cast<int>(_selectType)]->Render(rc, textureRenderer);
                }
                else
                {
                    _armorBackSprite->SetColorAlpha(0.5f);
                    _armorBackSprite->Render(rc, textureRenderer);
                }

			}
        }

        {
            Vector2 position{};
            position.x = _armorSprPositionInterval.x * _selectArmorColumnIndex;
            position.y = _armorSprPositionInterval.y * _selectArmorRowIndex;
            _armorFrontSprite->GetRectTransform().SetLocalPosition(position);
            _armorFrontSprite->GetRectTransform().UpdateTransform(&spriteRenderer->GetRectTransform(BoxBackSpr));
            _armorFrontSprite->Render(rc, textureRenderer);
        }
        break;
    }
}

// GUI描画
void ChestArmorMenuController::DrawGui()
{
    ImGui::DragFloat2(u8"テキストオフセット", &_textOffset.x, 1.0f);
    ImGui::DragFloat2(u8"テキスト原点位置", &_textOrigin.x, 0.1f);
    ImGui::DragFloat2(u8"テキストスケール", &_textScale.x, 0.1f);
    ImGui::ColorEdit4(u8"テキスト通常色", &_textColor.x);
    ImGui::Separator();
	if (ImGui::TreeNode(u8"スプライト"))
	{
		if (ImGui::TreeNode(u8"装備背景"))
		{
			_armorBackSprite->DrawGui();
			ImGui::TreePop();
		}
		for (int i = 0; i < static_cast<int>(SelectType::MaxType); ++i)
		{
            if (ImGui::TreeNode(ToString<SelectType>(static_cast<size_t>(i)).c_str()))
            {
                _armorSprites[i]->DrawGui();
                ImGui::TreePop();
            }
		}
        if (ImGui::TreeNode(u8"装備前景"))
        {
			_armorFrontSprite->DrawGui();
			ImGui::TreePop();
        }

		ImGui::TreePop();
	}
}

// インデックス追加
void ChestArmorMenuController::AddIndex(int val)
{
	// 状態確認
	if (_state != State::SelectArmorType)
		return;

    _selectType = static_cast<SelectType>(static_cast<int>(_selectType) + val);

    // indexの制限
    if (_selectType < SelectType::Weapon)
        _selectType = SelectType::Leg;
    else if (_selectType >= SelectType::MaxType)
        _selectType = SelectType::Weapon;

    auto spriteRenderer = _spriteRenderer.lock();
    if (!spriteRenderer)
        return;

    // 背景位置更新
    Vector2 position{};
    switch (_selectType)
    {
    case SelectType::Weapon:
        position = spriteRenderer->GetRectTransform(WeaponBackSpr).GetLocalPosition();
        break;
    case SelectType::Head:
        position = spriteRenderer->GetRectTransform(HeadBackSpr).GetLocalPosition();
        break;
    case SelectType::Chest:
        position = spriteRenderer->GetRectTransform(ChestBackSpr).GetLocalPosition();
        break;
    case SelectType::Arm:
        position = spriteRenderer->GetRectTransform(ArmBackSpr).GetLocalPosition();
        break;
    case SelectType::Waist:
		position = spriteRenderer->GetRectTransform(WaistBackSpr).GetLocalPosition();
        break;
    case SelectType::Leg:
		position = spriteRenderer->GetRectTransform(LegBackSpr).GetLocalPosition();
        break;
    }

    spriteRenderer->GetRectTransform(FrontSpr).SetLocalPosition(position);
}

// 装備インデックス追加
void ChestArmorMenuController::AddSelectArmorColumnIndex(int val)
{
    // 状態確認
	if (_state != State::SelectArmor)
		return;
	_selectArmorColumnIndex += val;
	// indexの制限
	if (_selectArmorColumnIndex < 0)
		_selectArmorColumnIndex = _armorSprColumnNum - 1;
	else if (_selectArmorColumnIndex >= _armorSprColumnNum)
		_selectArmorColumnIndex = 0;
}

// 装備インデックス追加
void ChestArmorMenuController::AddSelectArmorRowIndex(int val)
{
    // 状態確認
	if (_state != State::SelectArmor)
		return;
	_selectArmorRowIndex += val;
	// indexの制限
	if (_selectArmorRowIndex < 0)
		_selectArmorRowIndex = _armorSprRowNum - 1;
	else if (_selectArmorRowIndex >= _armorSprRowNum)
		_selectArmorRowIndex = 0;
}

// 次へ進む
void ChestArmorMenuController::NextState()
{
    auto userDataManager = _userDataManager.lock();
    if (!userDataManager)
        return;
    auto spriteRenderer = _spriteRenderer.lock();
    if (!spriteRenderer)
        return;
    auto playerArmorController = _playerArmorController.lock();
	if (!playerArmorController)
		return;

    ArmorType armorType = ArmorType::Head;
    switch (_selectType)
    {
    case SelectType::Head:
        armorType = ArmorType::Head;
        break;
    case SelectType::Chest:
        armorType = ArmorType::Chest;
        break;
    case SelectType::Arm:
        armorType = ArmorType::Arm;
        break;
    case SelectType::Waist:
        armorType = ArmorType::Waist;
        break;
    case SelectType::Leg:
        armorType = ArmorType::Leg;
        break;
    }
    size_t armorIndex = playerArmorController->GetArmorIndex(armorType);

    switch (_state)
    {
    case State::SelectArmorType:
		spriteRenderer->SetColorAlpha(BoxBackSpr, 0.5f);

		// 現在選択中の防具にインデックスを合わせる
        _selectArmorColumnIndex = static_cast<int>(armorIndex % _armorSprColumnNum);
        _selectArmorRowIndex = static_cast<int>(armorIndex / _armorSprColumnNum);

		_state = State::SelectArmor;
        break;
    case State::SelectArmor:
		// 防具変更
		armorIndex = _selectArmorRowIndex * _armorSprColumnNum + _selectArmorColumnIndex;
		// 現在装備中の防具と同じなら脱ぐ
		if (armorIndex == static_cast<size_t>(playerArmorController->GetArmorIndex(armorType)))
            playerArmorController->SetArmorIndex(armorType, -1);
		// 選択中のマスに対応する防具が存在するか確認
        else if (armorIndex < static_cast<int>(userDataManager->GetAcquiredArmorDataList(armorType).size()))
            playerArmorController->SetArmorIndex(armorType, static_cast<int>(armorIndex));
        break;
    }
}

// 前の状態へ戻る
bool ChestArmorMenuController::PreviousState()
{
    auto spriteRenderer = _spriteRenderer.lock();
    if (!spriteRenderer)
        return false;

    switch (_state)
    {
    case State::SelectArmorType:
        return true;
        break;
    case State::SelectArmor:
        spriteRenderer->SetColorAlpha(BoxBackSpr, 0.0f);
        _state = State::SelectArmorType;
        break;
    }
    return false;
}

// リセット
void ChestArmorMenuController::Reset()
{
    _selectType = SelectType::Weapon;
    _state = State::SelectArmorType;

	if (auto spriteRenderer = _spriteRenderer.lock())
	{
		spriteRenderer->GetRectTransform(FrontSpr).SetLocalPosition(
			spriteRenderer->GetRectTransform(WeaponBackSpr).GetLocalPosition());
	}
}

void ChestArmorMenuController::SetPlayerArmorController(const std::shared_ptr<PlayerArmorController>& controller)
{
	if (!controller)
		return;
    auto spriteRenderer = _spriteRenderer.lock();
    if (!spriteRenderer)
        return;

    _playerArmorController = controller;

    // 防具の色を設定
    auto SetColor = [&](const std::string& name, SelectType type)
        {
            ArmorType armorType = ArmorType::Head;
            switch (type)
            {
            case SelectType::Head:
                armorType = ArmorType::Head;
                break;
            case SelectType::Chest:
                armorType = ArmorType::Chest;
                break;
            case SelectType::Arm:
                armorType = ArmorType::Arm;
                break;
            case SelectType::Waist:
                armorType = ArmorType::Waist;
                break;
            case SelectType::Leg:
                armorType = ArmorType::Leg;
                break;
            }
            int armorIndex = controller->GetArmorIndex(armorType);
            auto userDataManager = _userDataManager.lock();
            if (!userDataManager)
                return;
            auto armorUserData = userDataManager->GetAcquiredArmorData(armorType, armorIndex);
            if (!armorUserData || !armorUserData->GetBaseData())
            {
                // データがないなら透明にする
                spriteRenderer->SetColorAlpha(name, 0.0f);
            }
            else
            {
                spriteRenderer->SetColor(name, armorUserData->GetBaseData()->GetRarityColor(armorUserData->GetBaseData()->rarity));
            }
        };
	SetColor(HeadSpr, SelectType::Head);
	SetColor(ChestSpr, SelectType::Chest);
	SetColor(ArmSpr, SelectType::Arm);
	SetColor(WaistSpr, SelectType::Waist);
	SetColor(LegSpr, SelectType::Leg);
}
