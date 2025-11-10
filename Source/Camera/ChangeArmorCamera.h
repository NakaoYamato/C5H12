#pragma once

#include "../../Library/Component/CameraControllerBase.h"

class ChangeArmorCamera : public CameraControllerBase
{
public:
	ChangeArmorCamera() {}
	~ChangeArmorCamera() override {}
	// 名前取得
	const char* GetName() const override { return "ChangeArmorCamera"; }
	// 開始処理
	void Start() override;
	// 更新時処理
	void OnUpdate(float elapsedTime) override;
	// Gui描画
	void DrawGui() override;

	void SetChestActor(const std::shared_ptr<Actor>& chestActor)
	{
		_chestActor = chestActor;
	}

private:
	std::weak_ptr<Actor> _chestActor;

	float _cameraOffsetY = 1.0f;
	float _cameraLength = 2.5f;
};
