#include "PlayerSwordActor.h"

#include "../../Library/Component/ModelRenderer.h"

// 生成時処理
void PlayerSwordActor::OnCreate()
{
	// モデル読み込み
	LoadModel("./Data/Model/Player/Sword/Sword.fbx");

	GetTransform().SetLengthScale(0.01f);
	GetTransform().SetScale(0.2f);
	GetTransform().SetPosition(Vector3(0.456f, 0.111f, -0.048f));
	GetTransform().SetRotation(Vector3::ToRadians(Vector3(0.0f, 0.0f, 90.0f)));
	// コンポーネント追加
	auto modelRenderer = AddComponent<ModelRenderer>();
}
