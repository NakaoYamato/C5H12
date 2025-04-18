#include "DragonActor.h"

#include "../../Library/Graphics/Graphics.h"
#include "../../Scene/Scene.h"
#include "../../Library/Component/ModelRenderer.h"
#include "../../Library/Component/Animator.h"

#include "../../Library/ResourceManager/GpuResourceManager.h"

// 生成時処理
void DragonActor::OnCreate()
{
	GetTransform().SetCoordinateType(COORDINATE_TYPE::RHS_Z_UP);
	GetTransform().SetLengthScale(0.01f);

	// コンポーネント追加
	auto modelRenderer = AddComponent<ModelRenderer>("./Data/Model/Dragons/Kuzar the Magnificent.fbx");
	auto animator = AddComponent<Animator>(modelRenderer->GetModel());

	// テクスチャ設定
	if (true)
	{
		auto resource = modelRenderer->GetModel()->GetResource();
		auto SetModelSRV = [&](const wchar_t* filename, std::string materialName, std::string textureKey)
			{
				// テクスチャからSRV作成
				Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
				GpuResourceManager::LoadTextureFromFile(Graphics::Instance().GetDevice(),
					filename,
					srv.ReleaseAndGetAddressOf(),
					nullptr);

				// SRV設定
				resource->ChangeMaterialSRV(srv, materialName, textureKey);
			};

#pragma region Body
		SetModelSRV(L"./Data/Model/Dragons/Texture/T_Kuzar_Red_Body_Albedo.png",	"Material #25", "Diffuse");
		SetModelSRV(L"./Data/Model/Dragons/Texture/T_Kuzar_Body_Normal.png",		"Material #25", "Normal");
		SetModelSRV(L"./Data/Model/Dragons/Texture/T_Kuzar_Body_EmissionBW.png",	"Material #25", "Emissive");
#pragma endregion
#pragma region Spikes
		SetModelSRV(L"./Data/Model/Dragons/Texture/T_Kuzar_Spikes_Albedo.png",		"Material #26", "Diffuse");
		SetModelSRV(L"./Data/Model/Dragons/Texture/T_Kuzar_Spikes_Normal.png",		"Material #26", "Normal");
#pragma endregion
#pragma region Eyes
		SetModelSRV(L"./Data/Model/Dragons/Texture/T_Dragon_Eyes_Albedo.png",		"Eyes", "Diffuse");
		SetModelSRV(L"./Data/Model/Dragons/Texture/T_Dragon_Eyes_Normal.png",		"Eyes", "Normal");
#pragma endregion
#pragma region Wings
		SetModelSRV(L"./Data/Model/Dragons/Texture/T_Kuzar_Red_Wings_Albedo.png",	"Material #31", "Diffuse");
		SetModelSRV(L"./Data/Model/Dragons/Texture/T_Kuzar_Wings_Normal.png",		"Material #31", "Normal");
		SetModelSRV(L"./Data/Model/Dragons/Texture/T_Kuzar_Wings_EmissionBW.png",	"Material #31", "Emissive");
#pragma endregion
#pragma region Horns
		SetModelSRV(L"./Data/Model/Dragons/Texture/T_Kuzar_Horns_Color.png",		"Horns", "Diffuse");
		SetModelSRV(L"./Data/Model/Dragons/Texture/T_Kuzar_Horns_Normal.png",		"Horns", "Normal");
		SetModelSRV(L"./Data/Model/Dragons/Texture/T_Kuzar_Horns_Emissive.png",		"Horns", "Emissive");
#pragma endregion
		modelRenderer->GetModel()->ReSerialize();
	}

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
	if (modelRenderer->GetModel()->GetResource()->GetAnimations().size() != _countof(animParams))
	{
		modelRenderer->GetModel()->GetResource()->GetAddressAnimations().clear();
		for (auto& [filename, animationName] : animParams)
		{
			modelRenderer->GetModel()->GetResource()->AppendAnimations(
				filename,
				animationName);
		}

		modelRenderer->GetModel()->ReSerialize();
	}
}
