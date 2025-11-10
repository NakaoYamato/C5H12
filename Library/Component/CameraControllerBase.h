#pragma once
#include "../../Library/Component/Component.h"

class CameraControllerBase : public Component
{
public:
	CameraControllerBase() = default;
	~CameraControllerBase() override {}
	// 生成時処理
	void OnCreate() final override;
	// 更新処理
	void LateUpdate(float elapsedTime) final override;
	// 起動時関数
	virtual void OnEntry() {}
	// 終了時処理
	virtual void OnExit() {}

	// このコントローラーに切り替える
	void Swich();
#pragma region アクセサ
	bool IsActive() const
	{
		return _isActive;
	}
	void SetActive(bool isActive)
	{
		_isActive = isActive;
	}
#pragma endregion

protected:
	// 更新時処理
	virtual void OnUpdate(float elapsedTime) = 0;

protected:
	bool _isActive{ false };
};
