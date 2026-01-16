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
	// レイヤー取得
	CollisionLayer GetLayer() const { return _layer; }
	// レイヤー設定
	void SetLayer(CollisionLayer layer) { this->_layer = layer; }
	// レイヤーマスク取得
	CollisionLayerMask GetLayerMask() const { return _layerMask; }
	// レイヤーマスク確認
	bool CheckMask(CollisionLayer layer) const
	{
		return (_layerMask & static_cast<CollisionLayerMask>(layer)) != 0;
	}
	// レイヤーマスク設定
	void SetLayerMask(CollisionLayerMask layerMask) { _layerMask = layerMask; }
	// 指定のレイヤーをマスクに追加
	void AddLayerMask(CollisionLayer layer)
	{
		_layerMask |= static_cast<CollisionLayerMask>(layer);
	}
	// 指定のレイヤーをマスクから削除
	void RemoveLayerMask(CollisionLayer layer)
	{
		_layerMask &= ~static_cast<CollisionLayerMask>(layer);
	}
	// 指定のレイヤー以外をマスクに追加
	void AddLayerMaskExcept(CollisionLayer layer)
	{
		_layerMask |= ~static_cast<CollisionLayerMask>(layer);
	}
	// コライダーの有効状態取得
	bool IsActive() const { return _isActive; }
	// コライダーの有効状態設定
	void SetActive(bool isActive) { _isActive = isActive; }
	// コライダーのトリガー状態取得
	bool IsTrigger() const { return _isTrigger; }
	// コライダーのトリガー状態設定
	void SetTrigger(bool isTrigger) { _isTrigger = isTrigger; }

#pragma region 入出力
	// ファイル読み込み(普段は使わない)
	bool LoadFromFile() override { return false; }
	// ファイル保存(普段は使わない)
	bool SaveToFile() override { return false; }
	// ファイル読み込み
	void LoadFromFile(nlohmann::json* json);
	// ファイル保存
	void SaveToFile(nlohmann::json* json);

	// JSON読み込み
	virtual void OnLoad(nlohmann::json* json) {}
	// JSON保存
	virtual void OnSave(nlohmann::json* json) {}
#pragma endregion

private:
	// レイヤー
	CollisionLayer	_layer = CollisionLayer::None;
	// マスク
	CollisionLayerMask _layerMask = CollisionLayerMaskAll;

	bool			_isActive = true; // 有効か
	bool			_isTrigger = false; // トリガーか
};