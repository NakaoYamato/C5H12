#include "ChestActor.h"

#include "../../Library/Scene/Scene.h"
#include "../../Library/Component/ModelRenderer.h"

// 生成時処理
void ChestActor::OnCreate()
{
	// モデルロード
	LoadModel("./Data/Model/Stage/Props/Chest/ChestBottom.fbx");

	// コンポーネント追加
	this->AddComponent<ModelRenderer>();

	// 蓋生成
	auto top = GetScene()->RegisterActor<Actor>(std::string(GetName()) + "Top", GetTag());
	top->LoadModel("./Data/Model/Stage/Props/Chest/ChestTop.fbx");
	top->SetParent(this);
	top->GetTransform().SetPositionY(0.538f);
	top->GetTransform().SetPositionZ(0.245f);
	// コンポーネント追加
	top->AddComponent<ModelRenderer>();
}
