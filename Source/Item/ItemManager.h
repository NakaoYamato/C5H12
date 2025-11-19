#pragma once

#include "../../Library/Resource/ResourceManager.h"
#include "../../Library/2D/Canvas.h"
#include "../../Library/Graphics/Texture.h"
#include "ItemData.h"
#include "ItemFunctions.h"

class ItemManager : public ResourceBase
{
public:
	ItemManager() = default;
	~ItemManager() override {}

	// 初期化処理
	bool Initialize() override;

	// 名前取得
	std::string GetName() const override { return "ItemManager"; }
	// ファイルパス取得
	std::string GetFilePath() const override { return _filePath; };
	// ファイル読み込み
	bool LoadFromFile() override;
	// ファイル保存
	bool SaveToFile() override;

	// Gui描画
	void DrawGui() override;
	// アイテムアイコンGui描画
	void DrawItemIconGui(int selectedIconIndex, int overlayIconIndex, const Vector4& color);

	// アイテムデータ取得
	std::vector<ItemData>& GetItemDataList() { return _itemDataList; }
	// アイテムデータ取得
	ItemData* GetItemData(size_t index);
	// アイテムアイコンテクスチャインデックス取得
	int GetItemIconTextureIndex() const { return _itemIconTextureIndex; }
	// キャンバス取得
	Canvas* GetItemIconCanvas() { return _itemIconCanvas.get(); }
	// アイコン番号からUV取得
	Canvas::TextureData* GetItemIconTextureData(int iconIndex);
	// オーバーレイアイコンテクスチャ取得
	Texture* GetOverlayIconTexture() { return &_overlayIconTexture; }
	// オーバーレイアイコンテクスチャのUV取得
	Canvas::TextureData GetOverlayIconTextureData(int overlayIconIndex);

	// アイテム効果処理取得
	ItemFunctionBase* GetItemFunction(int index)
	{
		if (index >= 0 && index < static_cast<int>(_itemFunctionList.size()))
		{
			return _itemFunctionList[index].get();
		}
		return nullptr;
	}

private:
	std::string _filePath = "./Data/Resource/ItemManager.json";
	std::unique_ptr<Canvas> _itemIconCanvas;
	std::unordered_map<int, Canvas::TextureData> _itemIconTextureMap;
	int _itemIconTextureIndex = 0;
	Texture _overlayIconTexture;

	// アイテムデータリスト
	std::vector<ItemData> _itemDataList;

	// アイテム効果処理リスト
	std::vector<std::unique_ptr<ItemFunctionBase>> _itemFunctionList;

	int _selectedAddTypeIndex = 0;
};

// リソース設定
_REGISTER_RESOURCE(ItemManager)
