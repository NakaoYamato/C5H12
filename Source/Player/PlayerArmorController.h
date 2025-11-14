#pragma once

#include "../../Library/Component/Component.h"

#include "../../Source/User/UserDataManager.h"
#include "../../Source/Armor/ArmorActor.h"

class PlayerArmorController : public Component
{
public:
	PlayerArmorController() {}
	~PlayerArmorController() override {}
	// 名前取得
	const char* GetName() const override { return "PlayerArmorController"; }
	// 生成時処理
	void OnCreate() override;
	// 更新処理
	void Update(float elapsedTime) override;
	// GUI描画
	void DrawGui() override;

	// 防具インデックス取得
	int GetArmorIndex(ArmorType type) const;
	// 防具インデックス設定
	void SetArmorIndex(ArmorType type, int index);
	// 防具アクター取得
	std::shared_ptr<ArmorActor> GetArmorActor(ArmorType type) const { return _armorActors[static_cast<int>(type)].lock(); }
	// 防具アクター設定
	void SetArmorActor(ArmorType type, std::shared_ptr<ArmorActor> actor) { _armorActors[static_cast<int>(type)] = actor; }
private:
#pragma region ファイル
	void SaveArmorData();
#pragma endregion

private:
	std::string _filePath = "./Data/Resource/PlayerArmorData.json";
	// 各部位のアクター参照
	std::weak_ptr<ArmorActor> _armorActors[static_cast<int>(ArmorType::Leg) + 1];
	// ユーザーデータマネージャー
	std::weak_ptr<UserDataManager> _userDataManager;
};