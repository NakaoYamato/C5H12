#pragma once

#include "../../Library/Actor/Actor.h"
#include "../../Library/Component/Component.h"
#include "../../Library/Input/Input.h"

class InputControllerBase;
using InputControllerRef = InputControllerBase*;

class InputManager : public Actor
{
public:
	~InputManager() override {}

	// 生成時処理
	void OnCreate() override;
	// 開始関数
	void OnStart() override;
	// 更新前処理
	void OnPreUpdate(float elapsedTime) override;
	// GUI描画
	void OnDrawGui() override;

	// 入力コントローラーの登録
	void RegisterInputController(InputControllerRef controller);
	// 入力の切り替え(直前の入力コントローラーに戻す)
	void SwitchPreviousInput();
	// 入力の切り替え
	void SwitchInput(const std::string& nextInputName);

private:
	// 入力コントローラー群
	std::unordered_map<std::string, InputControllerRef> _inputControllers;
	// 現在の入力コントローラー名
	std::string _currentInputControllerName;
	// 次の入力コントローラー名
	std::string _nextInputControllerName;

	// 入力コントローラー変更履歴
	std::vector<std::string> _inputControllerHistory;
};


class InputControllerBase : public Component
{
public:
	InputControllerBase() = default;
	~InputControllerBase() override {}

	// 生成時処理
	void OnCreate() override;
	// 更新処理
	void Update(float elapsedTime) final override;
	// 起動時関数
	virtual void OnExecute() {}
	// 終了時処理
	virtual void OnEnd() {}

	// この入力に切り替える
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
	void SetInputManager(InputManager* inputManager)
	{
		_inputManager = inputManager;
	}
#pragma endregion

protected:
	// 更新時処理
	virtual void OnUpdate(float elapsedTime) = 0;

protected:
	// Mediatorの参照
	InputManager* _inputManager = nullptr;
	// 起動状態
	bool _isActive = false;
};
