#pragma once

#include "../../Library/Component/Component.h"

#include "../../Source/InGame/UI/Item/ItemUIController.h"

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

	// 開く
	void Open();
	// 閉じる
	void Close();

	bool IsClosed() const;
	bool IsOpen() const;

	void AddIndex(int addIndex) { _currentIndex += addIndex; }
private:
	std::weak_ptr<ItemUIController> _itemUIController;

	std::weak_ptr<UserDataManager> _userDataManager;

	int _currentIndex = 0;
};