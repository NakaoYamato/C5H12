#include "FenceActor.h"

#include "../../Library/Scene/Scene.h"
#include "../../Library/Component/ModelRenderer.h"
#include "../../Library/Component/Collider/BoxCollider.h"

#include "GeteController.h"

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
        fence->AddComponent<ModelRenderer>();

        Vector3 positions[4] =
        {
            {0.0f, 105.0f,525.0f},
            {0.0f, 105.0f,-525.0f},
            {-325.0f, 105.0f,-840.0f},
            {-325.0f, 105.0f,840.0f},
        };
        Vector3 scales[4] =
        {
            {0.25f, 1.0f,3.3f},
            {0.25f, 1.0f,3.3f},
            {3.3f, 1.0f,0.25f},
            {3.3f, 1.0f,0.25f},
        };
        for (int i = 0; i < 4; ++i)
        {
            auto box = fence->AddCollider<BoxCollider>();
            box->SetLayer(CollisionLayer::Stage);
            box->SetLayerMask(GetCollisionLayerMaskExcept(CollisionLayer::Stage));
            box->SetPosition(positions[i]);
            box->SetRadius(scales[i]);
        }
    }
    // フェンスゲート生成
    {
        std::string nameL = std::string(GetName()) + "FenceGateL";
        auto fenceGateL = GetScene()->RegisterActor<Actor>(nameL, GetTag());
        fenceGateL->GetTransform().SetPositionZ(202.0f);
        fenceGateL->LoadModel("./Data/Model/Stage/Props/Fence/FenceGateL.fbx");
        fenceGateL->SetParent(this);
        fenceGateL->AddComponent<ModelRenderer>();
        fenceGateL->AddComponent<GeteController>();
        auto boxL = fenceGateL->AddCollider<BoxCollider>();
        boxL->SetLayer(CollisionLayer::Stage);
        boxL->SetLayerMask(GetCollisionLayerMaskExcept(CollisionLayer::Stage));
        boxL->SetPosition(Vector3(0.0f, 85.0f, -105.0f));
        boxL->SetRadius(Vector3(0.15f, 0.5f, 0.9f));

        std::string nameR = std::string(GetName()) + "FenceGateR";
        auto fenceGateR = GetScene()->RegisterActor<Actor>(nameR, GetTag());
        fenceGateR->GetTransform().SetPositionZ(-202.0f);
        fenceGateR->LoadModel("./Data/Model/Stage/Props/Fence/FenceGateR.fbx");
        fenceGateR->SetParent(this);
        fenceGateR->AddComponent<ModelRenderer>();
        fenceGateR->AddComponent<GeteController>();
        auto boxR = fenceGateR->AddCollider<BoxCollider>();
        boxR->SetLayer(CollisionLayer::Stage);
        boxR->SetLayerMask(GetCollisionLayerMaskExcept(CollisionLayer::Stage));
        boxR->SetPosition(Vector3(0.0f, 85.0f, 105.0f));
        boxR->SetRadius(Vector3(0.15f, 0.5f, 0.9f));
    }
}
