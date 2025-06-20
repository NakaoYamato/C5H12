#include "PlayerShieldActor.h"

#include "../../Library/Component/ModelRenderer.h"

void PlayerShieldActor::OnCreate()
{
	// モデル読み込み
	LoadModel(GetModelFilePath());

	GetTransform().SetLengthScale(0.01f);
	GetTransform().SetPosition(Vector3(0.0f, -0.2f, 0.1f));
	GetTransform().SetRotation(Vector3::ToRadians(Vector3(-18.0f, 0.0f, 0.0f)));
	//GetTransform().SetLengthScale(0.05f);
	//GetTransform().SetPosition(Vector3(-0.042f, 0.129f, 0.053f));
	//GetTransform().SetRotation(Vector3::ToRadians(Vector3(0.0f, 90.0f, 80.0f)));
	// コンポーネント追加
	auto modelRenderer = AddComponent<ModelRenderer>();
}
