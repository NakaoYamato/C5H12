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

		// マネージャーから元データ取得
		inline ArmorData* GetBaseData() const;
		// Gui描画
		inline void DrawGui();
	};

	struct ItemUserData
	{
		int			index			= 0;	// アイテムデータインデックス
		int			quantity		= 0;	// アイテム所持数
		float		acquisitionTime = 0.0f;	// アイテム入手時刻

		// マネージャーから元データ取得
		inline ItemData* GetBaseData() const;
		// Gui描画
		inline void DrawGui();
	};

	static constexpr int MaxPouchItemCount = 20;
	struct PouchItemData
	{
		int			pouchIndex	= 0;	// ポーチ内インデックス
		int			itemIndex	= -1;	// アイテムデータインデックス
		int 		quantity	= 0;	// アイテム所持数
	};

public:
	UserDataManager() = default;
	~UserDataManager() override {}
	// 名前取得
	std::string GetName() const override { return "UserDataManager"; }
	// ファイルパス取得
	std::string GetFilePath() const override { return _filePath; };

	// 初期化処理
	bool Initialize();

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

#pragma region アイテム
	// アイテムを使用する
	ItemFunctionBase* UseItem(int pouchIndex, Actor* user);
	// ポーチの整理
	void SortPouchItems();

	// アイテムポーチの最大所持数取得
	static int GetMaxPouchItemCount() { return MaxPouchItemCount; }
	// アイテムデータ取得
	ItemUserData* GetAcquiredItemData(int index);
	// アイテムの所持状況取得
	std::unordered_map<int, ItemUserData>& GetAcquiredItemDataMap() { return _acquiredItemMap; }
	// アイテムポーチ内の全アイテムインデックス取得
	PouchItemData* GetPouchItems() { return _pouchItems; }
	// アイテムポーチ内のアイテム取得
	PouchItemData* GetPouchItem(int pouchIndex);
	// アイテムポーチ内の最後のアイテムポーチ取得
	PouchItemData* GetLastPouchItem();

	// アイテムポーチ内のアイテムインデックス変更
	void SetPouchItemIndex(int pouchIndex, int itemIndex);
#pragma endregion

private:
	// 防具Gui描画
	void DrawAromrGui();
	// アイテムGui描画
	void DrawItemGui();

private:
	std::string _filePath = "./Data/Resource/UserData/UserDataManager.json";

	// 所持しているすべての防具
	std::unordered_map<ArmorType, std::vector<ArmorUserData>> _acquiredArmorMap;
	// すべてのアイテムの所持状況
	std::unordered_map<int, ItemUserData> _acquiredItemMap;

	// 装備中の防具インデックス
	int _equippedArmorIndices[static_cast<int>(ArmorType::Leg) + 1] = { -1, -1, -1, -1, -1 };

	// アイテムポーチ内のアイテム
	PouchItemData _pouchItems[MaxPouchItemCount];
	// 空のポーチアイテムデータ
	PouchItemData _emptyPouchItemData;
};

// リソース設定
_REGISTER_RESOURCE(UserDataManager)
