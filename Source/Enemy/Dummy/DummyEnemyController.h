#pragma once

#include "../EnemyController.h"
#include "../../Library/Component/ShapeController.h"

class DummyEnemyController : public EnemyController
{
public:
	DummyEnemyController() {}
	~DummyEnemyController() override {}
	// 名前取得
	const char* GetName() const override { return "DummyEnemyController"; }
	// 開始処理
	void Start() override;
	// 更新処理
	void Update(float elapsedTime) override;

	// ダメージを受けた時の処理
	void OnDamage(float damage, Vector3 hitPosition) override;

private:
	std::weak_ptr<ShapeController> _shapeController;
};