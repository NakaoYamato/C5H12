#include "PlayerShieldActor.h"

#include "../../Library/Component/ModelRenderer.h"

void PlayerShieldActor::OnCreate()
{
	// モデル読み込み
	LoadModel("./Data/Model/Player/Shield/Shield.fbx");

	GetTransform().SetLengthScale(0.05f);
	GetTransform().SetPosition(Vector3(0.0f, 0.1f, -0.1f));
	GetTransform().SetRotation(Vector3::ConvertToRadians(Vector3(0.0f, -90.0f, 80.0f)));
	// コンポーネント追加
	auto modelRenderer = AddComponent<ModelRenderer>();
}
