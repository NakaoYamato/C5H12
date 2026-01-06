#pragma once

#include "../../Library/Component/CameraControllerBase.h"
#include "../../Library/Camera/Camera.h"
#include "../../Source/Common/Targetable.h"

class LockOnCamera : public CameraControllerBase
{
public:
	LockOnCamera() {}
	~LockOnCamera() override {}
	// 名前取得
	const char* GetName() const override { return "LockOnCamera"; }
	// 開始時処理
	void OnStart() override;
	// 更新時処理
	void OnUpdate(float elapsedTime) override;
	// GUI描画
	void DrawGui() override;

	// ターゲット設定
	void SetTarget(Targetable* target);

private:
	Targetable* _target = nullptr;
};