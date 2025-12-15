#pragma once

#include "../../Library/Actor/ModelAttach/ModelAttachActor.h"

#include "../../Library/Component/ModelRenderer.h"
#include "../../Library/Component/Collider/ModelCollider.h"
#include "../../Library/Component/LocusRenderer.h"
#include "SharpnessController.h"

#include "../../Source/User/UserDataManager.h"

class WeaponActor : public ModelAttachActor
{
public:
	WeaponActor(WeaponType type, int weaponIndex = 0) :
		_weaponType(type),
		_weaponIndex(weaponIndex)
	{
	}
	~WeaponActor()override {}

	// 生成時処理
	void OnCreate() override;
	// 開始時処理
	void OnStart() override;
	// 遅延更新時処理
	void OnLateUpdate(float elapsedTime) override;
	// GUI描画時処理
	void OnDrawGui() override;

	// データ構築
	void BuildData(int index);

	// リムライトを上書きするか
	void SetIsOverrideRimLight(bool isOverride) { _isOverrideRimLight = isOverride; }
	// 上書きするリムライトの色設定
	void SetRimLightColor(const Vector4& color) { _rimLightColor = color; }
protected:
	WeaponType _weaponType = WeaponType::GreatSword;
	int _weaponIndex = 0;

	std::weak_ptr<ModelRenderer>	_modelRenderer;
	// 親のモデルコライダー
	std::weak_ptr<ModelCollider>	_ownerModelCollider;
	// 軌跡レンダラー
	std::weak_ptr<LocusRenderer>	_locusRenderer;
	// 斬れ味コントローラー
	std::weak_ptr<SharpnessController> _sharpnessController;
	// ユーザーデータマネージャー
	std::weak_ptr<UserDataManager> _userDataManager;

#pragma region 軌跡
	Vector3 _oldPosition = Vector3::Zero; // 前回の位置(ワールド座標)
	Vector3 _locusRootLocalPosition = Vector3::Zero; // 軌跡のルート位置(ローカル座標)
	Vector3 _locusTipLocalPosition = Vector3::Up; // 軌跡の先端位置(ローカル座標)
	float _locusLifeTime = 0.2f; // 軌跡のライフタイム
	float _locusPushInterval = 0.01f; // 軌跡の位置を追加する間隔(秒)
	float _locusPushTimer = 0.0f; // 軌跡の位置を追加するタイマー
	bool _isDrawingLocusPosition = true; // 軌跡の位置を描画するかどうか
#pragma endregion

#pragma region リムライト
	// リムライトを上書きするか
	bool _isOverrideRimLight = false;
	// 上書きするリムライトの色
	Vector4 _rimLightColor = Vector4::White;
#pragma endregion
};