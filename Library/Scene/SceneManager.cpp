#include "SceneManager.h"

#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Graphics/GpuResourceManager.h"
#include "SceneLoading.h"

#include "../Model/SerializeFunction.h"

#include<imgui.h>
#include <filesystem>
#include <fstream>

const char* SceneDefaultNamePath = "./Data/Debug/SceneDefaultName.cereal";

// 初期化
void SceneManager::Initialize()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11Device* device = graphics.GetDevice();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();

	// サンプラーステート設定
	{
		std::lock_guard<std::mutex> lock(Graphics::Instance().GetMutex());
		RenderState* renderState = graphics.GetRenderState();
		std::vector<ID3D11SamplerState*> samplerStates;
		for (size_t index = 0; index < static_cast<int>(SamplerState::EnumCount); ++index)
		{
			samplerStates.push_back(renderState->GetSamplerState(static_cast<SamplerState>(index)));
		}
		dc->DSSetSamplers(0, static_cast<UINT>(samplerStates.size()), samplerStates.data());
		dc->GSSetSamplers(0, static_cast<UINT>(samplerStates.size()), samplerStates.data());
		dc->PSSetSamplers(0, static_cast<UINT>(samplerStates.size()), samplerStates.data());
	}
	_primitive = std::make_unique<Primitive>(device);
	_inputUI.Initialize();
	// レンダラー作成
	{
		std::lock_guard<std::mutex> lock(Graphics::Instance().GetMutex());
		_meshRenderer.Initialize(device);
		_textureRenderer.Initialize(device);
		_textRenderer.Initialize(device, dc);
		_terrainRenderer.Initialize(device);
		_particleRenderer.Initialize(device, dc);
		_primitiveRenderer.Initialize(device);
		_decalRenderer.Initialize(device, static_cast<UINT>(graphics.GetScreenWidth()), static_cast<UINT>(graphics.GetScreenHeight()));
	}

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
					CEREAL_NVP(_defaultSceneLevel),
					CEREAL_NVP(_defaultSceneName)
				);

				if (_sceneDatas[_defaultSceneLevel][_defaultSceneName.c_str()].get() == nullptr)
					throw nullptr;

				ChangeScene(static_cast<SceneMenuLevel>(_defaultSceneLevel), _defaultSceneName.c_str());
			}
			catch (...)
			{
				ChangeScene(SceneMenuLevel::Debug, u8"Debug");
			}
		}
		else
		{
			ChangeScene(SceneMenuLevel::Debug, u8"Debug");
		}
	}
	else
	{
		ChangeScene(SceneMenuLevel::Debug, u8"Debug");
	}
#else
	ChangeScene(SceneMenuLevel::Game, u8"Title");
#endif
}

// 終了化
void SceneManager::Finalize()
{
	// エフェクトマネージャーの終了の後にエフェクトの終了処理を行うとエラーになるので防ぐ
	Clear();

	// シーンをクリア
	for (int i = 0; i < static_cast<int>(SceneMenuLevel::LevelEnd); ++i)
	{
		_sceneDatas[i].clear();
	}
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

/// 一定間隔の更新処理
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
			for (int i = 0; i < static_cast<int>(SceneMenuLevel::LevelEnd); ++i)
			{
				for (auto& [str, scene] : _sceneDatas[i])
				{
					if (ImGui::MenuItem(str.c_str()))
					{
						SceneManager::Instance().ChangeScene(std::make_shared<SceneLoading>(scene->GetNewShared()));
					}
				}
				ImGui::Separator();
			}

			ImGui::Separator();
			// 現在のシーンの名前
			ImGui::Text(u8"現在のシーン : %s", _currentScene->GetName());
			ImGui::Text(u8"起動時のシーン : %s", _defaultSceneName.c_str());
			ImGui::Separator();
			// 起動時のシーンをデフォルトに設定
			if (ImGui::Button(u8"現在のシーンを起動時のシーンに設定"))
			{
				_defaultSceneLevel = static_cast<int>(_currentScene->GetLevel());
				_defaultSceneName = _currentScene->GetName();
				std::ofstream ostream(SceneDefaultNamePath, std::ios::binary);
				if (ostream.is_open())
				{
					cereal::BinaryOutputArchive archive(ostream);

					try
					{
						archive(
							CEREAL_NVP(_defaultSceneLevel),
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
void SceneManager::ChangeScene(SceneMenuLevel level, std::string sceneName)
{
	auto& scene = _sceneDatas[static_cast<int>(level)][sceneName];
	assert(scene.get());
	SceneManager::Instance().ChangeScene(std::make_shared<SceneLoading>(scene->GetNewShared()));
}

// シーン切り替え
void SceneManager::ChangeScene(std::shared_ptr<Scene> scene)
{
	// 新しいシーンを設定
	_nextScene = scene;
}