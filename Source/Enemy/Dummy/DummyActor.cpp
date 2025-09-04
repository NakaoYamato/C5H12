#include "DummyActor.h"

#include "../../Library/Scene/Scene.h"
#include "../../Library/Component/CharactorController.h"
#include "DummyEnemyController.h"

// ¶¬Žžˆ—
void DummyActor::OnCreate()
{
    EnemyActor::OnCreate();

    this->AddComponent<DummyEnemyController>();
    auto shapeController = this->AddComponent<ShapeController>();
    shapeController->SetType(ShapeType::Capsule);

    GetTransform().SetPosition(Vector3(0.0f, 3.0f, 5.0f));
    _charactorController.lock()->SetRadius(1.2f);
    _charactorController.lock()->SetStepOffset(0.0f);
    _charactorController.lock()->SetMass(100.0f);

    auto collider = this->AddCollider<CapsuleCollider>();
    collider->SetStart(Vector3(0.0f, 1.0f, 0.0f));
    collider->SetEnd(Vector3(0.0f, 3.0f, 0.0f));
    collider->SetRadius(1.2f);
    collider->SetLayer(CollisionLayer::Hit);
}

