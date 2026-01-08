#pragma once

#include "../../Library/Component/Component.h"
#include "../../Library/Algorithm/CallBack/CallBack.h"

class InteractionController : public Component
{
public:
	InteractionController() = default;
	~InteractionController() override {}
	// 名前取得
	const char* GetName() const override { return "InteractionController"; }
	// GUI描画処理
	void DrawGui() override;

#pragma region コールバック呼び出し
	// 選択可能か
	// コールバックの戻り値がすべてtrueの場合にtrueを返す
	bool IsSelectable(Actor* target);
	// 選択時
	void OnSelect(Actor* target);
	// 使用可能か
	// コールバックの戻り値がすべてtrueの場合にtrueを返す
	bool IsUsable(Actor* target);
	// 使用時
	void OnUse(Actor* target);
#pragma endregion


#pragma region コールバック登録／解除
	// 選択可能かどうかのコールバック登録
	void RegisterIsSelectableCallback(const std::string& name, CallBack<bool, Actor*> callback) { _isSelectableCallback.RegisterCallBack(name, callback); }
	// 選択時のコールバック登録
	void RegisterOnSelectCallback(const std::string& name, CallBack<void, Actor*> callback) { _onSelectCallback.RegisterCallBack(name, callback); }
	// 使用可能かどうかのコールバック登録
	void RegisterIsUsableCallback(const std::string& name, CallBack<bool, Actor*> callback) { _isUsableCallback.RegisterCallBack(name, callback); }
	// 使用時のコールバック登録
	void RegisterOnUseCallback(const std::string& name, CallBack<void, Actor*> callback) { _onUseCallback.RegisterCallBack(name, callback); }

	// 選択可能かどうかのコールバック解除
	void UnregisterIsSelectableCallback(const std::string& name) { _isSelectableCallback.UnregisterCallBack(name); }
	// 選択時のコールバック解除
	void UnregisterOnSelectCallback(const std::string& name) { _onSelectCallback.UnregisterCallBack(name); }
	// 使用可能かどうかのコールバック解除
	void UnregisterIsUsableCallback(const std::string& name) { _isUsableCallback.UnregisterCallBack(name); }
	// 使用時のコールバック解除
	void UnregisterOnUseCallback(const std::string& name) { _onUseCallback.UnregisterCallBack(name); }
#pragma endregion

private:
	// 選択可能かどうかのコールバック
	CallBackHandler<bool, Actor*> _isSelectableCallback;
	// 選択時のコールバック
	CallBackHandler<void, Actor*> _onSelectCallback;
	// 使用可能かどうかのコールバック
	CallBackHandler<bool, Actor*> _isUsableCallback;
	// 使用時のコールバック
	CallBackHandler<void, Actor*> _onUseCallback;
};