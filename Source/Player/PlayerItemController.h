#pragma once

#include "../../Library/Component/Component.h"
#include "../../Source/InGame/UI/Item/ItemUIController.h"
#include "../../Source/InGame/UI/Operate/OperateUIController.h"

// 前方宣言
class PlayerController;

class PlayerItemController : public Component
{
public:
	PlayerItemController() {}
	~PlayerItemController() override {}
	// 名前取得
	const char* GetName() const override { return "PlayerItemController"; }
	// 開始処理
	void Start() override;
	// 更新処理
	void Update(float elapsedTime) override;
	// GUI描画
	void DrawGui() override;

	// アイテム効果処理
	ItemFunctionBase::State ExecuteItemFunction(float elapsedTime);

	// 開く
	void Open();
	// 閉じる
	void Close();
	// 使う
	bool Use();

	// 現在選択中のアイテムタイプを取得
	// ItemTypeMaxであれば選択中アイテムなし
	ItemType GetCurrentItemType() const;

	ItemData* GetUsingItemData() const { return _usingItemData; }

	bool IsClosed() const;
	bool IsOpen() const;

	void AddIndex(int addIndex);
private:
	std::weak_ptr<PlayerController> _playerController;
	std::weak_ptr<ItemUIController> _itemUIController;
	std::weak_ptr<UserDataManager> _userDataManager;
    std::weak_ptr<OperateUIController> _operateUIController;
	ItemFunctionBase* _function = nullptr;

	// 使用中のアイテムデータ
	ItemData* _usingItemData = nullptr;

	int _currentIndex = 0;
};