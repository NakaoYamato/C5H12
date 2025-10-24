#include "SceneLookAtDebug.h"

#include "../../Library/Graphics/Graphics.h"

#include "../../Library/Component/Animator.h"

#include <imgui.h>

void SceneLookAtDebug::OnInitialize()
{
    ID3D11Device* device = Graphics::Instance().GetDevice();

    SetSkyMap(L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/sheen_pmrem.dds",
        L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/diffuse_iem.dds",
        L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/specular_pmrem.dds");

	auto modelActor = RegisterActor<Actor>(u8"Model", ActorTag::Stage);
	modelActor->LoadModel("./Data/Model/Dragons/Kuzar the Magnificent.fbx");
	_model = modelActor->GetModel();
    modelActor->GetTransform().SetLengthScale(0.01f);
    _modelRenderer = modelActor->AddComponent<ModelRenderer>();
    auto animator = modelActor->AddComponent<Animator>();

    _targetActor = RegisterActor<Actor>(u8"Target", ActorTag::Stage);

    // デバッグカメラをオンにする
    Debug::GetDebugInput()->buttonData |= DebugInput::BTN_F4;

    // グリッド表示
    SetShowGrid(true);
}

// 更新処理
void SceneLookAtDebug::OnUpdate(float elapsedTime)
{
    // 初期姿勢時の頭ノードのローカル空間前方向を求める
	if (_headLocalForward == Vector3::Zero)
    {
        auto model = _model.lock();
        int nodeIndex = model->GetNodeIndex("Head");
        DirectX::XMMATRIX headInvTransform = DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&model->GetPoseNodes()[nodeIndex].worldTransform));
        _headLocalForward = Vector3::TransformNormal(Vector3::Up, headInvTransform);
    }

    // 頭のノード取得
    auto model = _model.lock();
    int nodeIndex = model->GetNodeIndex("Head");
    auto& headNode = model->GetPoseNodes()[nodeIndex];
    auto headParentNode = model->GetPoseNodes()[nodeIndex].parent;

    // 頭のローカル空間上のターゲット位置を算出
    DirectX::XMMATRIX headInvTransform = DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&headNode.worldTransform));
    DirectX::XMMATRIX headParentInvTransform = DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&headParentNode->worldTransform));
    Vector3 targetLocalPosition = Vector3::TransformCoord(_targetActor.lock()->GetTransform().GetPosition(), headInvTransform);

    // ターゲット方向に向く
    //headNode.rotation = Quaternion::LookAt(headNode.position,
    //    Vector3::Up,
    //    targetLocalPosition);
    //headNode.rotation = Quaternion::LookAt(headNode.position,
    //    _headLocalForward,
    //    targetLocalPosition);
    //headNode.rotation = Quaternion::LookAt(
    //    Vector3::TransformCoord(headNode.position, headNode.worldTransform),
    //    _headLocalForward,
    //    _targetActor.lock()->GetTransform().GetPosition());
    headNode.rotation = Quaternion::LookAt(
        Vector3::Zero,
        Vector3::Up,
        Vector3::TransformCoord(_targetActor.lock()->GetTransform().GetPosition(), headParentInvTransform));

    // 行列更新
    model->UpdateNodeTransform(&headNode);

    if (ImGui::Begin(u8"デバッグ"))
    {
		ImGui::DragFloat3(u8"フロント", &_headLocalForward.x, 0.1f);
        _targetActor.lock()->GetTransform().DrawGui();
		ImGui::Text(u8"ターゲットローカル位置: (%.2f, %.2f, %.2f)",
            targetLocalPosition.x,
			targetLocalPosition.y,
			targetLocalPosition.z);
    }
	ImGui::End();
}
