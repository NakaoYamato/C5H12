#include "SceneUIDebug.h"

#include "../../Library/Actor/UI/UIActor.h"
#include "../../Library/Actor/UI/Menu/MenuUIActor.h"

//èâä˙âª
void SceneUIDebug::OnInitialize()
{
    SetSkyMap(L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/sheen_pmrem.dds",
        L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/diffuse_iem.dds",
        L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/specular_pmrem.dds");


	auto ui = RegisterActor<MenuUIActor>("UI", ActorTag::UI);
}
