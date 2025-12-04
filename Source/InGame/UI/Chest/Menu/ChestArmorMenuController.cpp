#include "ChestArmorMenuController.h"

#include "../ChestUIController.h"
#include "../../Library/Actor/UI/UIActor.h"
#include "../../Library/Algorithm/Converter.h"
#include "../../Source/Skill/SkillManager.h"

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

		for (int i = 0; i < static_cast<int>(WeaponType::WeaponTypeMax); ++i)
		{
			std::wstring path = L"Data/Texture/UI/Chest/";
			path += ToWString(WeaponSprs[i]);
			path += L".png";
			spriteRenderer->LoadTexture(WeaponSprs[i], path.c_str());
		}
        spriteRenderer->LoadTexture(HeadSpr, L"Data/Texture/UI/Chest/ArmorHead.png");
        spriteRenderer->LoadTexture(ChestSpr, L"Data/Texture/UI/Chest/ArmorChest.png");
        spriteRenderer->LoadTexture(ArmSpr, L"Data/Texture/UI/Chest/ArmorArm.png");
        spriteRenderer->LoadTexture(WaistSpr, L"Data/Texture/UI/Chest/ArmorWaist.png");
        spriteRenderer->LoadTexture(LegSpr, L"Data/Texture/UI/Chest/ArmorLeg.png");

        spriteRenderer->LoadTexture(FrontSpr, L"Data/Texture/UI/Chest/ArmorFront.png");
        spriteRenderer->LoadTexture(BoxBackSpr, L"");

        spriteRenderer->LoadTexture(TextBoxSpr, L"");

        spriteRenderer->SetCenterAlignment(BackSpr, Sprite::LeftUp);
        spriteRenderer->GetRectTransform(BackSpr).SetLocalPosition(Vector2(80.0f, 110.0f));
        spriteRenderer->GetRectTransform(BackSpr).SetLocalScale(Vector2(77.0f, 50.0f));
        spriteRenderer->SetColor(BackSpr, Vector4(0.0f, 0.0f, 0.0f, 90.0f / 255.0f));

        spriteRenderer->SetCenterAlignment(BoxBackSpr, Sprite::LeftUp);
        spriteRenderer->GetRectTransform(BoxBackSpr).SetLocalPosition(Vector2(245.0f, 120.0f));
        spriteRenderer->GetRectTransform(BoxBackSpr).SetLocalScale(Vector2(34.5f, 49.0f));
        spriteRenderer->SetColor(BoxBackSpr, Vector4(0.0f, 0.0f, 0.0f, 0.0f));

        spriteRenderer->SetCenterAlignment(TextBoxSpr, Sprite::LeftUp);
        spriteRenderer->GetRectTransform(TextBoxSpr).SetLocalPosition(Vector2(810.0f, 110.0f));
        spriteRenderer->GetRectTransform(TextBoxSpr).SetLocalScale(Vector2(31.0f, 50.0f));
        spriteRenderer->SetColor(TextBoxSpr, Vector4(0.0f, 0.0f, 0.0f, 90.0f / 255.0f));

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
		for (int i = 0; i < static_cast<int>(WeaponType::WeaponTypeMax); ++i)
		{
			InitArmorSpr(WeaponSprs[i], WeaponBackSpr, SelectType::Weapon);
		}
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

#pragma region 武器UI
	_weaponSprites[static_cast<int>(WeaponType::GreatSword)] =
		std::make_unique<Sprite>(L"Data/Texture/UI/Chest/GreatSword.png", Sprite::LeftUp);
#pragma endregion

#pragma region 防具UI
    _armorSprites[static_cast<int>(ArmorType::Head)] =
        std::make_unique<Sprite>(L"Data/Texture/UI/Chest/ArmorHead.png", Sprite::LeftUp);
    _armorSprites[static_cast<int>(ArmorType::Chest)] =
        std::make_unique<Sprite>(L"Data/Texture/UI/Chest/ArmorChest.png", Sprite::LeftUp);
    _armorSprites[static_cast<int>(ArmorType::Arm)] =
        std::make_unique<Sprite>(L"Data/Texture/UI/Chest/ArmorArm.png", Sprite::LeftUp);
    _armorSprites[static_cast<int>(ArmorType::Waist)] =
        std::make_unique<Sprite>(L"Data/Texture/UI/Chest/ArmorWaist.png", Sprite::LeftUp);
    _armorSprites[static_cast<int>(ArmorType::Leg)] =
        std::make_unique<Sprite>(L"Data/Texture/UI/Chest/ArmorLeg.png", Sprite::LeftUp);
#pragma endregion

    _armorFrontSprite = std::make_unique<Sprite>(L"Data/Texture/UI/Chest/ArmorFront.png", Sprite::LeftUp);

	_armorBackSprite->GetRectTransform().SetReflectParentScale(false);
	_armorBackSprite->GetRectTransform().SetLocalScale(Vector2(0.5f, 0.5f));
    for (auto& weaponSpr : _weaponSprites)
    {
        weaponSpr->GetRectTransform().SetReflectParentScale(false);
        weaponSpr->GetRectTransform().SetLocalScale(Vector2(0.25f, 0.25f));
    }
    for (auto& armorSpr : _armorSprites)
    {
		armorSpr->GetRectTransform().SetReflectParentScale(false);
        armorSpr->GetRectTransform().SetLocalScale(Vector2(0.25f, 0.25f));
    }
	_armorFrontSprite->GetRectTransform().SetReflectParentScale(false);
    _armorFrontSprite->GetRectTransform().SetLocalScale(Vector2(0.5f, 0.5f));

	// 防具説明テキストパラメータ設定
    _nameTextData.position = Vector2(820.0f, 135.0f);
    _nameTextData.scale = Vector2(1.0f, 1.0f);
    _rarityextData.position = Vector2(820.0f, 190.0f);
    _rarityextData.scale = Vector2(1.0f, 1.0f);
    _rarityextData.color = Vector4::BlueGreen;
	_defenseTextData.position = Vector2(820.0f, 245.0f);
	_defenseTextData.scale = Vector2(1.0f, 1.0f);
    _defenseTextData.color = Vector4::Orange;
	_skillTextData.text = L"スキル";
    _skillTextData.position = Vector2(820.0f, 300.0f);
    _skillTextData.scale = Vector2(1.0f, 1.0f);
    _skillTextData.color = Vector4::Red;
}

// 更新処理
void ChestArmorMenuController::Update(float elapsedTime)
{
	auto playerEquipmentController = _PlayerEquipmentController.lock();
    if (!playerEquipmentController)
        return;
    auto spriteRenderer = _spriteRenderer.lock();
    if (!spriteRenderer)
        return;
    auto userDataManager = _userDataManager.lock();
    if (!userDataManager)
        return;

    // 装備中の武器のUIをアクティブ化
    {
        auto type = userDataManager->GetEquippedWeaponType();
		auto weaponData = userDataManager->GetEquippedWeaponData();
		for (int i = 0; i < static_cast<int>(WeaponType::WeaponTypeMax); ++i)
		{
            spriteRenderer->SetColor(WeaponSprs[i], weaponData ? WeaponData::GetRarityColor(weaponData->GetBaseData()->rarity) : Vector4::Zero);
			if (i == static_cast<int>(type))
			{
				spriteRenderer->SetColorAlpha(WeaponSprs[i], 1.0f);
			}
			else
			{
                spriteRenderer->SetColorAlpha(WeaponSprs[i], 0.0f);
			}
		}
    }

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
            int armorIndex = playerEquipmentController->GetArmorIndex(armorType);
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
    switch (_selectType)
    {
    case ChestArmorMenuController::SelectType::Weapon:
		playerEquipmentController->GetWeaponActor()->SetIsOverrideRimLight(true);
		playerEquipmentController->GetWeaponActor()->SetRimLightColor(_selectedRimLightColor);

		// 他の防具は光らせない
		for (int i = 0; i <= static_cast<int>(ArmorType::Leg); ++i)
		{
            playerEquipmentController->GetArmorActor(static_cast<ArmorType>(i))->SetIsOverrideRimLight(false);
		}

        break;
    case ChestArmorMenuController::SelectType::Head:
    case ChestArmorMenuController::SelectType::Chest:
    case ChestArmorMenuController::SelectType::Arm:
    case ChestArmorMenuController::SelectType::Waist:
    case ChestArmorMenuController::SelectType::Leg:
    {
        ArmorType type = static_cast<ArmorType>(static_cast<int>(_selectType) - 1);
		playerEquipmentController->GetArmorActor(type)->SetIsOverrideRimLight(true);
		playerEquipmentController->GetArmorActor(type)->SetRimLightColor(_selectedRimLightColor);

		// 他の防具は光らせない
        playerEquipmentController->GetWeaponActor()->SetIsOverrideRimLight(false);
		for (int i = 0; i <= static_cast<int>(ArmorType::Leg); ++i)
		{
			if (i == static_cast<int>(type))
				continue;
            playerEquipmentController->GetArmorActor(static_cast<ArmorType>(i))->SetIsOverrideRimLight(false);
		}
        break;
    }
    }
}

// 3D描画後の描画処理
void ChestArmorMenuController::DelayedRender(const RenderContext& rc)
{
    auto playerEquipmentController = _PlayerEquipmentController.lock();
    if (!playerEquipmentController)
        return;
    auto spriteRenderer = _spriteRenderer.lock();
    if (!spriteRenderer)
        return;
    auto userDataManager = _userDataManager.lock();
    if (!userDataManager)
        return;
    auto skillManager = ResourceManager::Instance().GetResourceAs<SkillManager>("SkillManager");
    if (!skillManager)
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
                Sprite* spr = nullptr;

				UserDataManager::WeaponUserData* weaponUserData = nullptr;
				UserDataManager::ArmorUserData* armorUserData = nullptr;
                switch (_selectType)
                {
                case ChestArmorMenuController::SelectType::Weapon:
					weaponUserData = userDataManager->GetAcquiredWeaponData(WeaponType::GreatSword, index);
					spr = _weaponSprites[static_cast<int>(WeaponType::GreatSword)].get();
                    break;
                case ChestArmorMenuController::SelectType::Head:
                    armorUserData = userDataManager->GetAcquiredArmorData(ArmorType::Head, index);
					spr = _armorSprites[static_cast<int>(ArmorType::Head)].get();
                    break;
                case ChestArmorMenuController::SelectType::Chest:
                    armorUserData = userDataManager->GetAcquiredArmorData(ArmorType::Chest, index);
					spr = _armorSprites[static_cast<int>(ArmorType::Chest)].get();
                    break;
                case ChestArmorMenuController::SelectType::Arm:
                    armorUserData = userDataManager->GetAcquiredArmorData(ArmorType::Arm, index);
					spr = _armorSprites[static_cast<int>(ArmorType::Arm)].get();
                    break;
                case ChestArmorMenuController::SelectType::Waist:
                    armorUserData = userDataManager->GetAcquiredArmorData(ArmorType::Waist, index);
					spr = _armorSprites[static_cast<int>(ArmorType::Waist)].get();
                    break;
                case ChestArmorMenuController::SelectType::Leg:
                    armorUserData = userDataManager->GetAcquiredArmorData(ArmorType::Leg, index);
					spr = _armorSprites[static_cast<int>(ArmorType::Leg)].get();
                    break;
                }

				// 位置設定
                Vector2 position{};
                position.x = _armorSprPositionInterval.x * c;
                position.y = _armorSprPositionInterval.y * r;
                _armorBackSprite->GetRectTransform().SetLocalPosition(position);
                _armorBackSprite->GetRectTransform().UpdateTransform(&spriteRenderer->GetRectTransform(BoxBackSpr));

                if (_selectType == ChestArmorMenuController::SelectType::Weapon)
                {
                    // 武器の一覧表示
					WeaponData* weaponData = weaponUserData ? weaponUserData->GetBaseData() : nullptr;
                    if (weaponData && spr)
                    {
                        _armorBackSprite->SetColorAlpha(1.0f);
                        _armorBackSprite->Render(rc, textureRenderer);

                        // 防具スプライト描画
                        spr->SetColor(weaponData->GetRarityColor(weaponData->rarity));
                        spr->GetRectTransform().SetLocalPosition(position);
                        spr->GetRectTransform().UpdateTransform(&spriteRenderer->GetRectTransform(BoxBackSpr));
                        spr->Render(rc, textureRenderer);
                    }
                    else
                    {
                        _armorBackSprite->SetColorAlpha(0.5f);
                        _armorBackSprite->Render(rc, textureRenderer);
                    }
                }
                else
                {
					// 防具の一覧表示
                    ArmorData* armorData = armorUserData ? armorUserData->GetBaseData() : nullptr;
                    if (armorData && spr)
                    {
                        _armorBackSprite->SetColorAlpha(1.0f);
                        _armorBackSprite->Render(rc, textureRenderer);

                        // 防具スプライト描画
                        spr->SetColor(armorData->GetRarityColor(armorData->rarity));
                        spr->GetRectTransform().SetLocalPosition(position);
                        spr->GetRectTransform().UpdateTransform(&spriteRenderer->GetRectTransform(BoxBackSpr));
                        spr->Render(rc, textureRenderer);
                    }
                    else
                    {
                        _armorBackSprite->SetColorAlpha(0.5f);
                        _armorBackSprite->Render(rc, textureRenderer);
                    }
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

    // 防具説明テキスト描画
    _nameTextData.text = L"■";
    _rarityextData.text = L"レア度:";
    _defenseTextData.text = L"防御力:";
    std::unordered_map<SkillType, int>* skillData = nullptr;
    switch (_state)
    {
    case ChestArmorMenuController::State::SelectArmorType:
        switch (_selectType)
        {
        case ChestArmorMenuController::SelectType::Weapon:
        {
			int weaponIndex = playerEquipmentController->GetWeaponIndex();
            if (auto weaponData = userDataManager->GetAcquiredWeaponData(userDataManager->GetEquippedWeaponType(), weaponIndex))
            {
				_nameTextData.text += ToUtf16(weaponData->GetBaseData()->name);
				_defenseTextData.text = L"攻撃力:" + std::to_wstring(static_cast<int>(weaponData->GetBaseData()->attack));
				_rarityextData.text += std::to_wstring(weaponData->GetBaseData()->rarity);
            }
            break;
        }
        case ChestArmorMenuController::SelectType::Head:
        case ChestArmorMenuController::SelectType::Chest:
        case ChestArmorMenuController::SelectType::Arm:
        case ChestArmorMenuController::SelectType::Waist:
        case ChestArmorMenuController::SelectType::Leg:
        {
            ArmorType type = static_cast<ArmorType>(static_cast<int>(_selectType) - 1);
            int armorIndex = playerEquipmentController->GetArmorIndex(type);
            if (armorIndex != -1)
            {
                if (auto armorData = userDataManager->GetAcquiredArmorData(type, armorIndex))
                {
                    _nameTextData.text += ToUtf16(armorData->GetBaseData()->name);
                    _defenseTextData.text += std::to_wstring(static_cast<int>(armorData->GetBaseData()->defense));
                    _rarityextData.text += std::to_wstring(armorData->GetBaseData()->rarity);
                    skillData = &armorData->GetBaseData()->skills;
                }
            }
            break;
        }
        }
        break;
    case ChestArmorMenuController::State::SelectArmor:
        switch (_selectType)
        {
        case ChestArmorMenuController::SelectType::Weapon:
        {
            int weaponIndex = _selectArmorRowIndex * _armorSprColumnNum + _selectArmorColumnIndex;
            if (auto weaponData = userDataManager->GetAcquiredWeaponData(userDataManager->GetEquippedWeaponType(), weaponIndex))
            {
                _nameTextData.text += ToUtf16(weaponData->GetBaseData()->name);
                _defenseTextData.text = L"攻撃力:" + std::to_wstring(static_cast<int>(weaponData->GetBaseData()->attack));
                _rarityextData.text += std::to_wstring(weaponData->GetBaseData()->rarity);
            }
            break;
        }
        case ChestArmorMenuController::SelectType::Head:
        case ChestArmorMenuController::SelectType::Chest:
        case ChestArmorMenuController::SelectType::Arm:
        case ChestArmorMenuController::SelectType::Waist:
        case ChestArmorMenuController::SelectType::Leg:
        {
            ArmorType type = static_cast<ArmorType>(static_cast<int>(_selectType) - 1);
            // 防具変更
            int armorIndex = _selectArmorRowIndex * _armorSprColumnNum + _selectArmorColumnIndex;
            // 選択中のマスに対応する防具が存在するか確認
            if (armorIndex < static_cast<int>(userDataManager->GetAcquiredArmorDataList(type).size()))
            {
                if (auto armorData = userDataManager->GetAcquiredArmorData(type, armorIndex))
                {
                    _nameTextData.text += ToUtf16(armorData->GetBaseData()->name);
                    _defenseTextData.text += std::to_wstring(static_cast<int>(armorData->GetBaseData()->defense));
                    _rarityextData.text += std::to_wstring(armorData->GetBaseData()->rarity);
                    skillData = &armorData->GetBaseData()->skills;
                }
            }
            break;
        }
        }
        break;
    }

	// テキスト描画
    if (!_nameTextData.text.empty())
    {
        textRenderer.Draw(_nameTextData);
        textRenderer.Draw(_rarityextData);
        textRenderer.Draw(_defenseTextData);
        textRenderer.Draw(_skillTextData);

        // スキル表示
        if (skillData)
        {
            float skillTextOffsetY = 55.0f;
            for (auto& [skillType, level] : *skillData)
            {
                if (auto data = skillManager->GetSkillData(skillType))
                {
					// レベル0のスキルは表示しない
					if (level <= 0)
						continue;
                    TextRenderer::TextDrawData skillData;
					skillData.text = L"　";
                    skillData.text += ToUtf16(data->name) + L":" + std::to_wstring(level);
                    skillData.position = Vector2(820.0f, 300.0f + skillTextOffsetY);
                    skillData.scale = Vector2(1.0f, 1.0f);
                    skillData.color = data->color;
                    textRenderer.Draw(skillData);
                    skillTextOffsetY += 55.0f;
                }
            }
        }
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
        ImGui::Separator();
		for (int i = 0; i <= static_cast<int>(WeaponType::WeaponTypeMax) - 1; ++i)
		{
			if (ImGui::TreeNode(ToString<WeaponType>(static_cast<size_t>(i)).c_str()))
			{
				_weaponSprites[i]->DrawGui();
				ImGui::TreePop();
			}
		}
        ImGui::Separator();
		for (int i = 0; i <= static_cast<int>(ArmorType::Leg); ++i)
		{
            if (ImGui::TreeNode(ToString<ArmorType>(static_cast<size_t>(i)).c_str()))
            {
                _armorSprites[i]->DrawGui();
                ImGui::TreePop();
            }
		}
        ImGui::Separator();
        if (ImGui::TreeNode(u8"装備前景"))
        {
			_armorFrontSprite->DrawGui();
			ImGui::TreePop();
        }

		ImGui::TreePop();
	}
    ImGui::Separator();

	_nameTextData.DrawGui(u8"防具名テキスト");
	_rarityextData.DrawGui(u8"レア度テキスト");
    _defenseTextData.DrawGui(u8"防御力テキスト");
    ImGui::Separator();

	ImGui::ColorEdit4(u8"選択中リムライト色", &_selectedRimLightColor.x);
}

// インデックス追加
void ChestArmorMenuController::AddIndex(int direction)
{
    bool up = (direction & static_cast<unsigned int>(ChestUIController::InputDirection::Up)) != 0;
    bool down = (direction & static_cast<unsigned int>(ChestUIController::InputDirection::Down)) != 0;
    bool left = (direction & static_cast<unsigned int>(ChestUIController::InputDirection::Left)) != 0;
    bool right = (direction & static_cast<unsigned int>(ChestUIController::InputDirection::Right)) != 0;

	// 状態確認
    if (_state != State::SelectArmorType)
    {
		if (left)
			AddSelectArmorColumnIndex(-1);
		else if (right)
			AddSelectArmorColumnIndex(1);

		if (up)
			AddSelectArmorRowIndex(-1);
		else if (down)
			AddSelectArmorRowIndex(1);

		return;
    }

	if (up)
        _selectType = static_cast<SelectType>(static_cast<int>(_selectType) - 1);
	else if (down)
        _selectType = static_cast<SelectType>(static_cast<int>(_selectType) + 1);

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
    auto playerEquipmentController = _PlayerEquipmentController.lock();
	if (!playerEquipmentController)
		return;

    size_t armorIndex = 0;
    ArmorType armorType = ArmorType::Head;
    switch (_selectType)
    {
    case SelectType::Weapon:
        armorIndex = playerEquipmentController->GetWeaponIndex();
        break;
    case SelectType::Head:
    case SelectType::Chest:
    case SelectType::Arm:
    case SelectType::Waist:
    case SelectType::Leg:
        armorType = static_cast<ArmorType>(static_cast<int>(_selectType) - 1);
        armorIndex = playerEquipmentController->GetArmorIndex(armorType);
        break;
    }

    switch (_state)
    {
    case State::SelectArmorType:
		spriteRenderer->SetColorAlpha(BoxBackSpr, 0.5f);

		// 装備していない場合はインデックスを0にする
        if (armorIndex < 0)
			armorIndex = 0;

		// 現在選択中の装備にインデックスを合わせる
        _selectArmorColumnIndex = static_cast<int>(armorIndex % _armorSprColumnNum);
        _selectArmorRowIndex = static_cast<int>(armorIndex / _armorSprColumnNum);

		_state = State::SelectArmor;
        break;
    case State::SelectArmor:
        if (_selectType == SelectType::Weapon)
        {
			// 武器変更
			size_t weaponIndex = _selectArmorRowIndex * _armorSprColumnNum + _selectArmorColumnIndex;
			// 現在装備中の武器と同じなら処理しない
			if (weaponIndex == static_cast<size_t>(playerEquipmentController->GetWeaponIndex()))
				return;
			// 選択中のマスに対応する武器が存在するか確認
			else if (weaponIndex < static_cast<int>(userDataManager->GetAcquiredWeaponDataList(WeaponType::GreatSword).size()))
				playerEquipmentController->SetWeaponIndex(static_cast<int>(weaponIndex));
        }
        else
        {
            // 防具変更
            armorIndex = _selectArmorRowIndex * _armorSprColumnNum + _selectArmorColumnIndex;
            // 現在装備中の防具と同じなら脱ぐ
            if (armorIndex == static_cast<size_t>(playerEquipmentController->GetArmorIndex(armorType)))
                playerEquipmentController->SetArmorIndex(armorType, -1);
            // 選択中のマスに対応する防具が存在するか確認
            else if (armorIndex < static_cast<int>(userDataManager->GetAcquiredArmorDataList(armorType).size()))
                playerEquipmentController->SetArmorIndex(armorType, static_cast<int>(armorIndex));
        }
        break;
    }
}

// 前の状態へ戻る
bool ChestArmorMenuController::PreviousState()
{
    auto spriteRenderer = _spriteRenderer.lock();
    if (!spriteRenderer)
        return false;
    auto playerEquipmentController = _PlayerEquipmentController.lock();
    if (!playerEquipmentController)
        return false;

    switch (_state)
    {
    case State::SelectArmorType:
        // 戻るときに装備の上書きリムライトを消す
        playerEquipmentController->GetWeaponActor()->SetIsOverrideRimLight(false);
        for (int i = 0; i <= static_cast<int>(ArmorType::Leg); ++i)
        {
            playerEquipmentController->GetArmorActor(static_cast<ArmorType>(i))->SetIsOverrideRimLight(false);
        }

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

void ChestArmorMenuController::SetPlayerEquipmentController(const std::shared_ptr<PlayerEquipmentController>& controller)
{
	if (!controller)
		return;
    auto spriteRenderer = _spriteRenderer.lock();
    if (!spriteRenderer)
        return;

    _PlayerEquipmentController = controller;

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
