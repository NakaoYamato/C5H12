#include "ActorFactory.h"

#include "../../Library/Scene/Scene.h"
#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Component/EnvironmentController.h"
#include "../../Library/Component/InstancingModelRenderer.h"

#include <Mygui.h>

std::unordered_map<std::string, ActorFactory::CreatorInfo> ActorFactory::_creators;

// ファイル読み込み
bool ActorFactory::LoadFromFile()
{
	std::string filePath = GetFilePath();
	nlohmann::json jsonData;
	if (Exporter::LoadJsonFile(filePath, &jsonData))
	{
		// 登録されたモデルファイルパス一覧読み込み
		size_t size = jsonData["RegisteredModelFilepaths"].size();
		_registeredModelFilepaths.clear();
		for (size_t i = 0; i < size; ++i)
		{
			std::string typeName = "Env" + std::to_string(_registeredModelFilepaths.size());
			std::string filepath = jsonData["RegisteredModelFilepaths"][i].get<std::string>();
			_registeredModelFilepaths.push_back(filepath);
			Register(typeName,
				[]()->std::shared_ptr<Actor>
				{
					return std::make_shared<Actor>();
				},
				[&](std::shared_ptr<Actor> actor)
				{
					// モデル情報があるか確認
					auto it = _registeredModels.find(filepath);
					if (it == _registeredModels.end())
					{
						ID3D11Device* device = Graphics::Instance().GetDevice();
						_registeredModels[filepath] = std::make_shared<Model>(device, filepath.c_str());
					}

					auto renderer = actor->AddComponent<InstancingModelRenderer>(_registeredModels[filepath]);
					auto controller = actor->AddComponent<EnvironmentController>();
				}
			);
		}
        return true;
	}
	return false;
}

// ファイル保存
bool ActorFactory::SaveToFile()
{
	std::string filePath = GetFilePath();
	nlohmann::json jsonData;
	// 登録されたモデルファイルパス一覧保存
    jsonData["RegisteredModelFilepaths"] = nlohmann::json::array();
	for (size_t i = 0; i < _registeredModelFilepaths.size(); ++i)
	{
		jsonData["RegisteredModelFilepaths"][i] = _registeredModelFilepaths[i];
    }

    return Exporter::SaveJsonFile(filePath, jsonData);
}

// Gui描画
void ActorFactory::DrawGui()
{
	for (const auto& [typeName, func] : _creators)
	{
		ImGui::BulletText("%s", typeName.c_str());
	}
	ImGui::Separator();
	std::string filepath{};
	if (ImGui::OpenDialogBotton(u8"オブジェクト追加", &filepath, ImGui::ModelFilter))
	{
        std::string typeName = "Env" + std::to_string(_registeredModelFilepaths.size());
        _registeredModelFilepaths.push_back(filepath);
		Register(typeName,
			[]()->std::shared_ptr<Actor>
			{
				return std::make_shared<Actor>();
			},
			[&](std::shared_ptr<Actor> actor)
			{
				// モデル情報があるか確認
                auto it = _registeredModels.find(filepath);
				if (it == _registeredModels.end())
				{
					ID3D11Device* device = Graphics::Instance().GetDevice();
					_registeredModels[filepath] = std::make_shared<Model>(device, filepath.c_str());
				}

				auto renderer = actor->AddComponent<InstancingModelRenderer>(_registeredModels[filepath]);
				auto controller = actor->AddComponent<EnvironmentController>();
			}
		);
	}
	ImGui::Separator();
}

// アクター生成
std::shared_ptr<Actor> ActorFactory::CreateActor(Scene* scene,
	const std::string& actorType,
	const std::string& name,
	ActorTag tag)
{
    auto it = _creators.find(actorType);
    if (it != _creators.end())
    {
		auto actor = it->second.createFunc(); // クリエーター関数を呼び出してインスタンスを生成
		actor->SetName(name.c_str());
		actor->SetScene(scene);
		actor->SetFolderPath(actorType);
		scene->GetActorManager().Register(actor, tag);
		if (it->second.initFunc)
		{
			it->second.initFunc(actor);
        }
		actor->Create(tag);
		return actor;
    }
    return nullptr;
}

// 登録されているアクタータイプの取得
std::vector<std::string> ActorFactory::GetRegisteredActorTypes() const
{
	std::vector<std::string> types;
	for (const auto& [typeName, func] : _creators)
	{
		types.push_back(typeName);
	}
	return types;
}
