#pragma once

#include "../../Library/Scene/SceneManager.h"

#include "../../Library/Network/ServerAssignment.h"

class SceneNetworkDebug : public Scene
{
public:
	SceneNetworkDebug() {}
	~SceneNetworkDebug()override {}

	// –¼‘Oæ“¾
	const char* GetName()const { return u8"NetworkDebug"; }
	// ŠK‘wæ“¾
	SceneMenuLevel GetLevel() const { return SceneMenuLevel::Debug; }

	//‰Šú‰»
	void Initialize()override;

	// I—¹‰»
	void Finalize() override;

	// Gui•`‰æˆ—
	void DrawGui() override;

	// ©g‚ğV‹K‚Ìƒ|ƒCƒ“ƒ^‚Å“n‚·
	std::shared_ptr<Scene> GetNewShared() override
	{
		return std::make_shared<SceneNetworkDebug>();
	}

private:
	std::shared_ptr<ServerAssignment> _server;
};

// ƒƒjƒ…[ƒo[‚É“o˜^
_REGISTER_SCENE(SceneNetworkDebug)