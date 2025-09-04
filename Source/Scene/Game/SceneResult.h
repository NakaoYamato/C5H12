#pragma once

#include "../../Library/Scene/SceneManager.h"

class SceneResult : public Scene
{
public:
	SceneResult() {}
	~SceneResult() override {}
	// –¼‘Oæ“¾
	const char* GetName() const override { return u8"Result"; }
	// ŠK‘wæ“¾
	SceneMenuLevel GetLevel() const override { return SceneMenuLevel::Game; }
	// ‰Šú‰»
	void OnInitialize() override;
	// ©g‚ğV‹K‚Ìƒ|ƒCƒ“ƒ^‚Å“n‚·
	std::shared_ptr<Scene> GetNewShared() override
	{
		return std::make_shared<SceneResult>();
	}
};

// ƒƒjƒ…[ƒo[‚É“o˜^
_REGISTER_SCENE(SceneResult)