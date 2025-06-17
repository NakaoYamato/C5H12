#pragma once

#include "../../Library/Actor/Actor.h"
#include "../../Library/Component/Component.h"
#include "../../Library/Input/Input.h"

class InputControllerBase;
using InputControllerRef = InputControllerBase*;

class InputMediator : public Actor
{
public:
	enum class CommandType
	{
		StartGame,
		OpenMenu,
		CloseMenu,
	};

	struct CommandData
	{ 
		// コマンドを送信したInputController
		const InputControllerBase* sender	= nullptr;
		// コマンドの種類
		CommandType commandType				= CommandType::StartGame;
		// コマンドの内容
		std::string command					= "";
	};

	static const char* PlayerInputName;
	static const char* MenuInputName;

public:
	~InputMediator() override {}

	// 開始関数
	void OnStart() override;
    // 更新前処理
    void OnPreUpdate(float elapsedTime) override;
    // GUI描画
    void OnDrawGui() override;

	// InputControllerBaseからの命令を受信
	void ReceiveCommand(const InputControllerBase* sender, CommandType commandType, const std::string& command);

	// 入力コントローラーの登録
	void RegisterInputController(InputControllerRef controller)
	{
		_inputControllers.push_back(controller);
	}
	// 入力コントローラーの削除
	void UnregisterInputController(InputControllerRef controller)
	{
		auto it = std::remove(_inputControllers.begin(), _inputControllers.end(), controller);
		if (it != _inputControllers.end())
		{
			_inputControllers.erase(it, _inputControllers.end());
		}
	}

private:
	// 入力コントローラーのリスト
	std::vector<InputControllerRef> _inputControllers;
	// コマンド履歴
	std::vector<CommandData> _commandHistory;
};

class InputControllerBase : public Component
{
public:
	InputControllerBase() = default;
	~InputControllerBase() override;
	// 他のInputControllerから命令を受信
	virtual void ReceiveCommandFromOther(InputMediator::CommandType commandType, const std::string& command) = 0;

#pragma region アクセサ
	bool IsActive() const
	{
		// InputMediatorが存在しない場合は常にアクティブ
		if (_inputMediator.lock() == nullptr)
			return true;

		return _isActive;
	}
	void SetActive(bool isActive)
	{
		_isActive = isActive;
	}
#pragma endregion

protected:
	// InputMediatorを探す
	bool FindInputMediator();

protected:
    // InputMediatorへの参照
	std::weak_ptr<InputMediator> _inputMediator;
	// 起動状態
	bool _isActive = false;
};