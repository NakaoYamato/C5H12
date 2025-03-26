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

		static const char* filenames[] =
		{
			"./Data/Model/Dragons/Animation/Attacks/WD_Attack_FireBreath.fbx",
			"./Data/Model/Dragons/Animation/Idle/WD_Idle.fbx",
			"./Data/Model/Dragons/Animation/Idle/WD_Idle_02_Look.fbx",
			"./Data/Model/Dragons/Animation/Idle/WD_Idle_02_Look_Right.fbx",
			"./Data/Model/Dragons/Animation/Idle/WD_Idle_03_Shake.fbx",
			"./Data/Model/Dragons/Animation/Idle/WD_Idle_Combat.fbx",
			"./Data/Model/Dragons/Animation/Idle/WD_Idle_Combat_Rage.fbx",
			"./Data/Model/Dragons/Animation/Idle/WD_Idle_Rage.fbx",
		};

		for (auto& filename : filenames)
		{
			modelRenderer->GetModel()->GetResource()->AppendAnimations(
				filename,
				rootStartAngle);
		}

		modelRenderer->GetModel()->ReSerialize();
	}
}
