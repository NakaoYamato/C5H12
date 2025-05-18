#include "DummyActor.h"

#include "../../Library/Scene/Scene.h"
#include "../../Library/Component/CharactorController.h"
#include "../../Source/Component/Enemy/EnemyController.h"

// ¶¬Žžˆ—
void DummyActor::OnCreate()
{
    GetTransform().SetPosition(Vector3(0.0f, 3.0f, 5.0f));

    auto charactorController = this->AddComponent<CharactorController>();
    charactorController->SetRadius(1.2f);
    charactorController->SetStepOffset(0.0f);
    charactorController->SetMass(100.0f);
    _shapeController = this->AddComponent<ShapeController>();
    _shapeController.lock()->SetType(ShapeType::Capsule);
    auto collider = this->AddCollider<CapsuleCollider>();
    collider->SetStart(Vector3(0.0f, 1.0f, 0.0f));
    collider->SetEnd(Vector3(0.0f, 3.0f, 0.0f));
    collider->SetRadius(1.2f);
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
