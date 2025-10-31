#include "PlayerGreatSwordActor.h"

#include "../../Library/Component/ModelRenderer.h"

void PlayerGreatSwordActor::OnCreate()
{
	WeaponActor::OnCreate();
	// モデル読み込み
	LoadModel(GetModelFilePath());

	// コンポーネント追加
	auto modelRenderer = AddComponent<ModelRenderer>();

	GetTransform().SetPosition(Vector3(-3.6f, 1.7f, 0.8f));
	GetTransform().SetScale(Vector3(1.5f, 1.5f, 1.5f));
	GetTransform().SetAngle(Vector3(5.0f, 180.0f, -12.0f).ToRadians());

	_locusRootLocalPosition = Vector3(0.0f, -20.0f, 0.0f);
	_locusTipLocalPosition = Vector3(0.0f, -130.0f, 0.0f);
}

// 開始時処理
void PlayerGreatSwordActor::OnStart()
{
	WeaponActor::OnStart();
	_modelRenderer = GetComponent<ModelRenderer>();
	if (GetParent())
	{
		_parentModelRenderer = GetParent()->GetComponent<ModelRenderer>();
	}

	// リムライト初期化
	if (_modelRenderer.lock())
	{
		for (auto& material : _modelRenderer.lock()->GetMaterials())
		{
			material.SetShaderName("Player");
			// シェーダー変更時はパラメータも初期化
			material.SetParameterMap(GetScene()->GetMeshRenderer().GetShaderParameterKey(
				_modelRenderer.lock()->GetRenderType(),
				"Player",
				true));
			Vector4 color = Vector4::Black;
			color.w = 0.0f;
			material.SetParameter("bodyColor", color);
		}
	}
}

void PlayerGreatSwordActor::OnUpdate(float elapsedTime)
{
	WeaponActor::OnUpdate(elapsedTime);
	// リムライト処理
	if (_modelRenderer.lock() && _parentModelRenderer.lock())
	{
		Vector4 color = *_parentModelRenderer.lock()->GetMaterials().at(0).GetParameterF4("bodyColor");

		if (_locusPushTimer > 0.0f)
			color.w = _rimLightFactor;

		for (auto& material : _modelRenderer.lock()->GetMaterials())
		{
			material.SetParameter("bodyColor", color);
		}
	}
}
