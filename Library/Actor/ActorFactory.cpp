#include "ActorFactory.h"

#include "../../Library/Scene/Scene.h"
#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Component/EnvironmentController.h"
#include "../../Library/Component/InstancingModelRenderer.h"

#include "../../Source/Stage/SafetyZone.h"
#include "../../Source/Stage/EntryZone.h"

#include <Mygui.h>

std::unordered_map<std::string, ActorFactory::CreatorInfo> ActorFactory::_creators;

// ファイル読み込み
bool ActorFactory::LoadFromFile()
{
	std::string filePath = GetFilePath();
	nlohmann::json jsonData;
	if (Exporter::LoadJsonFile(filePath, &jsonData))
	{
        return true;
	}
	return false;
}

// ファイル保存
bool ActorFactory::SaveToFile()
{
	std::string filePath = GetFilePath();
	nlohmann::json jsonData;

    return Exporter::SaveJsonFile(filePath, jsonData);
}

// 初期化処理
bool ActorFactory::Initialize()
{
#pragma region アクター登録
	// 開始、初期位置ゾーン
	Register("EntryZone",
		[]()->std::shared_ptr<Actor>
		{
			return std::make_shared<Actor>();
		},
		[&](std::shared_ptr<Actor> actor)
		{
			// コンポーネント追加
			auto entryZone = actor->AddComponent<EntryZone>();
		}
	);
	// 安全地帯
	Register("SafetyZone",
		[]()->std::shared_ptr<Actor>
		{
			return std::make_shared<Actor>();
		},
		[&](std::shared_ptr<Actor> actor)
		{
			// コンポーネント追加
			auto safetyZone = actor->AddComponent<SafetyZone>();
			auto box = actor->AddCollider<BoxCollider>();
		}
	);
#pragma endregion


	return true;
}

// Gui描画
void ActorFactory::DrawGui()
{
	for (const auto& [typeName, func] : _creators)
	{
		ImGui::BulletText("%s", typeName.c_str());
	}
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
		// クリエーター関数を呼び出してインスタンスを生成
		auto actor = it->second.makeFunc();
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
