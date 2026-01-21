#include "FenceActor.h"

#include "../../Library/Scene/Scene.h"
#include "../../Library/Component/ModelRenderer.h"
#include "../../Library/Component/Collider/BoxCollider.h"

#include "GeteController.h"

#include "../../Source/Common/InteractionController.h"

// 生成時処理
void FenceActor::OnCreate()
{
    GetTransform().SetLengthScale(0.01f);

    // フェンス生成
    {
        std::string name = std::string(GetName()) + "FencePart";
        auto fence = GetScene()->RegisterActor<Actor>(name, GetTag());
        fence->LoadModel("./Data/Model/Stage/Props/Fence/Fence.fbx");
        fence->SetParent(this);
        auto modelRenderer = fence->AddComponent<ModelRenderer>();
        modelRenderer->SetRenderType(ModelRenderType::Static);

        Vector3 positions[4] =
        {
            {0.0f, 105.0f,525.0f},
            {0.0f, 105.0f,-525.0f},
            {-325.0f, 105.0f,-840.0f},
            {-325.0f, 105.0f,840.0f},
        };
        Vector3 scales[4] =
        {
            {0.25f, 10.0f,3.3f},
            {0.25f, 10.0f,3.3f},
            {3.3f, 10.0f,0.25f},
            {3.3f, 10.0f,0.25f},
        };
        for (int i = 0; i < 4; ++i)
        {
            auto box = fence->AddCollider<BoxCollider>();
        }
    }
    // フェンスゲート生成
    {
		auto CreateFenceGate = [&](const char* name, const char* modelPath, float positionZ)
			{
				auto fenceGate = GetScene()->RegisterActor<Actor>(name, GetTag());
				fenceGate->GetTransform().SetPositionZ(positionZ);
				fenceGate->LoadModel(modelPath);
				fenceGate->SetParent(this);
				auto modelRenderer = fenceGate->AddComponent<ModelRenderer>();
				modelRenderer->SetRenderType(ModelRenderType::Static);
				fenceGate->AddComponent<GeteController>();
                auto interactionController = fenceGate->AddComponent<InteractionController>();
				interactionController->RegisterIsUsableCallback(
					"GeteController",
					[&](Actor* target) -> bool
					{
						// クエスト受注中なら使用可能
						auto gameManager = target->GetScene()->GetActorManager().FindByName("GameManager", ActorTag::System);
						if (!gameManager)
							return false;
						auto questOrderController = gameManager->GetComponent<QuestOrderController>();
						if (!questOrderController)
							return false;
						if (questOrderController->GetCurrentState() != QuestOrderController::State::Accepted)
							return false;

						return questOrderController->CanStartQuest();
					}
				);
				interactionController->RegisterOnSelectCallback(
					"GeteController",
					[&](Actor* target)
					{
						// 頭上にテキスト表示
						TextRenderer::Text3DDrawData drawData;
						drawData.type = FontType::MSGothic;
						if (Input::Instance().GetCurrentInputDevice() == Input::InputType::Keyboard)
							drawData.text = u8"Fキーでクエスト開始";
						else
							drawData.text = u8"Aボタンでクエスト開始";
						drawData.position = target->GetTransform().GetWorldPosition() + Vector3{ 0.0f, 2.0f, 0.0f };
						drawData.color = Vector4::Yellow;
						target->GetScene()->GetTextRenderer().Draw3D(drawData);
					}
				);
				interactionController->RegisterOnUseCallback(
					"GeteController",
					[&](Actor* target)
					{
						// クエスト開始
						auto gameManager = target->GetScene()->GetActorManager().FindByName("GameManager", ActorTag::System);
						if (!gameManager)
							return;
						auto questOrderController = gameManager->GetComponent<QuestOrderController>();
						if (!questOrderController)
							return;
						questOrderController->StartQuest();
					}
				);
				auto box = fenceGate->AddCollider<BoxCollider>();
				auto boxTrigger = fenceGate->AddCollider<BoxCollider>();
				boxTrigger->SetTrigger(true);
			};
		CreateFenceGate((std::string(GetName()) + "FenceGateL").c_str(), "./Data/Model/Stage/Props/Fence/FenceGateL.fbx", 202.0f);
		CreateFenceGate((std::string(GetName()) + "FenceGateR").c_str(), "./Data/Model/Stage/Props/Fence/FenceGateR.fbx", -202.0f);
    }
}
