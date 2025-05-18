#include "DummyActor.h"

#include "../../Library/Scene/Scene.h"

// ¶¬Žžˆ—
void DummyActor::OnCreate()
{
    GetTransform().SetLengthScale(0.01f);
    GetTransform().SetScale(2.0f);

    _shapeController = this->AddComponent<ShapeController>();
    _shapeController.lock()->SetType(ShapeType::Capsule);
    auto collider = this->AddCollider<CapsuleCollider>();
}

// XVŽžˆ—
void DummyActor::OnUpdate(float elapsedTime)
{
    _shapeController.lock()->SetColor(Vector4::White);
}

/// ÚGŽžˆ—
void DummyActor::OnContact(CollisionData& collisionData)
{
    _shapeController.lock()->SetColor(Vector4::Red);
}
