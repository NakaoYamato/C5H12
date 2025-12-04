#pragma once

#include "../../Library/Component/Component.h"

#include "../../Source/User/UserDataManager.h"
#include "../../Source/Weapon/WeaponActor.h"
#include "../../Source/Armor/ArmorActor.h"
#include "../../Source/Common/Damageable.h"
#include "../../Source/Common/DamageSender.h"
#include "../../Source/Skill/SkillManager.h"

class PlayerEquipmentController : public Component
{
public:
	PlayerEquipmentController() {}
	~PlayerEquipmentController() override {}
	// 名前取得
	const char* GetName() const override { return "PlayerEquipmentController"; }
	// 生成時処理
	void OnCreate() override;
	// 開始時処理
	void Start() override;
	// 更新処理
	void Update(float elapsedTime) override;
	// GUI描画
	void DrawGui() override;

	// 武器インデックス取得
	int GetWeaponIndex() const;
	// 武器インデックス設定
	void SetWeaponIndex(int index);
	// 武器アクター取得
	std::shared_ptr<WeaponActor> GetWeaponActor() const { return _weaponActor.lock(); }
	// 武器アクター設定
	void SetWeaponActor(std::shared_ptr<WeaponActor> actor) { _weaponActor = actor; }
	// 防具インデックス取得
	int GetArmorIndex(ArmorType type) const;
	// 防具インデックス設定
	void SetArmorIndex(ArmorType type, int index);
	// 防具アクター取得
	std::shared_ptr<ArmorActor> GetArmorActor(ArmorType type) const { return _armorActors[static_cast<int>(type)].lock(); }
	// 防具アクター設定
	void SetArmorActor(ArmorType type, std::shared_ptr<ArmorActor> actor) { _armorActors[static_cast<int>(type)] = actor; }

    // 基礎攻撃力取得
    float GetBaseAttack() const { return _baseAttack; }
    // 総合攻撃力取得
    float GetTotalAttack() const { return _totalAttack; }
    // 基礎防御力取得
    float GetBaseDefense() const { return _baseDefense; }
    // 総合防御力取得
    float GetTotalDefense() const { return _totalDefense; }
    // 現在のスキル情報取得
    const std::unordered_map<SkillType, int>& GetCurrentSkills() const { return _currentSkills; }
private:
	// パラメータの再計算
	void RecalculateParameters();

#pragma region ファイル
	void SaveArmorData();
#pragma endregion

private:
	std::string _filePath = "./Data/Resource/PlayerArmorData.json";
	// 武器アクター参照
	std::weak_ptr<WeaponActor> _weaponActor;
	// 各部位のアクター参照
	std::weak_ptr<ArmorActor> _armorActors[static_cast<int>(ArmorType::Leg) + 1];
	// ユーザーデータマネージャー
	std::weak_ptr<UserDataManager> _userDataManager;
	// スキルマネージャー
	std::weak_ptr<SkillManager> _skillManager;
	std::weak_ptr<Damageable> _damageable;
	std::weak_ptr<DamageSender> _damageSender;

	// 基礎攻撃力
	float _baseAttack = 0.0f;
	// 総合攻撃力
	float _totalAttack = 0.0f;
	// 基礎防御力
	float _baseDefense = 0.0f;
	// 総合防御力
	float _totalDefense = 0.0f;

	// 現在のスキル情報
	std::unordered_map<SkillType, int> _currentSkills;
};