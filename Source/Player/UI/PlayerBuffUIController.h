#pragma once

#include "../../Library/Actor/UI/UIActor.h"
#include "../BuffController.h"

class PlayerBuffUIController : public Component
{
public:
	PlayerBuffUIController(std::shared_ptr<BuffController> buffController) :
		_buffController(buffController) {
	}
	~PlayerBuffUIController() override {}
	// 名前取得
	const char* GetName() const override { return "PlayerBuffUIController"; }
	// 開始処理
	void Start() override;
	// 更新処理
	void Update(float elapsedTime) override;
	// 3D描画後の描画処理
	void DelayedRender(const RenderContext& rc) override;
	// GUI描画
	void DrawGui() override;

private:
	std::weak_ptr<BuffController> _buffController;

	// バフアイコン
	Sprite _buffIconSprite;

	// バフアイコンの開始位置
	Vector2 _buffIconStartPos = Vector2(110.0f, 20.0f);
	// バフアイコンの間隔
	float _buffIconSpacing = 40.0f;
};
