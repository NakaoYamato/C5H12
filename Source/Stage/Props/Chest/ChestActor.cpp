#include "ChestActor.h"

#include "../../Library/Scene/Scene.h"
#include "../../Library/Component/ModelRenderer.h"
#include "../../Library/Component/Collider/SphereCollider.h"

#include "../../Source/Camera/ChangeArmorCamera.h"

#include "ChestController.h"

// 生成時処理
void ChestActor::OnCreate()
{
	// モデルロード
	LoadModel("./Data/Model/Stage/Props/Chest/ChestBottom.fbx");

	// コンポーネント追加
	this->AddComponent<ModelRenderer>();
	auto controller = this->AddComponent<ChestController>();

	// コライダー追加
	auto bodyCollider = this->AddCollider<SphereCollider>();
	auto judgeCollider = this->AddCollider<SphereCollider>();

	// パラメータ設定
	GetTransform().SetScale(2.0f);
	bodyCollider->SetLayer(CollisionLayer::Stage);
	bodyCollider->SetTrigger(false);
	bodyCollider->SetPosition(Vector3(0.0f, 0.3f, 0.0f));
	bodyCollider->SetRadius(1.0f);
	judgeCollider->SetLayer(CollisionLayer::Stage);
	judgeCollider->SetTrigger(true);
	judgeCollider->SetPosition(Vector3(0.0f, 0.3f, 0.0f));
	judgeCollider->SetRadius(2.0f);

	// 蓋生成
	auto top = GetScene()->RegisterActor<Actor>(std::string(GetName()) + "Top", GetTag());
	top->LoadModel("./Data/Model/Stage/Props/Chest/ChestTop.fbx");
	top->SetParent(this);
	top->GetTransform().SetPositionY(0.538f);
	top->GetTransform().SetPositionZ(0.245f);
	// コンポーネント追加
	top->AddComponent<ModelRenderer>();

	// カメラ生成
	if (GetScene()->GetMainCameraActor()->IsControllerRegistered("ChangeArmorCamera") == false)
	{
		auto camera = GetScene()->RegisterActor<Actor>("ChangeArmorCamera", ActorTag::System);
		camera->AddComponent<ChangeArmorCamera>();
	}
}
