#pragma once

#include "../../Library/Resource/ResourceManager.h"
#include "WeaponData.h"

class WeaponManager : public ResourceBase
{
public:
	WeaponManager() = default;
	~WeaponManager() override {}

	// 初期化処理
	bool Initialize();

	// 名前取得
	std::string GetName() const override { return "WeaponManager"; }
	// ファイルパス取得
	std::string GetFilePath() const override { return _filePath; };
	// ファイル読み込み
	bool LoadFromFile() override;
	// ファイル保存
	bool SaveToFile() override;
	// Gui描画
	void DrawGui() override;
	// 武器データ取得
	std::vector<WeaponData>& GetWeaponDataList(WeaponType type) { return _weaponDataMap[type]; }
	// 武器データ取得
	WeaponData* GetWeaponData(WeaponType type, size_t index);

private:
	std::string _filePath = "./Data/Resource/WeaponManager.json";
	std::unordered_map<WeaponType, std::vector<WeaponData>> _weaponDataMap;
};

// リソース設定
_REGISTER_RESOURCE(WeaponManager)
