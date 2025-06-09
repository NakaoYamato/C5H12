#pragma once

#include "../SceneManager.h"
#include "../../Library/Particle/ParticleEmiter.h"

class SceneParticleEditor : public Scene
{
public:
	SceneParticleEditor() {}
	~SceneParticleEditor() override {}
	// 名前取得
	const char* GetName() const { return u8"ParticleEditor"; }
	// 階層取得
	SceneMenuLevel GetLevel() const { return SceneMenuLevel::Editor; }
	// 初期化
	void Initialize() override;
	// 自身を新規のポインタで渡す
	std::shared_ptr<Scene> GetNewShared() override
	{
		return std::make_shared<SceneParticleEditor>();
	}

private:
	ParticleEmiter _particleEmiter; // パーティクルエミッター
};

// メニューバーに登録
_REGISTER_SCENE(SceneParticleEditor)