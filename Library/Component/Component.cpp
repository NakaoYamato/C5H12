#include "Component.h"

#include "../../Library/Scene/Scene.h"

// ディレクトリ取得
std::string Component::GetDirectory() const
{
	auto actor = _actor.lock();

	std::string directory = "./Data/Resource/Component/";
	if (actor->GetFolderPath().empty())
		directory += actor->GetName();
	else
		directory += actor->GetFolderPath();
	directory += "/";
	return directory;
}
