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

	/// <summary>
	/// 接触の解消処理
	/// </summary>
	/// <param name="collisionData">接触情報</param>
	virtual void OnContact(CollisionData& collisionData) override;

	CollisionLayer GetLayer() const { return _layer; }

	void SetLayer(CollisionLayer layer) { this->_layer = layer; }
	// コライダーの有効状態取得
	bool IsActive() const { return _isActive; }
	// コライダーの有効状態設定
	void SetActive(bool isActive) { _isActive = isActive; }
	// コライダーのトリガー状態取得
	bool IsTrigger() const { return _isTrigger; }
	// コライダーのトリガー状態設定
	void SetTrigger(bool isTrigger) { _isTrigger = isTrigger; }
private:
	// レイヤー
	CollisionLayer	_layer = CollisionLayer::None;
	bool			_isActive = true; // 有効か
	bool			_isTrigger = false; // トリガーか
};