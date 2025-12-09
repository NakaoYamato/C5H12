#include "SceneInstancingModelDebug.h"

#include "../../Library/Graphics/Graphics.h"

#include "../../Library/Component/InstancingModelRenderer.h"

void SceneInstancingModelDebug::OnInitialize()
{
    ID3D11Device* device = Graphics::Instance().GetDevice();

    SetSkyMap(L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/sheen_pmrem.dds",
        L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/diffuse_iem.dds",
        L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/specular_pmrem.dds");

    _model0 = std::make_shared<Model>(device, "./Data/Terrain/Environment/Bush/Bush01Re.fbx");
    _model1 = std::make_shared<Model>(device, "./Data/Terrain/Environment/Bush/Bush02Re.fbx");
    _model2 = std::make_shared<Model>(device, "./Data/Terrain/Environment/Bush/Bush03Re.fbx");

    for (int i = 0; i < 99; i++)
    {
		std::string name = "Model0" + std::to_string(i);
		auto actor = RegisterActor<Actor>(name.c_str(), ActorTag::Stage);
        actor->GetTransform().SetPositionX(static_cast<float>(i % 10) * 5.0f);
        actor->GetTransform().SetPositionZ(static_cast<float>(i / 10) * 5.0f);
		actor->GetTransform().SetAngleX(90.0f);
		auto renderer = actor->AddComponent<InstancingModelRenderer>(_model0);
    }
    for (int i = 0; i < 99; i++)
    {
		std::string name = "Model1" + std::to_string(i);
		auto actor = RegisterActor<Actor>(name.c_str(), ActorTag::Stage);
        actor->GetTransform().SetPositionY(5.0f);
        actor->GetTransform().SetPositionX(static_cast<float>(i % 10) * 5.0f);
        actor->GetTransform().SetPositionZ(static_cast<float>(i / 10) * 5.0f);
        actor->GetTransform().SetAngleX(90.0f);
		auto renderer = actor->AddComponent<InstancingModelRenderer>(_model1);
    }
    for (int i = 0; i < 99; i++)
    {
		std::string name = "Model2" + std::to_string(i);
		auto actor = RegisterActor<Actor>(name.c_str(), ActorTag::Stage);
        actor->GetTransform().SetPositionY(10.0f);
        actor->GetTransform().SetPositionX(static_cast<float>(i % 10) * 5.0f);
        actor->GetTransform().SetPositionZ(static_cast<float>(i / 10) * 5.0f);
        actor->GetTransform().SetAngleX(90.0f);
		auto renderer = actor->AddComponent<InstancingModelRenderer>(_model2);
    }
}
