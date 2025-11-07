#include "SceneArmorEditor.h"

#include <Mygui.h>

#include "../../Library/Graphics/Graphics.h"
#include "../../Source/Player/PlayerActor.h"
#include "../../Source/Armor/ArmorActor.h"

void SceneArmorEditor::OnInitialize()
{
    ID3D11Device* device = Graphics::Instance().GetDevice();

    SetSkyMap(L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/sheen_pmrem.dds",
        L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/diffuse_iem.dds",
        L"./Data/SkyMap/kloofendal_48d_partly_cloudy_puresky_4k/specular_pmrem.dds");

    // デバッグカメラをオンにする
    Debug::GetDebugInput()->buttonData |= DebugInput::BTN_F4;

    // グリッド表示
    SetShowGrid(true);

	auto player = RegisterActor<Actor>(u8"Player", ActorTag::Player);
    player->LoadModel(PlayerActor::GetModelFilePath());
	// コンポーネント追加
	player->AddComponent<ModelRenderer>();
	player->AddComponent<Animator>();

	// 防具生成
	{
		auto head = RegisterActor<ArmorActor>(GetName() + std::string(u8"Head"), ActorTag::Player, ArmorType::Head);
		head->SetParent(player.get());
	}
	{
		auto chest = RegisterActor<ArmorActor>(GetName() + std::string(u8"Chest"), ActorTag::Player, ArmorType::Chest);
		chest->SetParent(player.get());
	}
	{
		auto arm = RegisterActor<ArmorActor>(GetName() + std::string(u8"Arm"), ActorTag::Player, ArmorType::Arm);
		arm->SetParent(player.get());
	}
	{
		auto Waist = RegisterActor<ArmorActor>(GetName() + std::string(u8"Waist"), ActorTag::Player, ArmorType::Waist);
		Waist->SetParent(player.get());
	}
	{
		auto Leg = RegisterActor<ArmorActor>(GetName() + std::string(u8"Leg"), ActorTag::Player, ArmorType::Leg);
		Leg->SetParent(player.get());
	}
}
