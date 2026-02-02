#pragma once
#include "../../Library/Component/Component.h"

class CameraControllerBase : public Component
{
public:
	CameraControllerBase() = default;
	~CameraControllerBase() override {}
	// 開始処理
	void Start() final override;
	// 更新処理
	void LateUpdate(float elapsedTime) final override;
	// 起動時関数
	virtual void OnActivate() {}
	// 終了時処理
	virtual void OnDeactivate() {}
	// Gui描画
	virtual void DrawGui() override;

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
	bool CanSkip() const
	{
		return _canSkip;
	}
	void SetCanSkip(bool canSkip)
	{
		_canSkip = canSkip;
	}
#pragma endregion

protected:
	// 開始時処理
	virtual void OnStart() = 0;
	// 更新時処理
	virtual void OnUpdate(float elapsedTime) = 0;

protected:
	bool _isActive = false;
	// スキップ可能か
	bool _canSkip = false;
};
