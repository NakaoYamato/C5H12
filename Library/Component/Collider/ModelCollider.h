#pragma once

#include "ColliderBase.h"
#include "../../Library/Model/ModelCollision.h"

class ModelCollider : public ColliderBase
{
public:
	ModelCollider() {}
	~ModelCollider() override {}
	// 名前取得
	const char* GetName() const override { return "ModelCollider"; }
	// 開始処理
	void Start() override;
	// 更新処理
	void Update(float elapsedTime) override;
	// デバッグ描画処理
	void DebugRender(const RenderContext& rc) override;
	// GUI描画
	void DrawGui() override;

	// モデル当たり判定情報取得
	const ModelCollision& GetModelCollision() const { return _modelCollision; }
private:
	// モデル当たり判定情報
	ModelCollision _modelCollision;
};