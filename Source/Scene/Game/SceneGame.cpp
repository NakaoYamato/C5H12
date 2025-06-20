#include "SceneGame.h"

#include "../../Library/Graphics/Graphics.h"

// ネットワーク有効フラグ
bool SceneGame::NetworkEnabled = false;
// ネットワークアドレス
std::string SceneGame::NetworkAddress = "127.0.0.1";

void SceneGame::OnInitialize()
{
    ID3D11Device* device = Graphics::Instance().GetDevice();

    SetSkyMap(L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/sheen_pmrem.dds",
        L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/diffuse_iem.dds",
        L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/specular_pmrem.dds");
}
