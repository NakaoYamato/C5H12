#pragma once

#include "../../Source/Item/ItemManager.h"
#include "../../Source/Armor/ArmorManager.h"

class UserDataManager : public ResourceBase
{
public:
	struct ArmorUserData
	{
		ArmorType	type			= ArmorType::Head;	// 防具の種類
		int			index			= 0;				// 防具データインデックス
		float		acquisitionTime = 0.0f;				// 防具入手時刻
		int			level			= 1;				// 防具レベル
		int			experience		= 0;				// 防具経験値
		Vector4		color			= Vector4::White;	// 防具カラー

		inline ArmorData* GetBaseData() const;
		inline void DrawGui();
	};

	struct ItemUserData
	{
		int			index			= 0;	// アイテムデータインデックス
		int			quantity		= 1;	// アイテム所持数
		float		acquisitionTime = 0.0f;	// アイテム入手時刻

		inline ItemData* GetBaseData() const;
		inline void DrawGui();
	};

	static constexpr int MaxPouchItemCount = 20;

public:
	UserDataManager() = default;
	~UserDataManager() override {}
	// 名前取得
	std::string GetName() const override { return "UserDataManager"; }
	// ファイルパス取得
	std::string GetFilePath() const override { return _filePath; };
	// ファイル読み込み
	bool LoadFromFile() override;
	// ファイル保存
	bool SaveToFile() override;
	// Gui描画
	void DrawGui() override;

#pragma region 防具
	// 防具データ取得
	ArmorUserData* GetAcquiredArmorData(ArmorType type, int index);
	// 装備中の防具データ取得
	ArmorUserData* GetEquippedArmorData(ArmorType type);
	// 所持している防具データリスト取得
	std::vector<ArmorUserData>& GetAcquiredArmorDataList(ArmorType type);

	// 装備中の防具インデックス取得
	int GetEquippedArmorIndex(ArmorType type) const;
	// 装備中の防具インデックス変更
	void SetEquippedArmorIndex(ArmorType type, int index);
#pragma endregion

private:
	std::string _filePath = "./Data/Resource/UserData/UserDataManager.json";

	// 所持しているすべての防具
	std::unordered_map<ArmorType, std::vector<ArmorUserData>> _acquiredArmorMap;
	// 所持しているすべてのアイテム
	std::vector<ItemUserData> _acquiredItemList;

	// 装備中の防具インデックス
	int _equippedArmorIndices[static_cast<int>(ArmorType::Leg) + 1] = { -1, -1, -1, -1, -1 };

	// アイテムポーチ内のアイテムインデックス
	int _itemPouchIndices[MaxPouchItemCount] = { -1 };
};

// リソース設定
_REGISTER_RESOURCE(UserDataManager)
