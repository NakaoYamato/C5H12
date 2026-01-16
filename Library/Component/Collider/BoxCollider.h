#pragma once

#include "ColliderBase.h"

class BoxCollider : public ColliderBase
{
public:
	BoxCollider() {}
	~BoxCollider() override {}
	// 名前取得
	const char* GetName() const override { return "BoxCollider"; }
	// 開始処理
	void Start() override;
	// デバッグ描画処理
	void DebugRender(const RenderContext& rc) override;
	// 削除時処理
	void OnDelete() override;
	// GUI描画
	void DrawGui() override;
#pragma region アクセサ
	Vector3 GetPosition() const { return _center; }
	Vector3 GetHalfSize() const { return _halfSize; }
	void SetPosition(const Vector3& position) { _center = position; }
	void SetRadius(Vector3 halfSize) { _halfSize = halfSize; }
#pragma endregion

#pragma region 入出力
	// ファイル読み込み
	bool LoadFromFile() override;
	// ファイル保存
	bool SaveToFile() override;
#pragma endregion

private:
	Vector3 _center = Vector3::Zero; // 中心座標
	Vector3 _halfSize = Vector3::One; // 半辺長
};