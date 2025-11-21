#pragma once

#include <Windows.h>
#include <memory>
#include <unordered_map>
#include <string>

#include "ManagedTarget/KeybordInput.h"
#include "ManagedTarget/XboxPadInput.h"
#include "ManagedTarget/MouseInput.h"
#include "ManagedTarget/DirectInput.h"

// 参考資料:https://qiita.com/tsuchinokoman/items/cbca967f5b6a0a643f84

// 先行宣言
class Input;

// マクロ
#define _INPUT_PRESSED(action)	Input::Instance().IsPressed(action)
#define _INPUT_TRIGGERD(action) Input::Instance().IsTriggerd(action)
#define _INPUT_RELEASED(action) Input::Instance().IsReleased(action)
#define _INPUT_VALUE(action)	Input::Instance().IsValue(action)
#define _Mouse Input::Instance().GetMouseInput()

/// <summary>
/// 入力情報監視クラス
/// コンストラクタでアクションを登録している
/// </summary>
class Input
{
private:
	Input();
	~Input() {};

public:
	// 入力情報の判別用
	enum class InputType
	{
		Keyboard,
		Mouse,
		XboxPad,
		DirectPad
	};

	// 入力対応マップに使用
	struct InputMapInfo
	{
		// 入力種別
		InputType type;
		// 割り当て用
		int buttonID;
	};

	// 入力とアクションの対応テーブル
	using InputActionMap = std::unordered_map<std::string, std::vector<InputMapInfo>>;
public:
	/// <summary>
	/// インプット生成
	/// プログラム開始時に呼び出す
	/// </summary>
	void Initialize(HWND hwnd, HINSTANCE instance);

	/// <summary>
	/// インスタンス取得
	/// </summary>
	static Input& Instance() {
		static Input ins;
		return ins;
	}

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();
	/// <summary>
	/// デバッグGUI表示
	/// </summary>
	void DrawGui();
public:
	/// <summary>
	/// 押されているか確認
	/// </summary>
	/// <param name="action">調べたいアクションの名前</param>
	/// <returns>押されていたらtrue</returns>
	bool IsPressed(const std::string& action)const;
	/// <summary>
	/// 押された瞬間か確認
	/// </summary>
	/// <param name="action">調べたいアクションの名前</param>
	/// <returns>今押されていたらtrue</returns>
	bool IsTriggerd(const std::string& action)const;
	/// <summary>
	/// 離した瞬間か確認
	/// </summary>
	/// <param name="action">調べたいアクションの名前</param>
	/// <returns>今離していたらtrue</returns>
	bool IsReleased(const std::string& action)const;
	/// <summary>
	/// 入力量があるアクションの値取得
	/// </summary>
	/// <param name="action">調べたいアクションの名前</param>
	/// <returns></returns>
	float IsValue(const std::string& action)const;

	/// <summary>
	/// 現在の入力デバイス取得
	/// </summary>
	/// <returns></returns>
	InputType GetCurrentInputDevice() const { return _currentInputDevice; }

	void ClearMapData();
public:
	// アクセサ(基本使わない)
	KeybordInput* GetKeybordInput() { return &_keybordInput;	}
	XboxPadInput* GetGamePadInput() { return &_gamePadInput;	}
	MouseInput* GetMouseInput() { return _mouseInput.get();	}
	DirectInput* GetDirectInput() { return _directInput.get(); }

	const InputActionMap& GetButtonActionMap() const { return _buttonActionMap; }
	const InputActionMap& GetValueActionMap() const { return _valueActionMap; }

private:
	InputActionMap							_buttonActionMap;
	// 現在の押下状態
	std::unordered_map<std::string, BOOL>	_currentInput;
	// 直前の押下状態
	std::unordered_map<std::string, BOOL>	_lastInput;
	// 入力量があるアクション
	InputActionMap							_valueActionMap;

	// 現在の入力量
	std::unordered_map<std::string, float>	_currentMovedParameter;

	// キーボードの入力監視クラス
	KeybordInput					_keybordInput;
	// ゲームパッドの入力監視クラス
	XboxPadInput					_gamePadInput;
	// マウスの入力監視クラス(ウィンドウのハンドルが必要なため動的に確保)
	std::unique_ptr<MouseInput>		_mouseInput;
	// DirectInputの入力監視クラス(ウィンドウのハンドル、インスタンスが必要なため動的に確保)
	std::unique_ptr<DirectInput>	_directInput;

	// 現在の入力デバイス
	InputType _currentInputDevice = InputType::Keyboard;

	// ウィンドウのハンドル
	HWND _hwnd{};
	// GUIの表示フラグ
	bool _showGui = false;
};
