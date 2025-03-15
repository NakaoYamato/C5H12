#pragma once

#include <Windows.h>
#include <memory>
#include <unordered_map>
#include <string>

#include "ManagedTarget/KeybordInput.h"
#include "ManagedTarget/GamePadInput.h"
#include "ManagedTarget/MouseInput.h"

// 参考資料:https://qiita.com/tsuchinokoman/items/cbca967f5b6a0a643f84

// 先行宣言
class Input;

// マクロ
#define INPUT_IS_PRESSED(action) Input::Instance().IsPressed(action)
#define INPUT_IS_TRIGGERD(action) Input::Instance().IsTriggerd(action)
#define INPUT_IS_RELEASED(action) Input::Instance().IsReleased(action)
#define INPUT_IS_MOVED(action) Input::Instance().IsMoved(action)

/// <summary>
/// 入力情報監視クラス
/// コンストラクタでアクションを登録している
/// </summary>
class Input
{
private:
	Input();
	~Input() {};

	// 入力情報の判別用
	enum class InputType
	{
		InputKeyboard,
		InputGamepad,
		InputMouse
	};

	// 入力対応マップに使用
	struct InputMapInfo
	{
		// 入力種別
		InputType type;
		// 割り当て用
		int buttonID;
	};

public:
	/// <summary>
	// インプット生成
	// プログラム開始時に呼び出す
	/// </summary>
	void Initialize(HWND hwnd);

public:
	/// <summary>
	// インスタンス取得
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
	float IsMoved(const std::string& action)const;

	void ClearMapData();
public:
	// アクセサ(基本使わない)
	KeybordInputObserver* GetKeybordInput() { return &_keybordInputObserver;	}
	GamePadInputObserver* GetGamePadInput() { return &_gamePadInputObserver;	}
	MouseInputObserver* GetMouseInput() { return _mouseInputObserver.get();	}

private:
	// 入力とアクションの対応テーブル
	using InputActionMap = std::unordered_map<std::string, std::vector<InputMapInfo>>;
	InputActionMap _inputActionMap;
	// 現在の押下状態
	std::unordered_map<std::string, BOOL> _currentInput;
	// 直前の押下状態
	std::unordered_map<std::string, BOOL> _lastInput;

	/// 入力量があるアクション
	InputActionMap _moveActionMap;
	// 現在の入力量
	std::unordered_map<std::string, float> _currentMovedParameter;

	// キーボードの入力監視クラス
	KeybordInputObserver _keybordInputObserver;
	// ゲームパッドの入力監視クラス
	GamePadInputObserver _gamePadInputObserver;
	// マウスの入力監視クラス(ウィンドウのハンドルが必要なため動的に確保)
	std::unique_ptr<MouseInputObserver> _mouseInputObserver;

	// ウィンドウのハンドル
	HWND _hwnd{};

	// GUIの表示フラグ
	bool _showGui = false;
};
