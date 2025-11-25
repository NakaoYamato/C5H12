#pragma once

#include "../../Library/Actor/Actor.h"
#include "../../Library/Component/Component.h"
#include "../../Library/Camera/Camera.h"
#include "../../Library/Camera/CameraShake.h"

#include <functional>

class CameraControllerBase;
using CameraControllerRef = CameraControllerBase*;

class MainCamera : public Actor
{
public:
	~MainCamera()override {}

	// 生成時処理
	void OnCreate() override;

	// 更新処理
	void OnUpdate(float elapsedTime) override;

	// GUI描画
	void OnDrawGui() override;

	// 指定方向を向く
	void SetLookAt(const Vector3& eye, const Vector3& focus, const Vector3& up);

	// パースペクティブ設定
	void SetPerspectiveFov(float fovY, float aspect, float nearZ, float farZ);

	// コントローラーの登録
	void RegisterCameraController(CameraControllerRef controller);
	// 切り替え(直前のコントローラーに戻す)
	void SwitchPreviousController();
	// 切り替え
	void SwitchController(const std::string& nextControllerName);
	// コントローラーが設定されているかどうか
	bool IsControllerRegistered(const std::string& controllerName) const
	{
		return _cameraControllers.find(controllerName) != _cameraControllers.end();
	}

	// 更新処理追加
	// 戻り値がtrueの場合、登録解除される
	void AddOnUpdateCallback(const std::function<bool(float, MainCamera*)>& callback);
	// コールバック用目のオフセット追加
	void AddEyeOffset(const Vector3& offset)
	{
		_callbackEyeOffset += offset;
	}
	// コールバック用目のオフセット取得
	const Vector3& GetCallBackEyeOffset() const { return _callbackEyeOffset; }

	// シェイクマネージャー取得
	CameraShakeManager* GetShakeManager() { return &_shakeManager; }

private:
	// コントローラー群
	std::unordered_map<std::string, CameraControllerRef> _cameraControllers;
	// 現在の入力コントローラー名
	std::string _currentCameraControllerName;
	// 次の入力コントローラー名
	std::string _nextCameraControllerName;

	// コントローラー変更履歴
	std::vector<std::string> _cameraControllerHistory;

	// 更新コールバック関数
	std::vector<std::function<bool(float, MainCamera*)>> _onUpdateCallbacks;
	Vector3 _callbackEyeOffset{};

	// カメラシェイク
	CameraShakeManager _shakeManager;
};
