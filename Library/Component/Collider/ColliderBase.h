#pragma once

#include "../Component.h"

/// <summary>
/// 当たり判定コンポーネント基底クラス
/// </summary>
class ColliderBase : public Component
{
public:
	// GUI描画
	void DrawGui() override;

	// 押し出し処理
	void ApplyExtrusion() {}

	// コライダーの有効状態取得
	bool IsActive() const { return _isActive; }
	// コライダーの有効状態設定
	void SetActive(bool isActive) { _isActive = isActive; }
	// コライダーのトリガー状態取得
	bool IsTrigger() const { return _isTrigger; }
	// コライダーのトリガー状態設定
	void SetTrigger(bool isTrigger) { _isTrigger = isTrigger; }
private:
	bool _isActive = true; // 有効か
	bool _isTrigger = false; // トリガーか
};