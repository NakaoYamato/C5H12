#pragma once

#include "Component.h"
#include "../../Library/Particle/ParticleEmiter.h"
#include "../../Library/Renderer/ParticleRenderer.h"

class ParticleController : public Component
{
public:
	ParticleController(std::string filename);
	~ParticleController() override {}
	// 名前取得
	const char* GetName() const override { return "ParticleController"; }
	// 更新処理
	void Update(float elapsedTime) override;
	// GUI描画
	void DrawGui() override;

	// パーティクルを再生
	void Play();
	// パーティクルを停止
	void Stop();
	// パーティクルが再生中か
	bool IsPlaying() const { return _particleEmiter->IsPlaying(); }
	// エディターGUI表示フラグを取得
	void SetShowEditorGui(bool show) { _showEditorGui = show; }
private:
	// パーティクルエミッター
	std::unique_ptr<ParticleEmiter> _particleEmiter;
	// 編集GUI表示フラグ
	bool _showEditorGui = false;
};