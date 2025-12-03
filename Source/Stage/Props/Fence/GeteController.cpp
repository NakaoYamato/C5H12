#include "GeteController.h"

#include "../../Library/Component/CharactorController.h"

#include <imgui.h>

// ŠJŽnŽžˆ—
void GeteController::Start()
{
    _boxCollider = GetActor()->GetCollider<BoxCollider>();
}

// XVˆ—
void GeteController::Update(float elapsedTime)
{
    // ƒQ[ƒg‰ñ“]
    float angleY = DirectX::XMConvertToDegrees(GetActor()->GetTransform().GetAngle().y);

    _angularVelocityY += _forceY;

    // Œ³‚É–ß‚é—Í
    float invCos = MathF::Clamp01(1.0f - std::cos(GetActor()->GetTransform().GetAngle().y));
    if (angleY > 0.0f)
    {
        _angularVelocityY += invCos * -_returnForce * elapsedTime;
    }
    else if (angleY < 0.0f)
    {
        _angularVelocityY += invCos * +_returnForce * elapsedTime;
    }

    // ‰ñ“]‚Ì–€ŽC
    _angularVelocityY += -_angularVelocityY * _friction * elapsedTime;
    // Šp‘¬“x‚©‚çŠp“xXV
    angleY += _angularVelocityY * elapsedTime;
    // Šp“x§ŒÀ
    if (angleY > +90.0f)
    {
        angleY = +90.0f;
        _angularVelocityY = 0.0f;
    }
    else if (angleY < -90.0f)
    {
        angleY = -90.0f;
        _angularVelocityY = 0.0f;
    }

    GetActor()->GetTransform().SetAngleY(DirectX::XMConvertToRadians(angleY));

    // —Í‚ðƒNƒŠƒA
    _forceY = {};
}

// GUI•`‰æ
void GeteController::DrawGui()
{
    ImGui::DragFloat(u8"–€ŽC—Í", &_friction);
    ImGui::DragFloat(u8"‰ñ“]•â³’l", &_rotationFactor);
    ImGui::DragFloat(u8"Œ³‚É–ß‚é—Í", &_returnForce);
    ImGui::Separator();
    ImGui::DragFloat(u8"—Í", &_forceY);
    ImGui::DragFloat(u8"Šp‘¬“x", &_angularVelocityY);
}

// ƒIƒuƒWƒFƒNƒg‚Æ‚ÌÚGŽž‚Ìˆ—
void GeteController::OnContact(CollisionData& collisionData)
{
    auto boxCollider = _boxCollider.lock();
    if (!boxCollider)
        return;
    // ƒLƒƒƒ‰ƒRƒ“ƒgƒ[ƒ‰[Žæ“¾
    auto charaCtrl = collisionData.other->GetComponent<CharactorController>();
    if (!charaCtrl)
        return;

    // ƒQ[ƒg‚ÌŒü‚«Žæ“¾
    Vector3 gateCenter = boxCollider->GetPosition().TransformCoord(GetActor()->GetTransform().GetMatrix());
    Vector3 toGate = gateCenter - GetActor()->GetTransform().GetWorldPosition();
    Vector3 toChar = collisionData.other->GetTransform().GetPosition() - GetActor()->GetTransform().GetWorldPosition();
    bool isHitLeft = (toGate.Cross(toChar).y > 0.0f);

    float factor = toChar.Length() * _rotationFactor;
    if (isHitLeft)
    {
        _forceY -= factor;
    }
    else
    {
        _forceY += factor;
    }
}
