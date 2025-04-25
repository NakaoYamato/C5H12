#include "DragonActor.h"

#include "../../Library/Graphics/Graphics.h"
#include "../../Scene/Scene.h"
#include "../../Library/Component/Animator.h"

#include "../../Library/ResourceManager/GpuResourceManager.h"
#include "../../Library/Converter/ToString.h"

#include <imgui.h>

// 生成時処理
void DragonActor::OnCreate()
{
	Actor::OnCreate();

	GetTransform().SetCoordinateType(COORDINATE_TYPE::RHS_Z_UP);
	GetTransform().SetLengthScale(0.01f);

	// コンポーネント追加
	_modelRenderer = AddComponent<ModelRenderer>("./Data/Model/Dragons/Kuzar the Magnificent.fbx");
	auto animator = AddComponent<Animator>(_modelRenderer.lock()->GetModel());

	// モデルがシリアライズされていなければアニメーションを設定
	if (!_modelRenderer.lock()->GetModel()->GetResource()->IsSerialized())
	{
		// テクスチャ設定
		SetModelTexture();

		// モデルのアニメーションを設定
		SetModelAnimation();

		// 再シリアライズ
		_modelRenderer.lock()->GetModel()->ReSerialize();
	}
}

// 開始関数
void DragonActor::Start()
{
	Actor::Start();
}

// 更新処理
void DragonActor::Update(float elapsedTime)
{
	Actor::Update(elapsedTime);

	// 使用する角を設定
	SetUseHorn();
}

// GUI描画
void DragonActor::DrawGui()
{
	Actor::DrawGui();

	static ImGuiTabBarFlags tab_bar_flags =
		ImGuiTabBarFlags_AutoSelectNewTabs |
		ImGuiTabBarFlags_Reorderable |
		ImGuiTabBarFlags_FittingPolicyResizeDown;
	if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
	{
		if (ImGui::BeginTabItem(u8"ドラゴン"))
		{
			static const char* modelTypeName[] =
			{
				u8"Brown",
				u8"Green",
				u8"Lava",
				u8"Red",
				u8"White",
			};
			int mt = static_cast<int>(_modelType);
			if (ImGui::Combo(u8"使用するテクスチャ", &mt, modelTypeName, _countof(modelTypeName)))
			{
				_modelType = static_cast<ModelType>(mt);
				SetModelTexture();
				// 再シリアライズ
				_modelRenderer.lock()->GetModel()->ReSerialize();
			}
			static const char* drawHornTypeName[] =
			{
				u8"Horn01",
				u8"Horn02",
				u8"Horn03",
				u8"Horn04",
				u8"Horn05",
				u8"Horn06",
			};
			int dht = static_cast<int>(_drawHornType);
			if (ImGui::Combo(u8"使用する角", &dht, drawHornTypeName, _countof(drawHornTypeName)))
			{
				_drawHornType = static_cast<DrawHornType>(dht);
				SetUseHorn();
			}

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
}

// モデルのテクスチャを設定
void DragonActor::SetModelTexture()
{
	auto resource = _modelRenderer.lock()->GetModel()->GetResource();
	auto SetModelSRV = [&](const wchar_t* filename, std::string materialName, std::string textureKey)
		{
			static std::wstring RelativePath = L"./Data/Model/Dragons/";

			// テクスチャからSRV作成
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
			GpuResourceManager::LoadTextureFromFile(Graphics::Instance().GetDevice(),
				std::wstring(RelativePath + filename).c_str(),
				srv.ReleaseAndGetAddressOf(),
				nullptr);

			// SRV設定
			for (auto& material : resource->GetAddressMaterials())
			{
				if (material.name == materialName)
				{
					material.textureDatas.at(textureKey).filename = ToString(filename).c_str();
					material.textureDatas.at(textureKey).textureSRV = srv;
					return;
				}
			}
		};

	// 共通部分のテクスチャ
#pragma region Body
	SetModelSRV(L"Texture/T_Kuzar_Body_Normal.png", "Material #25", "Normal");
	SetModelSRV(L"Texture/T_Kuzar_Body_EmissionBW.png", "Material #25", "Emissive");
#pragma endregion
#pragma region Spikes
	SetModelSRV(L"Texture/T_Kuzar_Spikes_Albedo.png", "Material #26", "Diffuse");
	SetModelSRV(L"Texture/T_Kuzar_Spikes_Normal.png", "Material #26", "Normal");
#pragma endregion
#pragma region Wings
	SetModelSRV(L"Texture/T_Kuzar_Wings_Normal.png", "Material #31", "Normal");
	SetModelSRV(L"Texture/T_Kuzar_Wings_EmissionBW.png", "Material #31", "Emissive");
#pragma endregion
#pragma region Horns
	SetModelSRV(L"Texture/T_Kuzar_Horns_Color.png", "Horns", "Diffuse");
	SetModelSRV(L"Texture/T_Kuzar_Horns_Normal.png", "Horns", "Normal");
	SetModelSRV(L"Texture/T_Kuzar_Horns_Emissive.png", "Horns", "Emissive");
#pragma endregion
#pragma region Eyes
	SetModelSRV(L"Texture/T_Dragon_Eyes_Albedo.png", "Eyes", "Diffuse");
	SetModelSRV(L"Texture/T_Dragon_Eyes_Normal.png", "Eyes", "Normal");
#pragma endregion

	switch (_modelType)
	{
	case DragonActor::ModelType::Brown:
#pragma region Body
		SetModelSRV(L"Texture/Brown/T_Kuzar_Brown_Body_Albedo.png", "Material #25", "Diffuse");
		SetModelSRV(L"Texture/Brown/T_Kuzar_Brown_Body_Rough_M_AO.png", "Material #25", "Roughness");
#pragma region Wings
#pragma endregion
		SetModelSRV(L"Texture/Brown/T_Kuzar_Brown_Wings_Albedo.png", "Material #31", "Diffuse");
		SetModelSRV(L"Texture/Brown/T_Kuzar_Brown_Wings_Rough_M_AO.png", "Material #31", "Roughness");
#pragma endregion
		break;
	case DragonActor::ModelType::Green:
#pragma region Body
		SetModelSRV(L"Texture/Green/T_Kuzar_Green_Body_Albedo.png", "Material #25", "Diffuse");
		SetModelSRV(L"Texture/Green/T_Kuzar_Green_Body_Rough_M_AO.png", "Material #25", "Roughness");
#pragma region Wings
#pragma endregion
		SetModelSRV(L"Texture/Green/T_Kuzar_Green_Wings_Albedo.png", "Material #31", "Diffuse");
		SetModelSRV(L"Texture/Green/T_Kuzar_Green_Wings_Rough_M_AO.png", "Material #31", "Roughness");
#pragma endregion
		break;
	case DragonActor::ModelType::Lava:
#pragma region Body
		SetModelSRV(L"Texture/Lava/T_Kuzar_Lava_Body_Albedo.png", "Material #25", "Diffuse");
		SetModelSRV(L"Texture/Lava/T_Kuzar_Lava_Body_Rough_M_AO.png", "Material #25", "Roughness");
		SetModelSRV(L"Texture/Lava/T_Kuzar_Lava_Body_Emissive.png", "Material #25", "Emissive");
#pragma region Wings
#pragma endregion
		SetModelSRV(L"Texture/Lava/T_Kuzar_Lava_Wings_Albedo.png", "Material #31", "Diffuse");
		SetModelSRV(L"Texture/Lava/T_Kuzar_Lava_Wings_Rough_M_AO.png", "Material #31", "Roughness");
		SetModelSRV(L"Texture/Lava/T_Kuzar_Lava_Wings_Emissive.png", "Material #31", "Emissive");
#pragma endregion
		break;
	case DragonActor::ModelType::Red:
#pragma region Body
		SetModelSRV(L"Texture/Red/T_Kuzar_Red_Body_Albedo.png", "Material #25", "Diffuse");
		SetModelSRV(L"Texture/Red/T_Kuzar_Red_Body_Rough_M_AO.png", "Material #25", "Roughness");
#pragma region Wings
#pragma endregion
		SetModelSRV(L"Texture/Red/T_Kuzar_Red_Wings_Albedo.png", "Material #31", "Diffuse");
		SetModelSRV(L"Texture/Red/T_Kuzar_Red_Wings_Rough_M_AO.png", "Material #31", "Roughness");
#pragma endregion
		break;
	case DragonActor::ModelType::White:
#pragma region Body
		SetModelSRV(L"Texture/White/T_Kuzar_White_Body_Albedo.png", "Material #25", "Diffuse");
		SetModelSRV(L"Texture/White/T_Kuzar_White_Body_Rough_M_AO.png", "Material #25", "Roughness");
#pragma region Wings
#pragma endregion
		SetModelSRV(L"Texture/White/T_Kuzar_White_Wings_Albedo.png", "Material #31", "Diffuse");
		SetModelSRV(L"Texture/White/T_Kuzar_White_Wings_Rough_M_AO.png", "Material #31", "Roughness");
#pragma endregion
		break;
	}
}

// モデルのアニメーションを設定
void DragonActor::SetModelAnimation()
{
	// アニメーションデータ一覧
	// ＊一部アニメーションは座標系が異なる
	static std::tuple<const char*, const char*> animParams[] =
	{
#pragma region Attack
		{ "./Data/Model/Dragons/Animation/Attacks/WD_Attack_Bite_Front.fbx" ,		"AttackBiteFront" },
		{ "./Data/Model/Dragons/Animation/Attacks/WD_Attack_Bite_Left.fbx" ,		"AttackBiteLeft" },
		{ "./Data/Model/Dragons/Animation/Attacks/WD_Attack_Bite_Right.fbx" ,		"AttackBiteRight" },
		{ "./Data/Model/Dragons/Animation/Attacks/WD_Attack_FireBall.fbx" ,			"AttackFireBall" },
		{ "./Data/Model/Dragons/Animation/Attacks/WD_Attack_FireBreath.fbx" ,		"AttackFireBreath" },
		{ "./Data/Model/Dragons/Animation/Attacks/WD_Attack_FireBreath_Swipe.fbx" ,	"AttackFireSwipe" },
		{ "./Data/Model/Dragons/Animation/Attacks/WD_Attack_Fly_Legs Mask.fbx" ,	"AttackFlyLegsMask" },
		{ "./Data/Model/Dragons/Animation/Attacks/WD_Attack_Fly_Melee Mask.fbx" ,	"AttackFlyMeleeMask" },
		{ "./Data/Model/Dragons/Animation/Attacks/WD_Attack_Special1.fbx" ,			"AttackSpecial1" },// R-Y
		{ "./Data/Model/Dragons/Animation/Attacks/WD_Attack_Tail_Left.fbx" ,		"AttackTailLeft" },
		{ "./Data/Model/Dragons/Animation/Attacks/WD_Attack_Tail_Right.fbx" ,		"AttackTailRight" },
		{ "./Data/Model/Dragons/Animation/Attacks/WD_Attack_Wing_Fist_Left.fbx" ,	"AttackWingFistLeft" },
		{ "./Data/Model/Dragons/Animation/Attacks/WD_Attack_Wing_Fist_Right.fbx" ,	"AttackWingFistRight" },
		{ "./Data/Model/Dragons/Animation/Attacks/WD_Attack_Wing_Swipe_Left.fbx" ,	"AttackWingSwipeLeft" },// R-Y
		{ "./Data/Model/Dragons/Animation/Attacks/WD_Attack_Wing_Swipe_Right.fbx" ,	"AttackWingSwipeRight" },// R-Y
		{ "./Data/Model/Dragons/Animation/Attacks/WD_FireMask.fbx" ,				"FireMask" },
#pragma endregion

#pragma region Damage
		{ "./Data/Model/Dragons/Animation/Damage/WD_Damage_Back_Left.fbx" ,			"DamageBackLeft" },
		{ "./Data/Model/Dragons/Animation/Damage/WD_Damage_Back_Right.fbx" ,		"DamageBackRight" },
		{ "./Data/Model/Dragons/Animation/Damage/WD_Damage_Front_Left.fbx" ,		"DamageFrontLeft" },
		{ "./Data/Model/Dragons/Animation/Damage/WD_Damage_Front_Right.fbx" ,		"DamageFrontRight" },
		{ "./Data/Model/Dragons/Animation/Damage/WD_Damage_Left.fbx" ,				"DamageLeft" },
		{ "./Data/Model/Dragons/Animation/Damage/WD_Damage_Right.fbx" ,				"DamageRight" },
		{ "./Data/Model/Dragons/Animation/Damage/WD_Damage_Mask.fbx" ,				"DamageMask" },
#pragma endregion

#pragma region Death
		{ "./Data/Model/Dragons/Animation/Death/WD_Death01.fbx" ,				"Death01" },
		{ "./Data/Model/Dragons/Animation/Death/WD_Death02.fbx" ,				"Death02" },
		{ "./Data/Model/Dragons/Animation/Death/WD_Death01_Opp.fbx" ,			"Death01Opp" },
		{ "./Data/Model/Dragons/Animation/Death/WD_Death02_Opp.fbx" ,			"Death02Opp" },
#pragma endregion

#pragma region Dodge
		{ "./Data/Model/Dragons/Animation/Dodge/WD_Dodge_Air_Left.fbx" ,			"DodgeAirLeft" },
		{ "./Data/Model/Dragons/Animation/Dodge/WD_Dodge_Air_Right.fbx" ,			"DodgeAirRight" },
		{ "./Data/Model/Dragons/Animation/Dodge/WD_Dodge_Ground_Left.fbx" ,			"DodgeGroundLeft" },
		{ "./Data/Model/Dragons/Animation/Dodge/WD_Dodge_Ground_Right.fbx" ,		"DodgeGroundRight" },
#pragma endregion

#pragma region Fall
		{ "./Data/Model/Dragons/Animation/Fall/WD_Fall_Hit_Air Recover.fbx" ,	"FallHitAirRecover" },
		{ "./Data/Model/Dragons/Animation/Fall/WD_Fall_Hit_DeathGround.fbx" ,	"FallDeathGround" },
		{ "./Data/Model/Dragons/Animation/Fall/WD_Fall_Hit_Down.fbx" ,			"FallHitDown" },
		{ "./Data/Model/Dragons/Animation/Fall/WD_Fall_Hit_Loop.fbx" ,			"FallHitLoop" },
		{ "./Data/Model/Dragons/Animation/Fall/WD_Fall_High.fbx" ,				"FallHigh" },
		{ "./Data/Model/Dragons/Animation/Fall/WD_Fall_Low.fbx" ,				"FallLow" },
		{ "./Data/Model/Dragons/Animation/Fall/WD_Fall_Land.fbx" ,				"FallLand" },// R-Y
		{ "./Data/Model/Dragons/Animation/Fall/WD_Fall_Edge.fbx" ,				"FallEdge" },
#pragma endregion

#pragma region Fly
		{ "./Data/Model/Dragons/Animation/Fly/WD_Fly_Stand.fbx" ,					"FlyStand" },
		{ "./Data/Model/Dragons/Animation/Fly/WD_Fly_Backwards.fbx" ,				"FlyBackward" },
		{ "./Data/Model/Dragons/Animation/Fly/WD_Fly_Forward.fbx" ,					"FlyForward" },
		{ "./Data/Model/Dragons/Animation/Fly/WD_Fly_Left.fbx" ,					"FlyLeft" },
		{ "./Data/Model/Dragons/Animation/Fly/WD_Fly_Right.fbx" ,					"FlyRight" },
		{ "./Data/Model/Dragons/Animation/Fly/WD_Fly_Stand_Turn_Left.fbx" ,			"FlyStandTurnLeft" },
		{ "./Data/Model/Dragons/Animation/Fly/WD_Fly_Stand_Turn_Right.fbx" ,		"FlyStandTurnRight" },
		{ "./Data/Model/Dragons/Animation/Fly/WD_Glide_Forward.fbx" ,				"GlideForward" },
		{ "./Data/Model/Dragons/Animation/Fly/WD_Glide_Right.fbx" ,					"GlideRight" },
		{ "./Data/Model/Dragons/Animation/Fly/WD_Glide_Left.fbx" ,					"GlideLeft" },
		{ "./Data/Model/Dragons/Animation/Fly/WD_Glide_Wings_Closed_50.fbx" ,		"GlideWingsClosed50" },
		{ "./Data/Model/Dragons/Animation/Fly/WD_Glide_Wings_Closed_50_Left.fbx" ,	"GlideWingsClosed50Left" },
		{ "./Data/Model/Dragons/Animation/Fly/WD_Glide_Wings_Closed_50_Right.fbx" ,	"GlideWingsClosed50Right" },
		{ "./Data/Model/Dragons/Animation/Fly/WD_Glide_Wings_Closed_100.fbx" ,		"GlideWingsClosed100" },
#pragma endregion

#pragma region Fly2
		{ "./Data/Model/Dragons/Animation/Fly2/WD_v2_Dash_Start.fbx" ,					"Fly2DashStart" },
		{ "./Data/Model/Dragons/Animation/Fly2/WD_v2_Dash.fbx" ,						"Fly2Dash" },
		{ "./Data/Model/Dragons/Animation/Fly2/WD_v2_Dash_Left.fbx" ,					"Fly2DashLeft" },
		{ "./Data/Model/Dragons/Animation/Fly2/WD_v2_Dash_Right.fbx" ,					"Fly2DashRight" },
		{ "./Data/Model/Dragons/Animation/Fly2/WD_v2_Dash_End.fbx" ,					"Fly2DashEnd" },
		{ "./Data/Model/Dragons/Animation/Fly2/WD_v2_Fly_Flap_Forward.fbx" ,			"Fly2FlapForward" },
		{ "./Data/Model/Dragons/Animation/Fly2/WD_v2_Fly_Flap_Forward_DeltaUP.fbx" ,	"Fly2FlapForwardDeltaUp" },
		{ "./Data/Model/Dragons/Animation/Fly2/WD_v2_Fly_Flap_Left.fbx" ,				"Fly2FlapLeft" },
		{ "./Data/Model/Dragons/Animation/Fly2/WD_v2_Fly_Flap_Right.fbx" ,				"Fly2FlapRight" },
		{ "./Data/Model/Dragons/Animation/Fly2/WD_v2_Fly_Flap_Upwards.fbx" ,			"Fly2FlapUpward" },
		{ "./Data/Model/Dragons/Animation/Fly2/WD_v2_Fly_Stand.fbx" ,					"Fly2Stand" },
		{ "./Data/Model/Dragons/Animation/Fly2/WD_v2_Fly_Stand_TurnLeft.fbx" ,			"Fly2StandTurnLeft" },
		{ "./Data/Model/Dragons/Animation/Fly2/WD_v2_Fly_Stand_TurnRight.fbx" ,			"Fly2StandTurnRight" },
		{ "./Data/Model/Dragons/Animation/Fly2/WD_v2_Fly_Strafe_Forward.fbx" ,			"Fly2StrafeForward" },
		{ "./Data/Model/Dragons/Animation/Fly2/WD_v2_Fly_Strafe_Back.fbx" ,				"Fly2StrafeBack" },
		{ "./Data/Model/Dragons/Animation/Fly2/WD_v2_Fly_Strafe_Left.fbx" ,				"Fly2StrafeLeft" },
		{ "./Data/Model/Dragons/Animation/Fly2/WD_v2_Fly_Strafe_Right.fbx" ,			"Fly2StrafeRight" },
		{ "./Data/Model/Dragons/Animation/Fly2/WD_v2_Glide_Closed_50.fbx" ,				"GlideClosed50" },
		{ "./Data/Model/Dragons/Animation/Fly2/WD_v2_Glide_Closed_50_DeltaDown.fbx" ,	"GlideClosed50DeltaDown" },
		{ "./Data/Model/Dragons/Animation/Fly2/WD_v2_Glide_Closed_50_DeltaUp.fbx" ,		"GlideClosed50DeltaUp" },
		{ "./Data/Model/Dragons/Animation/Fly2/WD_v2_Glide_Closed_50_Left.fbx" ,		"GlideClosed50Left" },
		{ "./Data/Model/Dragons/Animation/Fly2/WD_v2_Glide_Closed_50_Right.fbx" ,		"GlideClosed50Right" },
		{ "./Data/Model/Dragons/Animation/Fly2/WD_v2_Glide_Closed_100.fbx" ,			"GlideClosed100" },
		{ "./Data/Model/Dragons/Animation/Fly2/WD_v2_Glide_Closed_100_DeltaDown.fbx" ,	"GlideClosed100DeltaDown" },
		{ "./Data/Model/Dragons/Animation/Fly2/WD_v2_Glide_Closed_100_DeltaUp.fbx" ,	"GlideClosed100DeltaUp" },
		{ "./Data/Model/Dragons/Animation/Fly2/WD_v2_Glide_Closed_100_Left.fbx" ,		"GlideClosed100Left" },
		{ "./Data/Model/Dragons/Animation/Fly2/WD_v2_Glide_Closed_100_Right.fbx" ,		"GlideClosed100Right" },
		{ "./Data/Model/Dragons/Animation/Fly2/WD_v2_Glide_DeltaDown.fbx" ,				"GlideDeltaDown" },
		{ "./Data/Model/Dragons/Animation/Fly2/WD_v2_Glide_DeltaUp.fbx" ,				"GlideDeltaUp" },
		{ "./Data/Model/Dragons/Animation/Fly2/WD_v2_Glide_Open.fbx" ,					"GlideOpen" },
		{ "./Data/Model/Dragons/Animation/Fly2/WD_v2_Glide_Open_Left.fbx" ,				"GlideOpenLeft" },
		{ "./Data/Model/Dragons/Animation/Fly2/WD_v2_Glide_Open_Right.fbx" ,			"GlideOpenRight" },
#pragma endregion

#pragma region Jumps
		{ "./Data/Model/Dragons/Animation/Jumps/WD_Jump_In_Place.fbx" ,	"JumpInPlace" },
		{ "./Data/Model/Dragons/Animation/Jumps/WD_Jump_Run.fbx" ,		"JumpRun" },
		{ "./Data/Model/Dragons/Animation/Jumps/WD_Jump_Trot.fbx" ,		"JumpTrot" },
		{ "./Data/Model/Dragons/Animation/Jumps/WD_Jump_Trot_V2.fbx" ,	"JumpTrotV2" },
		{ "./Data/Model/Dragons/Animation/Jumps/WD_Land.fbx" ,			"JumpLand" },
		{ "./Data/Model/Dragons/Animation/Jumps/WD_TakeOff.fbx" ,		"JumpTakeOff" },
#pragma endregion

#pragma region Locomotion
		{ "./Data/Model/Dragons/Animation/Locomotion/WD_Run_Forward.fbx" ,		"RunForward" },
		{ "./Data/Model/Dragons/Animation/Locomotion/WD_Run_Left.fbx" ,			"RunLeft" },
		{ "./Data/Model/Dragons/Animation/Locomotion/WD_Run_Right.fbx" ,		"RunRight" },
		{ "./Data/Model/Dragons/Animation/Locomotion/WD_Trot_Forward.fbx" ,		"TrotForward" },
		{ "./Data/Model/Dragons/Animation/Locomotion/WD_Trot_Left.fbx" ,		"TrotLeft" },
		{ "./Data/Model/Dragons/Animation/Locomotion/WD_Trot_Right.fbx" ,		"TrotRight" },
		{ "./Data/Model/Dragons/Animation/Locomotion/WD_Turn_Left_90.fbx" ,		"TurnLeft90" },
		{ "./Data/Model/Dragons/Animation/Locomotion/WD_Turn_Left_180.fbx" ,	"TurnLeft180" },
		{ "./Data/Model/Dragons/Animation/Locomotion/WD_Turn_Right_90.fbx" ,	"TurnRight90" },
		{ "./Data/Model/Dragons/Animation/Locomotion/WD_Turn_Right_180.fbx" ,	"TurnRight180" },
		{ "./Data/Model/Dragons/Animation/Locomotion/WD_Walk_Forward.fbx" ,		"WalkForward" },
		{ "./Data/Model/Dragons/Animation/Locomotion/WD_Walk_Left.fbx" ,		"WalkLeft" },
		{ "./Data/Model/Dragons/Animation/Locomotion/WD_Walk_Right.fbx" ,		"WalkRight" },
		{ "./Data/Model/Dragons/Animation/Locomotion/WD_WalkBack.fbx" ,			"WalkBack" },
		{ "./Data/Model/Dragons/Animation/Locomotion/WD_WalkBack_Left.fbx" ,	"WalkBackLeft" },
		{ "./Data/Model/Dragons/Animation/Locomotion/WD_WalkBack_Right.fbx" ,	"WalkBackRight" },
#pragma endregion

#pragma region Swim
		{ "./Data/Model/Dragons/Animation/Swim/WD_Swim_Backwards.fbx" ,				"SwimBackwards" },
		{ "./Data/Model/Dragons/Animation/Swim/WD_Swim_Enter_Water.fbx" ,			"SwimEnterWater" },
		{ "./Data/Model/Dragons/Animation/Swim/WD_Swim_Forward.fbx" ,				"SwimForward" },
		{ "./Data/Model/Dragons/Animation/Swim/WD_Swim_Idle.fbx" ,					"SwimIdle" },
		{ "./Data/Model/Dragons/Animation/Swim/WD_Swim_Jump.fbx" ,					"SwimJump" },
		{ "./Data/Model/Dragons/Animation/Swim/WD_Swim_Left.fbx" ,					"SwimLeft" },
		{ "./Data/Model/Dragons/Animation/Swim/WD_Swim_Right.fbx" ,					"SwimRight" },
		{ "./Data/Model/Dragons/Animation/Swim/WD_Swim_Turn_Left.fbx" ,				"SwimTurnLeft" },
		{ "./Data/Model/Dragons/Animation/Swim/WD_Swim_Turn_Right.fbx" ,			"SwimTurnRight" },
		{ "./Data/Model/Dragons/Animation/Swim/WD_Swim_UnderWater_Backwards.fbx" ,	"SwimUnderWaterBackwards" },
		{ "./Data/Model/Dragons/Animation/Swim/WD_Swim_Underwater_Forward.fbx" ,	"SwimUnderWaterForward" },
		{ "./Data/Model/Dragons/Animation/Swim/WD_Swim_UnderWater_Idle.fbx" ,		"SwimUnderWaterIdle" },
		{ "./Data/Model/Dragons/Animation/Swim/WD_Swim_Underwater_Left.fbx" ,		"SwimUnderWaterLeft" },
		{ "./Data/Model/Dragons/Animation/Swim/WD_Swim_Underwater_Right.fbx" ,		"SwimUnderWaterRight" },
		{ "./Data/Model/Dragons/Animation/Swim/WD_Swim_UnderWater_Turn_Left.fbx" ,	"SwimUnderWaterTurnLeft" },
		{ "./Data/Model/Dragons/Animation/Swim/WD_Swim_UnderWater_Turn_Right.fbx" ,	"SwimUnderWaterTurnRight" },
#pragma endregion

#pragma region Idle
		{ "./Data/Model/Dragons/Animation/Idle/WD_Idle.fbx" ,					"Idle01" },
		{ "./Data/Model/Dragons/Animation/Idle/WD_Idle_02_Look.fbx" ,			"Idle02Look" },
		{ "./Data/Model/Dragons/Animation/Idle/WD_Idle_02_Look_Right.fbx" ,		"Idle02LookRight" },// R-Y
		{ "./Data/Model/Dragons/Animation/Idle/WD_Idle_03_Shake.fbx" ,			"Idle03Shake" },
		{ "./Data/Model/Dragons/Animation/Idle/WD_Idle_Combat.fbx" ,			"IdleCombat" },// R-Y
		{ "./Data/Model/Dragons/Animation/Idle/WD_Idle_Combat_Rage.fbx" ,		"IdleCombatRage" },// R-Y
		{ "./Data/Model/Dragons/Animation/Idle/WD_Idle_Rage.fbx" ,				"IdleRage" },// R-Y
#pragma endregion
	};

	// アニメーション追加
	_modelRenderer.lock()->GetModel()->GetResource()->GetAddressAnimations().clear();
	for (auto& [filename, animationName] : animParams)
	{
		_modelRenderer.lock()->GetModel()->GetResource()->AppendAnimations(
			filename,
			animationName);
	}
}

// 使用する角を設定
void DragonActor::SetUseHorn()
{
	auto& nodes = _modelRenderer.lock()->GetModel()->GetAddressPoseNodes();
	// posenodeから"Kuzar3_Horn"を含むnodeを全取得
	ModelResource::Node* horns[6] = {};
	std::vector<int> hornIncludeIndices;
	int index = 0;
	for (auto& node : nodes)
	{
		if (node.name.find("Kuzar3_Horn") != std::string::npos)
			hornIncludeIndices.push_back(index);
		index++;
	}

	static std::string HornNames[] =
	{
		"Kuzar3_Horn01",
		"Kuzar3_Horn02",
		"Kuzar3_Horn03",
		"Kuzar3_Horn04",
		"Kuzar3_Horn05",
		"Kuzar3_Horn06",
	};
	for (int i : hornIncludeIndices)
	{
		if (nodes[i].name == HornNames[static_cast<int>(_drawHornType)])
			nodes[i].scale = _VECTOR3_ONE;
		else
			nodes[i].scale = _VECTOR3_ZERO;
	}
}
