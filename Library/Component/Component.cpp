#include "Component.h"

#include "../../Library/Scene/Scene.h"

// ディレクトリ取得
std::string Component::GetDirectory() const
{
	auto actor = _actor.lock();

	std::string directory = "./Data/Resource/Component/";
	directory += actor->GetName();
	directory += "/";
	return directory;
}
