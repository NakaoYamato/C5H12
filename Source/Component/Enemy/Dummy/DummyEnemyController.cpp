#include "DummyEnemyController.h"

// 開始処理
void DummyEnemyController::Start()
{
	EnemyController::Start();
	_shapeController = GetActor()->GetComponent<ShapeController>();
}

// 更新処理
void DummyEnemyController::Update(float elapsedTime)
{
	EnemyController::Update(elapsedTime);
	_shapeController.lock()->SetColor(Vector4::White);
}

// ダメージを与える
void DummyEnemyController::AddDamage(float damage, Vector3 hitPosition)
{
	EnemyController::AddDamage(damage, hitPosition);
	_shapeController.lock()->SetColor(Vector4::Red);
}
