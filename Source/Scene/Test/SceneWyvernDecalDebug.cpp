#include "SceneWyvernDecalDebug.h"

#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Actor/Terrain/TerrainActor.h"
#include "../../Library/Actor/ModelAttach/ModelAttachActor.h"
// コンポーネント
#include "../../Library/Component/DecalController.h"

#include "../../Source/AI/MetaAI.h"
#include "../../Source/Player/PlayerActor.h"
#include "../../Source/Enemy/Wyvern/WyvernActor.h"

#include "../../InGame/InputManager.h"
#include "../../InGame/InGameCanvasActor.h"

void SceneWyvernDecalDebug::OnInitialize()
{
    static const float CompletionLoadingRate = 1.0f / 8.0f;
    SetSkyMap(L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/sheen_pmrem.dds",
        L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/diffuse_iem.dds",
        L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/specular_pmrem.dds");
    AddCompletionLoading(CompletionLoadingRate);

    {
        auto metaAI = RegisterActor<Actor>("MetaAI", ActorTag::System);
        metaAI->AddComponent<MetaAI>();
    }
    AddCompletionLoading(CompletionLoadingRate);
    {
        auto inputManager = RegisterActor<InputManager>("InputManager", ActorTag::System);
    }
    AddCompletionLoading(CompletionLoadingRate);
    {
        auto stage0 = RegisterActor<TerrainActor>("Stage0", ActorTag::Stage, "./Data/Terrain/Save/002.json", Vector3(0.0f, 0.0f, 0.0f));
    }
    AddCompletionLoading(CompletionLoadingRate);
    {
        auto player = RegisterActor<PlayerActor>("Player", ActorTag::Player);
    }
    AddCompletionLoading(CompletionLoadingRate);
    {
        auto decal = RegisterActor<Actor>("TestDecal", ActorTag::Player);
		decal->AddComponent<DecalController>(
            L"./Data/Texture/Decal/gun_holes.png",
            L"./Data/Texture/Decal/NormalMap.png");
    }
    AddCompletionLoading(CompletionLoadingRate);
    {
        auto canvas = RegisterActor<InGameCanvasActor>("Canvas", ActorTag::UI);
    }
    AddCompletionLoading(CompletionLoadingRate);
    {
        auto wyvern = RegisterActor<WyvernActor>("Wyvern", ActorTag::Enemy);
        wyvern->GetTransform().SetPosition(Vector3(0.0f, 5.0f, 20.0f));

        {
			auto decal = RegisterActor<ModelAttachActor>("WyvernDecal", ActorTag::Enemy);
            auto model = wyvern->GetModel().lock();
			decal->Initialize(wyvern.get(), &model->GetPoseNodes()[model->GetNodeIndex("L Clavicle")]);
            decal->AddComponent<DecalController>(
                L"./Data/Texture/Decal/gun_holes.png",
                L"./Data/Texture/Decal/NormalMap.png");
        }
    }
    AddCompletionLoading(CompletionLoadingRate);
}
