#include "Component.h"

#include "../../Library/Scene/Scene.h"

// ディレクトリ取得
std::string Component::GetDirectory() const
{
	std::string directory = "./Data/Resource/Component/";
	if (_actor->GetFolderPath().empty())
		directory += _actor->GetName();
	else
		directory += _actor->GetFolderPath();
	directory += "/";
	return directory;
}
