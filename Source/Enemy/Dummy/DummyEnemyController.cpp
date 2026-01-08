#include "DummyEnemyController.h"

// 開始処理
void DummyEnemyController::Start()
{
	EnemyController::Start();
	_shapeController = GetActor()->GetComponent<ShapeController>();

	// ダメージを受けた時の処理
	_damageable.lock()->RegisterOnDamageCallback(
		"DummyEnemyController",
		[&](float damage, Vector3 hitPosition) -> void
		{
			_shapeController.lock()->SetColor(Vector4::Red);
		}
	);
}

// 更新処理
void DummyEnemyController::Update(float elapsedTime)
{
	EnemyController::Update(elapsedTime);
	_shapeController.lock()->SetColor(Vector4::White);
}
