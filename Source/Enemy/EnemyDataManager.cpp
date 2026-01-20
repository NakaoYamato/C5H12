#include "EnemyDataManager.h"

#include "../../Library/Scene/Scene.h"
#include "../../Library/Graphics/Graphics.h"
#include "../../Library/JobSystem/JobSystem.h"

#include "Wyvern/WyvernActor.h"

#include <Mygui.h>

bool EnemyDataManager::Initialize()
{
	Register<WyvernActor>("Wyvern", WyvernActor::GetModelFilePath());

	return false;
}

bool EnemyDataManager::LoadFromFile()
{
	return false;
}
bool EnemyDataManager::SaveToFile()
{
	return false;
}

// Gui描画
void EnemyDataManager::DrawGui()
{
	if (ImGui::TreeNode(u8"敵データ一覧"))
	{
		for (const auto& enemyData : _enemyDatas)
		{
			ImGui::Text(u8"敵タイプ: %s", enemyData.first.c_str());
			ImGui::Text(u8"モデルパス: %s", enemyData.second.modelFilePath.c_str());
			if (ImGui::Button((u8"モデル読み込み##" + enemyData.first).c_str()))
			{
				LoadModel(enemyData.first);
			}
			ImGui::Separator();
		}
		ImGui::TreePop();
	}

	if (ImGui::TreeNode(u8"敵モデル一覧"))
	{
		for (const auto& enemyModel : _enemyModels)
		{
			ImGui::Text(u8"敵タイプ: %s", enemyModel.first.c_str());
			bool isLoaded = IsModelLoaded(enemyModel.first);
			ImGui::Text(u8"モデル読み込み済み: %s", isLoaded ? u8"はい" : u8"いいえ");
			if (isLoaded)
			{
				if (ImGui::Button((u8"モデル解放##" + enemyModel.first).c_str()))
				{
					ReleaseModel(enemyModel.first);
				}
			}
			else
			{
				if (ImGui::Button((u8"モデル読み込み##" + enemyModel.first).c_str()))
				{
					LoadModel(enemyModel.first);
				}
			}

			ImGui::Separator();
		}
		ImGui::TreePop();
	}
}

// アクター生成
std::shared_ptr<Actor> EnemyDataManager::CreateActor(Scene* scene, const std::string& actorType, const std::string& name)
{
	auto it = _enemyDatas.find(actorType);
	if (it != _enemyDatas.end())
	{
		// 生成関数を呼び出してインスタンスを生成
		auto actor = it->second.makeFunc();
		actor->SetName(name.c_str());
		actor->SetScene(scene);
		actor->SetFolderPath(actorType);
		scene->GetActorManager().Register(actor, ActorTag::Enemy);
		actor->Create(ActorTag::Enemy);
		return actor;
	}
	return nullptr;
}

// モデル読み込み
void EnemyDataManager::LoadModel(const std::string& actorType)
{
	auto loadModel = _enemyModels.find(actorType);
	// ロードされていなければ読み込み
	if (loadModel == _enemyModels.end())
	{
		auto it = _enemyDatas.find(actorType);
		if (it != _enemyDatas.end())
		{
			_enemyModels[actorType].isLoading = false;
			// 別スレッドで処理
			JobSystem::Instance().EnqueueJob(
				"LoadModel",
				ImGuiControl::Profiler::Color::Blue,
				[&](const std::string& key, const std::string& modelFilePath)
				{
					auto model = std::make_shared<Model>(Graphics::Instance().GetDevice(),
						modelFilePath.c_str());

					{
						// 排他制御
						std::lock_guard<std::mutex> loadLock(_modelLoadMutex);
						auto it = _enemyModels.find(actorType);
						if (it != _enemyModels.end())
						{
							std::lock_guard<std::mutex> lock(it->second.mutex);
							it->second.model = model;
							it->second.isLoading = true;
						}
					}
				},
				actorType, it->second.modelFilePath);
		}
	}
}

// モデル解放
void EnemyDataManager::ReleaseModel(const std::string& actorType)
{
	std::lock_guard<std::mutex> loadLock(_modelLoadMutex);
	auto it = _enemyModels.find(actorType);
	if (it != _enemyModels.end())
	{
		_enemyModels.erase(it);
	}
}

// モデルが読み込まれているか
bool EnemyDataManager::IsModelLoaded(const std::string& actorType)
{
	std::lock_guard<std::mutex> loadLock(_modelLoadMutex);
	// モデルが読み込まれているか
	auto it = _enemyModels.find(actorType);
	if (it != _enemyModels.end())
	{
		// 排他制御
		std::lock_guard<std::mutex> lock(it->second.mutex);
		return it->second.isLoading;
	}
	return false;
}
