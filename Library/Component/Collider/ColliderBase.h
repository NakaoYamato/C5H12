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
	/// <param name="other">接触対象</param>
	/// <param name="hitPosition">接触位置</param>
	/// <param name="hitNormal">接触対象から自身に向けての法線</param>
	/// <param name="penetration">めり込み量</param>
	virtual void Resolve(Actor* other,
		const Vector3& hitPosition,
		const Vector3& hitNormal,
		const float& penetration);

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