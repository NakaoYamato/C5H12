#include "ActorFactory.h"

#include "../../Library/Scene/Scene.h"

#include <Mygui.h>

std::unordered_map<std::string, ActorFactory::CreatorFunc> ActorFactory::_creators;

// ファイル読み込み
bool ActorFactory::LoadFromFile()
{
	return false;
}

// ファイル保存
bool ActorFactory::SaveToFile()
{
	return false;
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
		auto actor = it->second(); // クリエーター関数を呼び出してインスタンスを生成
		actor->SetName(name.c_str());
		actor->SetScene(scene);
		actor->SetFolderPath(actorType);
		scene->GetActorManager().Register(actor, tag);
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
