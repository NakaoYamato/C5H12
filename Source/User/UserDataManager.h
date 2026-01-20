#pragma once

#include "../../Source/Item/ItemManager.h"
#include "../../Source/Armor/ArmorManager.h"
#include "../../Source/Weapon/WeaponManager.h"
#include "../../Source/Quest/QuestManager.h"
#include "../../Source/Enemy/EnemyDataManager.h"

class UserDataManager : public ResourceBase
{
public:
#pragma region 武器ユーザーデータ
	struct WeaponUserData
	{
		WeaponType	type = WeaponType::GreatSword;	// 武器の種類
		int			index = 0;						// 武器データインデックス
		float		acquisitionTime = 0.0f;			// 武器入手時刻
		int			level = 1;						// 武器レベル
		// マネージャーから元データ取得
		inline WeaponData* GetBaseData() const;
		// Gui描画
		inline void DrawGui();
	};
#pragma endregion

#pragma region 防具ユーザーデータ
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
#pragma endregion

#pragma region アイテムユーザーデータ
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
	// ポーチ内アイテムデータ
	struct PouchItemData
	{
		int			pouchIndex	= 0;	// ポーチ内インデックス
		int			itemIndex	= -1;	// アイテムデータインデックス
		int 		quantity	= 0;	// アイテム所持数
	};
#pragma endregion

#pragma region クエストユーザーデータ
	struct QuestUserData
	{
		int			index		= 0;	// クエストデータインデックス
		int			orderCount	= 0;	// クエスト受注回数
		int			clearCount	= 0;	// クエストクリア回数
		float		bestClearTime = 0.0f; // 最速クリアタイム(s)

		// マネージャーから元データ取得
		inline QuestData* GetBaseData() const;
		// Gui描画
		inline void DrawGui();
	};
#pragma endregion

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

#pragma region 武器
	// 武器データ取得
	WeaponUserData* GetAcquiredWeaponData(WeaponType type, int index);
	// 装備中の武器データ取得
	WeaponUserData* GetEquippedWeaponData();
	// 所持している武器データリスト取得
	std::vector<WeaponUserData>& GetAcquiredWeaponDataList(WeaponType type);
	// 装備中の武器タイプ取得
	WeaponType GetEquippedWeaponType() const;
	// 装備中の武器タイプ変更
	void SetEquippedWeaponType(WeaponType type);
	// 装備中の武器インデックス取得
	int GetEquippedWeaponIndex() const;
	// 装備中の武器インデックス変更
	void SetEquippedWeaponIndex(int index);
#pragma endregion


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
	// 特定のアイテムを所持しているポーチ取得
	PouchItemData* GetPouchItemFromItemIndex(int itemIndex);

	// アイテムポーチ内のアイテムインデックス変更
	void SetPouchItemIndex(int pouchIndex, int itemIndex);
#pragma endregion

#pragma region クエスト
	// クエスト受注回数増加
	void IncreaseQuestOrderCount(int questIndex);
	// クエストクリア回数増加
	void IncreaseQuestClearCount(int questIndex);
	// クエスト最速クリアタイム更新
	void UpdateQuestBestClearTime(int questIndex, float time);

	// 受注カウント取得
	int GetQuestOrderCount(int questIndex);
	// クリアカウント取得
	int GetQuestClearCount(int questIndex);
	// 最速クリアタイム取得
	float GetQuestBestClearTime(int questIndex);

	// 所持しているクエストデータリスト取得
	std::unordered_map<int, QuestUserData>& GetQuestUserDataMap() { return _questUserDataMap; }
#pragma endregion


private:
	// 武器Gui描画
	void DrawWeaponGui();
	// 防具Gui描画
	void DrawAromrGui();
	// アイテムGui描画
	void DrawItemGui();
	// クエストGui描画
	void DrawQuestGui();

private:
	std::string _filePath = "./Data/Resource/UserData/UserDataManager.json";

	// 所持しているすべての武器
	std::unordered_map<WeaponType, std::vector<WeaponUserData>> _acquiredWeaponMap;
	// 所持しているすべての防具
	std::unordered_map<ArmorType, std::vector<ArmorUserData>> _acquiredArmorMap;
	// すべてのアイテムの所持状況
	std::unordered_map<int, ItemUserData> _acquiredItemMap;

	// 装備中の武器タイプ
	WeaponType _equippedWeaponType = WeaponType::GreatSword;
	// 装備中の武器インデックス
	int _equippedWeaponIndex = 0;

	// 装備中の防具インデックス
	int _equippedArmorIndices[static_cast<int>(ArmorType::Leg) + 1] = { -1, -1, -1, -1, -1 };

	// アイテムポーチ内のアイテム
	PouchItemData _pouchItems[MaxPouchItemCount];
	// 空のポーチアイテムデータ
	PouchItemData _emptyPouchItemData;

	// 所持しているすべてのクエストデータ
	std::unordered_map<int, QuestUserData> _questUserDataMap;
	// 現在の受注履歴
	std::vector<int> _currentAcceptedQuestHistory;
};

// リソース設定
_REGISTER_RESOURCE(UserDataManager)
