#pragma once

#include "../../Library/Component/SpriteRenderer.h"

class ItemUIController : public Component
{
public:
	ItemUIController() {}
	~ItemUIController() override {}
	// 名前取得
	const char* GetName() const override { return "ItemUIController"; }
	// 開始処理
	void Start() override;
	// 更新処理
	void Update(float elapsedTime) override;
	// 3D描画後の描画処理
	void DelayedRender(const RenderContext& rc) override;
	// GUI描画
	void DrawGui() override;

	// 開く
	void Open(std::vector<int>* itemList, int currentIndex);
	// 閉じる
	void Close();

	bool IsOpen() const { return _isOpen; }
private:
	std::vector<int>* _itemIndexListPtr = nullptr;
	int _currentIndex = 0;

	bool _isOpen = false;
};