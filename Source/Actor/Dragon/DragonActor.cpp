#include "DragonActor.h"

#include "../../Library/Graphics/Graphics.h"
#include "../../Scene/Scene.h"
#include "../../Library/Component/ModelRenderer.h"
#include "../../Library/Component/Animator.h"

// 生成時処理
void DragonActor::OnCreate()
{
	GetTransform().SetCoordinateType(COORDINATE_TYPE::RHS_Z_UP);
	GetTransform().SetLengthScale(0.01f);

	// コンポーネント追加
	auto modelRenderer = AddComponent<ModelRenderer>("./Data/Model/Dragons/Dragons.fbx");
	auto animator = AddComponent<Animator>(modelRenderer->GetModel());

	// アニメーション追加
	if (modelRenderer->GetModel()->GetResource()->GetAnimations().size() == 0)
	{
		DirectX::XMFLOAT3 rootStartAngle = { 0.0f, 0.0f, 0.0f };

		static std::tuple<const char*, const char*, DirectX::XMFLOAT3> animParams[] =
		{
			{ "./Data/Model/Dragons/Animation/Attacks/WD_Attack_Bite_Front.fbx" ,	"AttackBiteFront" ,	{ 0.0f, 0.0f, 0.0f } },
			{ "./Data/Model/Dragons/Animation/Attacks/WD_Attack_Bite_Left.fbx" ,	"AttackBiteLeft" ,	{ 0.0f, 0.0f, 0.0f } },
			{ "./Data/Model/Dragons/Animation/Attacks/WD_Attack_Bite_Right.fbx" ,	"AttackBiteRight" ,	{ 0.0f, 0.0f, 0.0f } },
			{ "./Data/Model/Dragons/Animation/Attacks/WD_Attack_FireBall.fbx" ,		"AttackFireBall" ,	{ 0.0f, 0.0f, 0.0f } },
			{ "./Data/Model/Dragons/Animation/Attacks/WD_Attack_FireBreath.fbx" ,	"AttackFireBreath" ,	{ 0.0f, 0.0f, 0.0f } },
			{ "./Data/Model/Dragons/Animation/Attacks/WD_Attack_FireBreath_Swipe.fbx" ,	"AttackFireSwipe" ,	{ 0.0f, 0.0f, 0.0f } },
			{ "./Data/Model/Dragons/Animation/Attacks/WD_Attack_Fly_Legs Mask.fbx" ,	"AttackFlyLegsMask" ,	{ 0.0f, 0.0f, 0.0f } },
			{ "./Data/Model/Dragons/Animation/Attacks/WD_Attack_Fly_Melee Mask.fbx" ,	"AttackFlyMeleeMask" ,	{ 0.0f, 0.0f, 0.0f } },
			{ "./Data/Model/Dragons/Animation/Attacks/WD_Attack_Special1.fbx" ,		"AttackSpecial1" ,	{ 90.0f, 0.0f, 0.0f } },
			{ "./Data/Model/Dragons/Animation/Attacks/WD_Attack_Tail_Left.fbx" ,	"AttackTailLeft" ,	{ 0.0f, 0.0f, 0.0f } },
			{ "./Data/Model/Dragons/Animation/Attacks/WD_Attack_Tail_Right.fbx" ,	"AttackTailRight" ,	{ 0.0f, 0.0f, 0.0f } },
			{ "./Data/Model/Dragons/Animation/Attacks/WD_Attack_Wing_Fist_Left.fbx" ,	"AttackWingFistLeft" ,	{ 0.0f, 0.0f, 0.0f } },
			{ "./Data/Model/Dragons/Animation/Attacks/WD_Attack_Wing_Fist_Right.fbx" ,	"AttackWingFistRight" ,	{ 0.0f, 0.0f, 0.0f } },
			{ "./Data/Model/Dragons/Animation/Attacks/WD_Attack_Wing_Swipe_Left.fbx" ,	"AttackWingSwipeLeft" ,	{ 90.0f, 0.0f, 0.0f } },
			{ "./Data/Model/Dragons/Animation/Attacks/WD_Attack_Wing_Swipe_Right.fbx" ,	"AttackWingSwipeRight" ,	{ 90.0f, 0.0f, 0.0f } },
			{ "./Data/Model/Dragons/Animation/Attacks/WD_FireMask.fbx" ,			"FireMask" ,	{ 0.0f, 0.0f, 0.0f } },
			
			{ "./Data/Model/Dragons/Animation/Idle/WD_Idle.fbx" ,					"Idle01" ,				{ 0.0f, 0.0f, 0.0f } },
			{ "./Data/Model/Dragons/Animation/Idle/WD_Idle_02_Look.fbx" ,			"Idle02Look" ,			{ 0.0f, 0.0f, 0.0f } },
			{ "./Data/Model/Dragons/Animation/Idle/WD_Idle_02_Look_Right.fbx" ,		"Idle02LookRight" ,		{ 90.0f, 0.0f, 0.0f } },
			{ "./Data/Model/Dragons/Animation/Idle/WD_Idle_03_Shake.fbx" ,			"Idle03Shake" ,			{ 0.0f, 0.0f, 0.0f } },
			{ "./Data/Model/Dragons/Animation/Idle/WD_Idle_Combat.fbx" ,			"IdleCombat" ,			{ 90.0f, 0.0f, 0.0f } },
			{ "./Data/Model/Dragons/Animation/Idle/WD_Idle_Combat_Rage.fbx" ,		"IdleCombatRage" ,		{ 90.0f, 0.0f, 0.0f } },
			{ "./Data/Model/Dragons/Animation/Idle/WD_Idle_Rage.fbx" ,				"IdleRage" ,			{ 90.0f, 0.0f, 0.0f } },
		};
		for (auto& [filename, animationName, rootStartAng] : animParams)
		{
			modelRenderer->GetModel()->GetResource()->AppendAnimations(
				filename,
				animationName,
				rootStartAng);
		}

		modelRenderer->GetModel()->ReSerialize();
	}
}
