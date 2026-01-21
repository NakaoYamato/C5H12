#pragma once

#include "../../Library/Component/SpriteRenderer.h"
#include "../../Source/Quest/QuestOrderController.h"

class QuestUIController : public Component
{
public:
	QuestUIController() = default;
	~QuestUIController() override {}
	// 名前取得
	const char* GetName() const override { return "QuestUIController"; }
	// 開始処理
	void Start() override;
	// 更新処理
	void Update(float elapsedTime) override;
	// GUI描画
	void DrawGui() override;
	// 3D描画後の描画処理
	void DelayedRender(const RenderContext& rc) override;

private:
	std::weak_ptr<SpriteRenderer> _spriteRenderer;
	std::weak_ptr<QuestOrderController> _questOrderController;

	const std::string Front = "FrontSpr";

	// タイトル名位置
	Vector2 _titlePosition = Vector2(45.0f, 20.0f);
	// タイトル名色
	Vector4 _titleColor = Vector4::White;
	// タイトル名スケール
	Vector2 _titleScale = Vector2::One;

	// ロード進捗文位置
	Vector2 _loadProgressPosition = Vector2(-50.0f, 80.0f);
	// ロード進捗文スケール
	Vector2 _loadProgressScale = Vector2(0.7f, 0.7f);
};