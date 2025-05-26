#include "DummyActor.h"

#include "../../Library/Scene/Scene.h"
#include "../../Library/Component/CharactorController.h"
#include "DummyEnemyController.h"

// ¶¬Žžˆ—
void DummyActor::OnCreate()
{
    GetTransform().SetPosition(Vector3(0.0f, 3.0f, 5.0f));

    auto charactorController = this->AddComponent<CharactorController>();
    auto shapeController = this->AddComponent<ShapeController>();
    this->AddComponent<DummyEnemyController>();

    charactorController->SetRadius(1.2f);
    charactorController->SetStepOffset(0.0f);
    charactorController->SetMass(100.0f);

    shapeController->SetType(ShapeType::Capsule);

    auto collider = this->AddCollider<CapsuleCollider>();
    collider->SetStart(Vector3(0.0f, 1.0f, 0.0f));
    collider->SetEnd(Vector3(0.0f, 3.0f, 0.0f));
    collider->SetRadius(1.2f);
}

