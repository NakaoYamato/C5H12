#include "SceneManager.h"

#include "SceneLoading.h"

#include "../Resource/SerializeFunction.h"

#include<imgui.h>
#include <filesystem>
#include <fstream>

const char* SceneDefaultNamePath = "./Data/Debug/SceneDefaultName.cereal";

void SceneManager::Initialize()
{
#ifdef _DEBUG
	// デバッグビルド時のデフォルトシーンを読み込み
	std::filesystem::path serializePath(SceneDefaultNamePath);
	if (std::filesystem::exists(serializePath))
	{
		std::ifstream istream(serializePath, std::ios::binary);
		if (istream.is_open())
		{
			cereal::BinaryInputArchive archive(istream);

			try
			{
				archive(
					CEREAL_NVP(_defaultSceneName)
				);

				if (_sceneDatas[_defaultSceneName.c_str()].get() == nullptr)
					throw nullptr;

				ChangeScene(_defaultSceneName.c_str());
			}
			catch (...)
			{
				ChangeScene(u8"Debug");
			}
		}
		else
		{
			ChangeScene(u8"Debug");
		}
	}
	else
	{
		ChangeScene(u8"Debug");
	}
#else
	ChangeScene(u8"Debug");
#endif
}

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

			ImGui::Separator();
			// 現在のシーンの名前
			ImGui::Text(u8"現在のシーン : %s", _currentScene->GetName());
			ImGui::Text(u8"起動時のシーン : %s", _defaultSceneName.c_str());
			ImGui::Separator();
			// 起動時のシーンをデフォルトに設定
			if (ImGui::Button(u8"現在のシーンを起動時のシーンに設定"))
			{
				_defaultSceneName = _currentScene->GetName();
				std::ofstream ostream(SceneDefaultNamePath, std::ios::binary);
				if (ostream.is_open())
				{
					cereal::BinaryOutputArchive archive(ostream);

					try
					{
						archive(
							CEREAL_NVP(_defaultSceneName)
						);
					}
					catch (...)
					{
					}
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