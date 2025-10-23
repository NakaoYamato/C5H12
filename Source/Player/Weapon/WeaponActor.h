#pragma once

#include "../../Library/Actor/ModelAttach/ModelAttachActor.h"

#include "../../Library/Model/ModelResource.h"
#include "../../Library/Component/Collider/ModelCollider.h"
#include "../../Library/Component/LocusRenderer.h"

class WeaponActor : public ModelAttachActor
{
public:
	~WeaponActor()override {}

	// 生成時処理
	void OnCreate() override;
	// 開始時処理
	void OnStart() override;
	// 遅延更新時処理
	void OnLateUpdate(float elapsedTime) override;
	// GUI描画時処理
	void OnDrawGui() override;
protected:
	std::weak_ptr<ModelCollider>	_ownerModelCollider; // 親のモデルコライダー
	std::weak_ptr<LocusRenderer>	_locusRenderer; // 軌跡レンダラー

	Vector3 _oldPosition = Vector3::Zero; // 前回の位置(ワールド座標)
	Vector3 _locusRootLocalPosition = Vector3::Zero; // 軌跡のルート位置(ローカル座標)
	Vector3 _locusTipLocalPosition = Vector3::Up; // 軌跡の先端位置(ローカル座標)
    float _locusLifeTime = 0.1f; // 軌跡のライフタイム
	float _locusPushInterval = 0.01f; // 軌跡の位置を追加する間隔(秒)
	float _locusPushTimer = 0.0f; // 軌跡の位置を追加するタイマー
	bool _isDrawingLocusPosition = true; // 軌跡の位置を描画するかどうか
};