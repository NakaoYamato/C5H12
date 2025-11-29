#include "Component.h"

#include "../../Library/Scene/Scene.h"

// ディレクトリ取得
std::string Component::GetDirectory() const
{
	auto actor = _actor.lock();

	std::string directory = "./Data/Resource/";
	directory += actor->GetScene()->GetName();
	directory += "/";
	directory += actor->GetName();
	directory += "/";
	return directory;
}
