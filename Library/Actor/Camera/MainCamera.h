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
	// 開始時処理
	void OnStart() override;
	// 更新処理
	void OnUpdate(float elapsedTime) override;
	// GUI描画
	void OnDrawGui() override;

	// コントローラーの登録
	void RegisterController(CameraControllerRef controller);
	// コントローラーが設定されているかどうか
	bool IsControllerRegistered(const std::string& controllerName) const
	{
		return _cameraControllers.find(controllerName) != _cameraControllers.end();
	}

	// 切り替え(直前のコントローラーに戻す)
	void SwitchPreviousController();
	// 切り替え
	void SwitchController(const std::string& nextControllerName);

	// 現在のコントローラー名取得
	std::string GetCurrentControllerName() const
	{
		return _currentControllerName;
	}
	// 現在のコントローラー取得
	CameraControllerRef GetCurrentController() const
	{
		auto it = _cameraControllers.find(_currentControllerName);
		if (it != _cameraControllers.end())
		{
			return it->second;
		}
		return nullptr;
	}
	// 指定のコントローラー取得
	CameraControllerRef GetControllerByName(const std::string& controllerName) const
	{
		auto it = _cameraControllers.find(controllerName);
		if (it != _cameraControllers.end())
		{
			return it->second;
		}
		return nullptr;
	}
	// 指定のコントローラー取得(型指定)
	template<class T>
	T* GetControllerByClass() const
	{
		for (auto& [name, cont] : _cameraControllers)
		{
			T* controller = dynamic_cast<T*>(cont);
			if (controller)
			{
				return controller;
			}
		}
		return nullptr;
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

	// カメラの垂直操作反転設定
	void SetInvertY(bool isInvert)
	{
		_isInvertY = isInvert;
	}
	// カメラの垂直操作反転取得
	bool IsInvertY() const
	{
		return _isInvertY;
	}
	// カメラの水平操作反転設定
	void SetInvertX(bool isInvert)
	{
		_isInvertX = isInvert;
	}
	// カメラの水平操作反転取得
	bool IsInvertX() const
	{
		return _isInvertX;
	}

private:
	// コントローラー群
	std::unordered_map<std::string, CameraControllerRef> _cameraControllers;
	// 現在のコントローラー名
	std::string _currentControllerName;
	// 次のコントローラー名
	std::string _nextControllerName;

	// コントローラー変更履歴
	std::vector<std::string> _cameraControllerHistory;

	// 更新コールバック関数
	std::vector<std::function<bool(float, MainCamera*)>> _onUpdateCallbacks;
	Vector3 _callbackEyeOffset{};

	// カメラシェイク
	CameraShakeManager _shakeManager;

	// カメラの垂直操作反転フラグ
	bool _isInvertY = false;
	// カメラの水平操作反転フラグ
	bool _isInvertX = false;
};
