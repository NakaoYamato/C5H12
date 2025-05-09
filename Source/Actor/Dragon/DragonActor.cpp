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

	// モデル読み込み
	auto model = LoadModel("./Data/Model/Dragons/Kuzar the Magnificent.fbx");

	GetTransform().SetLengthScale(0.01f);

	// コンポーネント追加
	_modelRenderer = AddComponent<ModelRenderer>();
	auto animator = AddComponent<Animator>();

	// モデルがシリアライズされていなければアニメーションを設定
	if (!model.lock()->GetResource()->IsSerialized())
	{
		// テクスチャ設定
		SetModelTexture();

		// モデルのアニメーションを設定
		SetModelAnimation();

		// 再シリアライズ
		model.lock()->ReSerialize();
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
				GetModel().lock()->ReSerialize();
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
	auto resource = GetModel().lock()->GetResource();
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
		{ "./Data/Model/Dragons/Animation/Attacks/WD_Attack_Bite_Front.model" ,		"AttackBiteFront" },
		{ "./Data/Model/Dragons/Animation/Attacks/WD_Attack_Bite_Left.model" ,		"AttackBiteLeft" },
		{ "./Data/Model/Dragons/Animation/Attacks/WD_Attack_Bite_Right.model" ,		"AttackBiteRight" },
		{ "./Data/Model/Dragons/Animation/Attacks/WD_Attack_FireBall.model" ,			"AttackFireBall" },
		{ "./Data/Model/Dragons/Animation/Attacks/WD_Attack_FireBreath.model" ,		"AttackFireBreath" },
		{ "./Data/Model/Dragons/Animation/Attacks/WD_Attack_FireBreath_Swipe.model" ,	"AttackFireSwipe" },
		{ "./Data/Model/Dragons/Animation/Attacks/WD_Attack_Fly_Legs Mask.model" ,	"AttackFlyLegsMask" },
		{ "./Data/Model/Dragons/Animation/Attacks/WD_Attack_Fly_Melee Mask.model" ,	"AttackFlyMeleeMask" },
		{ "./Data/Model/Dragons/Animation/Attacks/WD_Attack_Special1.model" ,			"AttackSpecial1" },// R-Y
		{ "./Data/Model/Dragons/Animation/Attacks/WD_Attack_Tail_Left.model" ,		"AttackTailLeft" },
		{ "./Data/Model/Dragons/Animation/Attacks/WD_Attack_Tail_Right.model" ,		"AttackTailRight" },
		{ "./Data/Model/Dragons/Animation/Attacks/WD_Attack_Wing_Fist_Left.model" ,	"AttackWingFistLeft" },
		{ "./Data/Model/Dragons/Animation/Attacks/WD_Attack_Wing_Fist_Right.model" ,	"AttackWingFistRight" },
		{ "./Data/Model/Dragons/Animation/Attacks/WD_Attack_Wing_Swipe_Left.model" ,	"AttackWingSwipeLeft" },// R-Y
		{ "./Data/Model/Dragons/Animation/Attacks/WD_Attack_Wing_Swipe_Right.model" ,	"AttackWingSwipeRight" },// R-Y
		{ "./Data/Model/Dragons/Animation/Attacks/WD_FireMask.model" ,				"FireMask" },
#pragma endregion

#pragma region Damage
		{ "./Data/Model/Dragons/Animation/Damage/WD_Damage_Back_Left.model" ,			"DamageBackLeft" },
		{ "./Data/Model/Dragons/Animation/Damage/WD_Damage_Back_Right.model" ,		"DamageBackRight" },
		{ "./Data/Model/Dragons/Animation/Damage/WD_Damage_Front_Left.model" ,		"DamageFrontLeft" },
		{ "./Data/Model/Dragons/Animation/Damage/WD_Damage_Front_Right.model" ,		"DamageFrontRight" },
		{ "./Data/Model/Dragons/Animation/Damage/WD_Damage_Left.model" ,				"DamageLeft" },
		{ "./Data/Model/Dragons/Animation/Damage/WD_Damage_Right.model" ,				"DamageRight" },
		{ "./Data/Model/Dragons/Animation/Damage/WD_Damage_Mask.model" ,				"DamageMask" },
#pragma endregion

#pragma region Death
		{ "./Data/Model/Dragons/Animation/Death/WD_Death01.model" ,				"Death01" },
		{ "./Data/Model/Dragons/Animation/Death/WD_Death02.model" ,				"Death02" },
		{ "./Data/Model/Dragons/Animation/Death/WD_Death01_Opp.model" ,			"Death01Opp" },
		{ "./Data/Model/Dragons/Animation/Death/WD_Death02_Opp.model" ,			"Death02Opp" },
#pragma endregion

#pragma region Dodge
		{ "./Data/Model/Dragons/Animation/Dodge/WD_Dodge_Air_Left.model" ,			"DodgeAirLeft" },
		{ "./Data/Model/Dragons/Animation/Dodge/WD_Dodge_Air_Right.model" ,			"DodgeAirRight" },
		{ "./Data/Model/Dragons/Animation/Dodge/WD_Dodge_Ground_Left.model" ,			"DodgeGroundLeft" },
		{ "./Data/Model/Dragons/Animation/Dodge/WD_Dodge_Ground_Right.model" ,		"DodgeGroundRight" },
#pragma endregion

#pragma region Fall
		{ "./Data/Model/Dragons/Animation/Fall/WD_Fall_Hit_Air Recover.model" ,	"FallHitAirRecover" },
		{ "./Data/Model/Dragons/Animation/Fall/WD_Fall_Hit_DeathGround.model" ,	"FallDeathGround" },
		{ "./Data/Model/Dragons/Animation/Fall/WD_Fall_Hit_Down.model" ,			"FallHitDown" },
		{ "./Data/Model/Dragons/Animation/Fall/WD_Fall_Hit_Loop.model" ,			"FallHitLoop" },
		{ "./Data/Model/Dragons/Animation/Fall/WD_Fall_High.model" ,				"FallHigh" },
		{ "./Data/Model/Dragons/Animation/Fall/WD_Fall_Low.model" ,				"FallLow" },
		{ "./Data/Model/Dragons/Animation/Fall/WD_Fall_Land.model" ,				"FallLand" },// R-Y
		{ "./Data/Model/Dragons/Animation/Fall/WD_Fall_Edge.model" ,				"FallEdge" },
#pragma endregion

#pragma region Fly
		{ "./Data/Model/Dragons/Animation/Fly/WD_Fly_Stand.model" ,					"FlyStand" },
		{ "./Data/Model/Dragons/Animation/Fly/WD_Fly_Backwards.model" ,				"FlyBackward" },
		{ "./Data/Model/Dragons/Animation/Fly/WD_Fly_Forward.model" ,					"FlyForward" },
		{ "./Data/Model/Dragons/Animation/Fly/WD_Fly_Left.model" ,					"FlyLeft" },
		{ "./Data/Model/Dragons/Animation/Fly/WD_Fly_Right.model" ,					"FlyRight" },
		{ "./Data/Model/Dragons/Animation/Fly/WD_Fly_Stand_Turn_Left.model" ,			"FlyStandTurnLeft" },
		{ "./Data/Model/Dragons/Animation/Fly/WD_Fly_Stand_Turn_Right.model" ,		"FlyStandTurnRight" },
		{ "./Data/Model/Dragons/Animation/Fly/WD_Glide_Forward.model" ,				"GlideForward" },
		{ "./Data/Model/Dragons/Animation/Fly/WD_Glide_Right.model" ,					"GlideRight" },
		{ "./Data/Model/Dragons/Animation/Fly/WD_Glide_Left.model" ,					"GlideLeft" },
		{ "./Data/Model/Dragons/Animation/Fly/WD_Glide_Wings_Closed_50.model" ,		"GlideWingsClosed50" },
		{ "./Data/Model/Dragons/Animation/Fly/WD_Glide_Wings_Closed_50_Left.model" ,	"GlideWingsClosed50Left" },
		{ "./Data/Model/Dragons/Animation/Fly/WD_Glide_Wings_Closed_50_Right.model" ,	"GlideWingsClosed50Right" },
		{ "./Data/Model/Dragons/Animation/Fly/WD_Glide_Wings_Closed_100.model" ,		"GlideWingsClosed100" },
#pragma endregion

#pragma region Fly2
		{ "./Data/Model/Dragons/Animation/Fly2/WD_v2_Dash_Start.model" ,					"Fly2DashStart" },
		{ "./Data/Model/Dragons/Animation/Fly2/WD_v2_Dash.model" ,						"Fly2Dash" },
		{ "./Data/Model/Dragons/Animation/Fly2/WD_v2_Dash_Left.model" ,					"Fly2DashLeft" },
		{ "./Data/Model/Dragons/Animation/Fly2/WD_v2_Dash_Right.model" ,					"Fly2DashRight" },
		{ "./Data/Model/Dragons/Animation/Fly2/WD_v2_Dash_End.model" ,					"Fly2DashEnd" },
		{ "./Data/Model/Dragons/Animation/Fly2/WD_v2_Fly_Flap_Forward.model" ,			"Fly2FlapForward" },
		{ "./Data/Model/Dragons/Animation/Fly2/WD_v2_Fly_Flap_Forward_DeltaUP.model" ,	"Fly2FlapForwardDeltaUp" },
		{ "./Data/Model/Dragons/Animation/Fly2/WD_v2_Fly_Flap_Left.model" ,				"Fly2FlapLeft" },
		{ "./Data/Model/Dragons/Animation/Fly2/WD_v2_Fly_Flap_Right.model" ,				"Fly2FlapRight" },
		{ "./Data/Model/Dragons/Animation/Fly2/WD_v2_Fly_Flap_Upwards.model" ,			"Fly2FlapUpward" },
		{ "./Data/Model/Dragons/Animation/Fly2/WD_v2_Fly_Stand.model" ,					"Fly2Stand" },
		{ "./Data/Model/Dragons/Animation/Fly2/WD_v2_Fly_Stand_TurnLeft.model" ,			"Fly2StandTurnLeft" },
		{ "./Data/Model/Dragons/Animation/Fly2/WD_v2_Fly_Stand_TurnRight.model" ,			"Fly2StandTurnRight" },
		{ "./Data/Model/Dragons/Animation/Fly2/WD_v2_Fly_Strafe_Forward.model" ,			"Fly2StrafeForward" },
		{ "./Data/Model/Dragons/Animation/Fly2/WD_v2_Fly_Strafe_Back.model" ,				"Fly2StrafeBack" },
		{ "./Data/Model/Dragons/Animation/Fly2/WD_v2_Fly_Strafe_Left.model" ,				"Fly2StrafeLeft" },
		{ "./Data/Model/Dragons/Animation/Fly2/WD_v2_Fly_Strafe_Right.model" ,			"Fly2StrafeRight" },
		{ "./Data/Model/Dragons/Animation/Fly2/WD_v2_Glide_Closed_50.model" ,				"GlideClosed50" },
		{ "./Data/Model/Dragons/Animation/Fly2/WD_v2_Glide_Closed_50_DeltaDown.model" ,	"GlideClosed50DeltaDown" },
		{ "./Data/Model/Dragons/Animation/Fly2/WD_v2_Glide_Closed_50_DeltaUp.model" ,		"GlideClosed50DeltaUp" },
		{ "./Data/Model/Dragons/Animation/Fly2/WD_v2_Glide_Closed_50_Left.model" ,		"GlideClosed50Left" },
		{ "./Data/Model/Dragons/Animation/Fly2/WD_v2_Glide_Closed_50_Right.model" ,		"GlideClosed50Right" },
		{ "./Data/Model/Dragons/Animation/Fly2/WD_v2_Glide_Closed_100.model" ,			"GlideClosed100" },
		{ "./Data/Model/Dragons/Animation/Fly2/WD_v2_Glide_Closed_100_DeltaDown.model" ,	"GlideClosed100DeltaDown" },
		{ "./Data/Model/Dragons/Animation/Fly2/WD_v2_Glide_Closed_100_DeltaUp.model" ,	"GlideClosed100DeltaUp" },
		{ "./Data/Model/Dragons/Animation/Fly2/WD_v2_Glide_Closed_100_Left.model" ,		"GlideClosed100Left" },
		{ "./Data/Model/Dragons/Animation/Fly2/WD_v2_Glide_Closed_100_Right.model" ,		"GlideClosed100Right" },
		{ "./Data/Model/Dragons/Animation/Fly2/WD_v2_Glide_DeltaDown.model" ,				"GlideDeltaDown" },
		{ "./Data/Model/Dragons/Animation/Fly2/WD_v2_Glide_DeltaUp.model" ,				"GlideDeltaUp" },
		{ "./Data/Model/Dragons/Animation/Fly2/WD_v2_Glide_Open.model" ,					"GlideOpen" },
		{ "./Data/Model/Dragons/Animation/Fly2/WD_v2_Glide_Open_Left.model" ,				"GlideOpenLeft" },
		{ "./Data/Model/Dragons/Animation/Fly2/WD_v2_Glide_Open_Right.model" ,			"GlideOpenRight" },
#pragma endregion

#pragma region Jumps
		{ "./Data/Model/Dragons/Animation/Jumps/WD_Jump_In_Place.model" ,	"JumpInPlace" },
		{ "./Data/Model/Dragons/Animation/Jumps/WD_Jump_Run.model" ,		"JumpRun" },
		{ "./Data/Model/Dragons/Animation/Jumps/WD_Jump_Trot.model" ,		"JumpTrot" },
		{ "./Data/Model/Dragons/Animation/Jumps/WD_Jump_Trot_V2.model" ,	"JumpTrotV2" },
		{ "./Data/Model/Dragons/Animation/Jumps/WD_Land.model" ,			"JumpLand" },
		{ "./Data/Model/Dragons/Animation/Jumps/WD_TakeOff.model" ,		"JumpTakeOff" },
#pragma endregion

#pragma region Locomotion
		{ "./Data/Model/Dragons/Animation/Locomotion/WD_Run_Forward.model" ,		"RunForward" },
		{ "./Data/Model/Dragons/Animation/Locomotion/WD_Run_Left.model" ,			"RunLeft" },
		{ "./Data/Model/Dragons/Animation/Locomotion/WD_Run_Right.model" ,		"RunRight" },
		{ "./Data/Model/Dragons/Animation/Locomotion/WD_Trot_Forward.model" ,		"TrotForward" },
		{ "./Data/Model/Dragons/Animation/Locomotion/WD_Trot_Left.model" ,		"TrotLeft" },
		{ "./Data/Model/Dragons/Animation/Locomotion/WD_Trot_Right.model" ,		"TrotRight" },
		{ "./Data/Model/Dragons/Animation/Locomotion/WD_Turn_Left_90.model" ,		"TurnLeft90" },
		{ "./Data/Model/Dragons/Animation/Locomotion/WD_Turn_Left_180.model" ,	"TurnLeft180" },
		{ "./Data/Model/Dragons/Animation/Locomotion/WD_Turn_Right_90.model" ,	"TurnRight90" },
		{ "./Data/Model/Dragons/Animation/Locomotion/WD_Turn_Right_180.model" ,	"TurnRight180" },
		{ "./Data/Model/Dragons/Animation/Locomotion/WD_Walk_Forward.model" ,		"WalkForward" },
		{ "./Data/Model/Dragons/Animation/Locomotion/WD_Walk_Left.model" ,		"WalkLeft" },
		{ "./Data/Model/Dragons/Animation/Locomotion/WD_Walk_Right.model" ,		"WalkRight" },
		{ "./Data/Model/Dragons/Animation/Locomotion/WD_WalkBack.model" ,			"WalkBack" },
		{ "./Data/Model/Dragons/Animation/Locomotion/WD_WalkBack_Left.model" ,	"WalkBackLeft" },
		{ "./Data/Model/Dragons/Animation/Locomotion/WD_WalkBack_Right.model" ,	"WalkBackRight" },
#pragma endregion

#pragma region Swim
		{ "./Data/Model/Dragons/Animation/Swim/WD_Swim_Backwards.model" ,				"SwimBackwards" },
		{ "./Data/Model/Dragons/Animation/Swim/WD_Swim_Enter_Water.model" ,			"SwimEnterWater" },
		{ "./Data/Model/Dragons/Animation/Swim/WD_Swim_Forward.model" ,				"SwimForward" },
		{ "./Data/Model/Dragons/Animation/Swim/WD_Swim_Idle.model" ,					"SwimIdle" },
		{ "./Data/Model/Dragons/Animation/Swim/WD_Swim_Jump.model" ,					"SwimJump" },
		{ "./Data/Model/Dragons/Animation/Swim/WD_Swim_Left.model" ,					"SwimLeft" },
		{ "./Data/Model/Dragons/Animation/Swim/WD_Swim_Right.model" ,					"SwimRight" },
		{ "./Data/Model/Dragons/Animation/Swim/WD_Swim_Turn_Left.model" ,				"SwimTurnLeft" },
		{ "./Data/Model/Dragons/Animation/Swim/WD_Swim_Turn_Right.model" ,			"SwimTurnRight" },
		{ "./Data/Model/Dragons/Animation/Swim/WD_Swim_UnderWater_Backwards.model" ,	"SwimUnderWaterBackwards" },
		{ "./Data/Model/Dragons/Animation/Swim/WD_Swim_Underwater_Forward.model" ,	"SwimUnderWaterForward" },
		{ "./Data/Model/Dragons/Animation/Swim/WD_Swim_UnderWater_Idle.model" ,		"SwimUnderWaterIdle" },
		{ "./Data/Model/Dragons/Animation/Swim/WD_Swim_Underwater_Left.model" ,		"SwimUnderWaterLeft" },
		{ "./Data/Model/Dragons/Animation/Swim/WD_Swim_Underwater_Right.model" ,		"SwimUnderWaterRight" },
		{ "./Data/Model/Dragons/Animation/Swim/WD_Swim_UnderWater_Turn_Left.model" ,	"SwimUnderWaterTurnLeft" },
		{ "./Data/Model/Dragons/Animation/Swim/WD_Swim_UnderWater_Turn_Right.model" ,	"SwimUnderWaterTurnRight" },
#pragma endregion

#pragma region Idle
		{ "./Data/Model/Dragons/Animation/Idle/WD_Idle.model" ,					"Idle01" },
		{ "./Data/Model/Dragons/Animation/Idle/WD_Idle_02_Look.model" ,			"Idle02Look" },
		{ "./Data/Model/Dragons/Animation/Idle/WD_Idle_02_Look_Right.model" ,		"Idle02LookRight" },// R-Y
		{ "./Data/Model/Dragons/Animation/Idle/WD_Idle_03_Shake.model" ,			"Idle03Shake" },
		{ "./Data/Model/Dragons/Animation/Idle/WD_Idle_Combat.model" ,			"IdleCombat" },// R-Y
		{ "./Data/Model/Dragons/Animation/Idle/WD_Idle_Combat_Rage.model" ,		"IdleCombatRage" },// R-Y
		{ "./Data/Model/Dragons/Animation/Idle/WD_Idle_Rage.model" ,				"IdleRage" },// R-Y
#pragma endregion
	};

	DirectX::XMMATRIX ToZUP = DirectX::XMMatrixRotationX(+90.0f / DirectX::XM_PI);
	DirectX::XMMATRIX InvToZUP = DirectX::XMMatrixInverse(nullptr, ToZUP);
	DirectX::XMMATRIX M = DirectX::XMMatrixIdentity();
	// アニメーション追加
	GetModel().lock()->GetResource()->GetAddressAnimations().clear();
	for (auto& [filename, animationName] : animParams)
	{
		GetModel().lock()->GetResource()->AppendAnimations(
			filename,
			animationName);

		// アニメーションを右手Z-UPに変換
		Quaternion q = QuaternionFromRollPitchYaw(Vec3ConvertToRadians(Vector3(90.0f, 0.0f, 0.0f)));
		size_t index = GetModel().lock()->GetResource()->GetAddressAnimations().size() - 1;
		auto& animation = GetModel().lock()->GetResource()->GetAddressAnimations()[index];
		//for (auto& rotation : animation.nodeAnims[0].rotationKeyframes)
		//{
		//	rotation.value = q;
		//}
		for (auto& nodeAnim : animation.nodeAnims)
		{
			//// 位置
			//for (auto& position : nodeAnim.positionKeyframes)
			//{
			//	position.value = Vec3TransformCoord(position.value, 
			//		DirectX::XMMatrixInverse(nullptr, M));
			//	//position.value = Vector3(position.value.x, -position.value.z, position.value.y);
			//}
			//// 回転
			//for (auto& rotation : nodeAnim.rotationKeyframes)
			//{
			//	DirectX::XMMATRIX rot = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&rotation.value));
			//	DirectX::XMMATRIX R = rot * M;
			//	DirectX::XMStoreFloat4(&rotation.value, DirectX::XMQuaternionRotationMatrix(R));
			//}
		}
	}
}

// 使用する角を設定
void DragonActor::SetUseHorn()
{
	auto& nodes = GetModel().lock()->GetAddressPoseNodes();
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
