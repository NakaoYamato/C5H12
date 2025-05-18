#include "DummyActor.h"

#include "../../Library/Scene/Scene.h"
#include "../../Source/Component/Enemy/EnemyController.h"
// ¶¬Žžˆ—
void DummyActor::OnCreate()
{
    GetTransform().SetScale(2.0f);
    GetTransform().SetPosition(Vector3(0.0f, 3.0f, 5.0f));

    _shapeController = this->AddComponent<ShapeController>();
    _shapeController.lock()->SetType(ShapeType::Capsule);
    auto collider = this->AddCollider<CapsuleCollider>();
    collider->SetStart(Vector3(0.0f, -1.0f, 0.0f));
    collider->SetEnd(Vector3(0.0f, 1.0f, 0.0f));
    collider->SetRadius(2.0f);
    collider->SetTrigger(true);
    this->AddComponent<EnemyController>();
}

// XVŽžˆ—
void DummyActor::OnUpdate(float elapsedTime)
{
    _shapeController.lock()->SetColor(Vector4::White);
}

/// ÚGŽžˆ—
void DummyActor::OnContact(CollisionData& collisionData)
{
    if (collisionData.otherLayer == "Attack")
    {
        _shapeController.lock()->SetColor(Vector4::Red);
    }
}
