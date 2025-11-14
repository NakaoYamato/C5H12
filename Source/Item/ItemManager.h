#pragma once

#include "../../Library/Resource/ResourceManager.h"
#include "../../Library/2D/Canvas.h"
#include "../../Library/Graphics/Texture.h"
#include "ItemData.h"

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

	int GetItemIconTextureIndex() const { return _itemIconTextureIndex; }

private:
	std::string _filePath = "./Data/Resource/ItemManager.json";
	std::unique_ptr<Canvas> _itemIconCanvas;
	std::unordered_map<int, Canvas::TextureData> _itemIconTextureMap;
	int _itemIconTextureIndex = 0;
	Texture _overlayIconTexture;

	std::vector<ItemData> _itemDataList;

	int _selectedAddTypeIndex = 0;
};

// リソース設定
_REGISTER_RESOURCE(ItemManager)
