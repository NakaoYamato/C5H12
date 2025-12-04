#include "PlayerEquipmentController.h"

#include "../../Library/Exporter/Exporter.h"
#include "../../Library/Algorithm/Converter.h"

#include <Mygui.h>

// 生成時処理
void PlayerEquipmentController::OnCreate()
{
	_damageable = GetActor()->GetComponent<Damageable>();
	_damageSender = GetActor()->GetComponent<DamageSender>();
	_userDataManager = ResourceManager::Instance().GetResourceAs<UserDataManager>("UserDataManager");
	_skillManager = ResourceManager::Instance().GetResourceAs<SkillManager>("SkillManager");
}

// 開始時処理
void PlayerEquipmentController::Start()
{
	// パラメータの再計算
	RecalculateParameters();
}

// 更新処理
void PlayerEquipmentController::Update(float elapsedTime)
{
	auto userDataManager = _userDataManager.lock();
	if (!userDataManager)
		return;

	// 防御力を設定
	float totalDefense = 0.0f;
	for (size_t i = 0; i <= static_cast<size_t>(ArmorType::Leg); ++i)
	{
		UserDataManager::ArmorUserData* userData = userDataManager->GetEquippedArmorData(static_cast<ArmorType>(i));
		if (userData)
		{
			totalDefense += userData->GetBaseData()->defense;
		}
	}
	if (auto damageable = _damageable.lock())
	{
		damageable->SetDefense(totalDefense);
	}
}

// GUI描画
void PlayerEquipmentController::DrawGui()
{
	auto skillManager = _skillManager.lock();
	if (!skillManager)
		return;

	if (ImGui::Button(u8"保存"))
	{
		SaveArmorData();
	}
	ImGui::Separator();
	ImGui::Text(u8"基礎攻撃力: %.2f", _baseAttack);
	ImGui::Text(u8"総合攻撃力: %.2f", _totalAttack);
	ImGui::Separator();
	ImGui::Text(u8"基礎防御力: %.2f", _baseDefense);
	ImGui::Text(u8"総合防御力: %.2f", _totalDefense);
	ImGui::Separator();
	for (auto& [skillType, level] : _currentSkills)
	{
		ImGui::Text(u8"スキル: %s レベル: %d", skillManager->GetSkillData(skillType)->name.c_str(), level);
	}
}

// 武器インデックス取得
int PlayerEquipmentController::GetWeaponIndex() const
{
	auto userDataManager = _userDataManager.lock();
	if (!userDataManager)
		return -1;

	return userDataManager->GetEquippedWeaponIndex();
}

// 武器インデックス設定
void PlayerEquipmentController::SetWeaponIndex(int index)
{
	auto userDataManager = _userDataManager.lock();
	if (!userDataManager)
		return;

	// データがあるか確認
	UserDataManager::WeaponUserData* userData = userDataManager->GetAcquiredWeaponData(WeaponType::GreatSword, index);

	// アクターが存在する場合は更新
	auto weaponActor = GetWeaponActor();
	if (weaponActor)
	{
		if (userData)
			weaponActor->BuildData(index);
		else
			weaponActor->BuildData(-1);
	}

	// パラメータの再計算
	RecalculateParameters();
}

// 防具インデックス取得
int PlayerEquipmentController::GetArmorIndex(ArmorType type) const
{
	auto userDataManager = _userDataManager.lock();
	if (!userDataManager)
		return -1;

	return userDataManager->GetEquippedArmorIndex(type);
}

// 防具インデックス設定
void PlayerEquipmentController::SetArmorIndex(ArmorType type, int index)
{
	auto userDataManager = _userDataManager.lock();
	if (!userDataManager)
		return;

	// データがあるか確認
	UserDataManager::ArmorUserData* userData = userDataManager->GetAcquiredArmorData(type, index);

	// アクターが存在する場合は更新
	auto armorActor = GetArmorActor(type);
	if (armorActor)
	{
		if (userData)
			armorActor->BuildData(userData->GetBaseData(), index);
		else
			armorActor->BuildData(nullptr, -1);
	}

	// パラメータの再計算
	RecalculateParameters();
}

// パラメータの再計算
void PlayerEquipmentController::RecalculateParameters()
{
	auto userDataManager = _userDataManager.lock();
	if (!userDataManager)
		return;
	auto skillManager = _skillManager.lock();
	if (!skillManager)
		return;

	_baseAttack = 0.0f;
	_baseDefense = 0.0f;
	_currentSkills.clear();
	if (auto weaponUserData = userDataManager->GetEquippedWeaponData())
	{
		// 攻撃力を更新
		_baseAttack += weaponUserData->GetBaseData()->attack;
	}
	for (size_t i = 0; i <= static_cast<size_t>(ArmorType::Leg); ++i)
	{
		UserDataManager::ArmorUserData* userData = userDataManager->GetEquippedArmorData(static_cast<ArmorType>(i));
		if (userData)
		{
			// 防御力を更新
			_baseDefense += userData->GetBaseData()->defense;
			// スキル情報更新
			for (auto& [skillType, level] : userData->GetBaseData()->skills)
			{
				_currentSkills[skillType] += level;
				_currentSkills[skillType] = std::clamp(
					_currentSkills[skillType],
					0,
					skillManager->GetSkillData(skillType)->maxLevel);
			}
		}
	}

	// ダメージ可能オブジェクトに防御力を設定
	if (auto damageable = _damageable.lock())
	{
		float defense = _baseDefense;
		// スキルによる防御力補正を適用
		int defenseLevel = 0;
		if (_currentSkills.find(SkillType::DefenseBoost) != _currentSkills.end())
		{
			defenseLevel = _currentSkills[SkillType::DefenseBoost];
        }
		switch (defenseLevel)
		{
		case 1: defense += 3.0f; break;
		case 2: defense += 5.0f; break;
		case 3: defense += 7.0f; break;
		case 4: defense = defense * 1.02f; defense += 8.0f; break;
		case 5: defense = defense * 1.04f; defense += 9.0f; break;
		}
		_totalDefense = defense;

		damageable->SetDefense(_totalDefense);
	}
	// ダメージ送信オブジェクトに攻撃力を設定
	if (auto damageSender = _damageSender.lock())
	{
		float attack = _baseAttack;
		// スキルによる攻撃力補正を適用
		int attackLevel = 0;
		if (_currentSkills.find(SkillType::AttackBoost) != _currentSkills.end())
		{
			attackLevel = _currentSkills[SkillType::AttackBoost];
		}
		switch (attackLevel)
		{
		case 1: attack += 3.0f; break;
		case 2: attack += 5.0f; break;
		case 3: attack += 7.0f; break;
		case 4: attack = attack * 1.02f; attack += 8.0f; break;
		case 5: attack = attack * 1.04f; attack += 9.0f; break;
		}
		_totalAttack = attack;

		damageSender->SetBaseATK(_totalAttack);
	}
}

void PlayerEquipmentController::SaveArmorData()
{
	auto userDataManager = _userDataManager.lock();
	if (!userDataManager)
		return;

	userDataManager->SaveToFile();
}
