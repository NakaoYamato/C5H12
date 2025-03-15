#include "SceneManager.h"

#include "SceneLoading.h"

#include<imgui.h>

// 更新処理
void SceneManager::Update(float elapsedTime)
{
	if (_nextScene.get() != nullptr)
	{
		// 古いシーンの終了処理
		Clear();

		// 新しいシーンを設定
		_currentScene = std::move(_nextScene);
		_nextScene.reset();

		// シーン初期化
		if (!(_currentScene->IsReady()))
			_currentScene->Initialize();
	}

	if (_currentScene != nullptr)
	{
		_currentScene->Update(elapsedTime);
	}
}

// 1秒ごとの更新処理
void SceneManager::FixedUpdate()
{
	if (_currentScene != nullptr)
	{
		_currentScene->FixedUpdate();
	}
}

// 描画処理
void SceneManager::Render()
{
	if (_currentScene != nullptr)
	{
		_currentScene->Render();
	}
}

// Gui描画
void SceneManager::DrawGui()
{
	if (_currentScene != nullptr)
	{
		_currentScene->DrawGui();
	}
}

// GUIのタブからシーン変更
void SceneManager::SceneMenuGui()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu(u8"シーン選択"))
		{
			for (auto& [str, scene] : _sceneDatas)
			{
				if (ImGui::MenuItem(str.c_str()))
				{
					SceneManager::Instance().ChangeScene(std::make_shared<SceneLoading>(scene->GetNewShared()));
				}

			}

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}

// シーンクリア
void SceneManager::Clear()
{
	if (_currentScene != nullptr)
	{
		_currentScene->Finalize();
		_currentScene.reset();
	}
}

// シーン切り替え
// REGISTER_SCENE_MANAGERで登録したときの名前から切り替え
void SceneManager::ChangeScene(std::string sceneName)
{
	auto& scene = _sceneDatas[sceneName];
	assert(scene.get());
	SceneManager::Instance().ChangeScene(std::make_shared<SceneLoading>(scene->GetNewShared()));
}

// シーン切り替え
void SceneManager::ChangeScene(std::shared_ptr<Scene> scene)
{
	// 新しいシーンを設定
	_nextScene = scene;
}