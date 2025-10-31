#pragma once

#include "Component.h"

#include <functional>

class CameraEventReceiver : public Component
{
public:
	CameraEventReceiver() {}
	virtual ~CameraEventReceiver() override {}
	// 名前取得
	const char* GetName() const override { return "CameraEventReceiver"; }

	// 更新処理
	void Update(float elapsedTime) override;
	// GUI描画
	void DrawGui() override;

	// 更新処理追加
	// 戻り値がtrueの場合、登録解除される
	void AddOnUpdateCallback(const std::function<bool(float, CameraEventReceiver*)>& callback)
	{
		_onUpdateCallbacks.push_back(callback);
	}

	void AddEyeOffset(const Vector3& offset)
	{
		_eyeOffset += offset;
	}

	const Vector3& GetEyeOffset() const
	{
		return _eyeOffset;
	}
private:
	std::vector<std::function<bool(float, CameraEventReceiver*)>> _onUpdateCallbacks;

	Vector3 _eyeOffset{};
};