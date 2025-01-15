#include "SceneManager.h"

#include "SceneLoading.h"

#include<imgui.h>

// 更新処理
void SceneManager::Update(float elapsedTime)
{
	if (nextScene_.get() != nullptr)
	{
		// 古いシーンの終了処理
		Clear();

		// 新しいシーンを設定
		currentScene_ = std::move(nextScene_);
		nextScene_.reset();

		// シーン初期化
		if (!(currentScene_->IsReady()))
			currentScene_->Initialize();
	}

	if (currentScene_ != nullptr)
	{
		currentScene_->Update(elapsedTime);
	}
}

// 1秒ごとの更新処理
void SceneManager::FixedUpdate()
{
	if (currentScene_ != nullptr)
	{
		currentScene_->FixedUpdate();
	}
}

// 描画処理
void SceneManager::Render()
{
	if (currentScene_ != nullptr)
	{
		currentScene_->Render();
	}
}

// Gui描画
void SceneManager::DrawGui()
{
	if (currentScene_ != nullptr)
	{
		currentScene_->DrawGui();
	}
}

// GUIのタブからシーン変更
void SceneManager::SceneMenuGui()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu(u8"シーン選択"))
		{
			for (auto& [str, scene] : sceneDatas_)
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
	if (currentScene_ != nullptr)
	{
		currentScene_->Finalize();
		currentScene_.reset();
	}
}

// シーン切り替え
// REGISTER_SCENE_MANAGERで登録したときの名前から切り替え
void SceneManager::ChangeScene(std::string sceneName)
{
	auto& scene = sceneDatas_[sceneName];
	assert(scene.get());
	SceneManager::Instance().ChangeScene(std::make_shared<SceneLoading>(scene->GetNewShared()));
}

// シーン切り替え
void SceneManager::ChangeScene(std::shared_ptr<Scene> scene)
{
	// 新しいシーンを設定
	nextScene_ = scene;
}