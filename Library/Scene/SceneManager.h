#pragma once

#include <memory>
#include <unordered_map>
#include <string>

#include "Scene.h"

// シーンマネージャーのGUIに登録するための構造体
template<class T, typename... Arg>
struct SceneRegister
{
	SceneRegister(const std::string& str, Arg... arg);
};

// シーンマネージャーのGUIに登録するためのマクロ
// type : Scene継承クラス
// sceneName : GUIに登録する名前
#define _REGISTER_SCENE_MANAGER(type, sceneName) \
static SceneRegister<type> type##register(sceneName)

//---------------------------------------------------
//  シーン管理クラス
//---------------------------------------------------
class SceneManager
{
private:
	SceneManager() {}
	~SceneManager() {}

public:
	// インスタンス取得
	static SceneManager& Instance() {
		static SceneManager ins_;
		return ins_;
	}

	// 初期化
	void Initialize();

	// 更新処理
	void Update(float elapsedTime);

	// 1秒ごとの更新処理
	void FixedUpdate();

	// 描画処理
	void Render();

	// Gui描画
	void DrawGui();

	// GUIのタブからシーン変更
	void SceneMenuGui();

	// シーンクリア
	void Clear();

	// シーン切り替え
	// REGISTER_SCENE_MANAGERで登録したときの名前から切り替え
	void ChangeScene(std::string sceneName);

	// シーン切り替え
	void ChangeScene(std::shared_ptr<Scene> scene);

	// デバッグ用シーンに登録
	template<class T, typename... Arg>
	void PushScene(const std::string& str, Arg... arg)
	{
		_sceneDatas[str] = std::make_unique<T>(arg...);
	}
	
private:
	std::shared_ptr<Scene> _currentScene;
	std::shared_ptr<Scene> _nextScene;

	// デバッグ用で保存しているシーン
	std::unordered_map<std::string, std::unique_ptr<Scene>> _sceneDatas;

	// Debugビルドでのデフォルトシーン
	std::string _defaultSceneName = "";
};

template<class T, typename ...Arg>
inline SceneRegister<T, Arg...>::SceneRegister(const std::string& str, Arg ...arg)
{
	SceneManager::Instance().PushScene<T>(str, arg...);
}
