#include "ItemUIController.h"

// 開始処理
void ItemUIController::Start()
{
}

// 更新処理
void ItemUIController::Update(float elapsedTime)
{
}

// 3D描画後の描画処理
void ItemUIController::DelayedRender(const RenderContext& rc)
{
}

// GUI描画
void ItemUIController::DrawGui()
{
}

// 開く
void ItemUIController::Open(std::vector<int>* itemList, int currentIndex)
{
	_itemIndexListPtr = itemList;
	_currentIndex = currentIndex;
}

// 閉じる
void ItemUIController::Close()
{
}
