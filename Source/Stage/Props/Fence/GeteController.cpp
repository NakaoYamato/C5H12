#include "GeteController.h"

#include "../../Library/Scene/Scene.h"
#include "../../Library/Component/CharactorController.h"
#include "../../Source/Quest/QuestOrderController.h"

#include <imgui.h>

// 開始時処理
void GeteController::Start()
{
    _boxCollider = GetActor()->GetCollider<BoxCollider>();

    // ゲームマネージャーからクエスト受注コントローラー取得
    auto gameManager = GetActor()->GetScene()->GetActorManager().FindByName("GameManager", ActorTag::System);
    if (gameManager)
    {
        if (auto cont = gameManager->GetComponent<QuestOrderController>())
        {
            _questOrderController = cont;
        }
    }
}

// 更新処理
void GeteController::Update(float elapsedTime)
{
    // クエスト受注中はゲートを閉じて固定する
	auto questOrderController = _questOrderController.lock();
    if (questOrderController && questOrderController->GetCurrentState() == QuestOrderController::State::Accepted)
    {
		GetActor()->GetTransform().SetAngleY(0.0f);

        return;
    }

    // ゲート回転
    float angleY = DirectX::XMConvertToDegrees(GetActor()->GetTransform().GetAngle().y);

    _angularVelocityY += _forceY;

    // 元に戻る力
    float invCos = MathF::Clamp01(1.0f - std::cos(GetActor()->GetTransform().GetAngle().y));
    if (angleY > 0.0f)
    {
        _angularVelocityY += invCos * -_returnForce * elapsedTime;
    }
    else if (angleY < 0.0f)
    {
        _angularVelocityY += invCos * +_returnForce * elapsedTime;
    }

    // 回転の摩擦
    _angularVelocityY += -_angularVelocityY * _friction * elapsedTime;
    // 角速度から角度更新
    angleY += _angularVelocityY * elapsedTime;
    // 角度制限
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

    // 力をクリア
    _forceY = {};
}

// GUI描画
void GeteController::DrawGui()
{
    ImGui::DragFloat(u8"摩擦力", &_friction);
    ImGui::DragFloat(u8"回転補正値", &_rotationFactor);
    ImGui::DragFloat(u8"元に戻る力", &_returnForce);
    ImGui::Separator();
    ImGui::DragFloat(u8"力", &_forceY);
    ImGui::DragFloat(u8"角速度", &_angularVelocityY);
}

// オブジェクトとの接触時の処理
void GeteController::OnContact(CollisionData& collisionData)
{
    // トリガーの接触
    if (collisionData.isTrigger)
    {
        // クエスト受注中に接触していてキー入力があればクエスト開始
        auto questOrderController = _questOrderController.lock();
        if (questOrderController && questOrderController->GetCurrentState() == QuestOrderController::State::Accepted)
        {
			if (_INPUT_TRIGGERD("Action1"))
			{
				questOrderController->StartQuest();
			}
        }

		return;
    }

    auto boxCollider = _boxCollider.lock();
    if (!boxCollider)
        return;
    // キャラコントローラー取得
    auto charaCtrl = collisionData.other->GetComponent<CharactorController>();
    if (!charaCtrl)
        return;

    // ゲートの向き取得
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
