#include "SceneParticleEditor.h"

#include "../../Library/Component/EffectController.h"

#include <imgui.h>

// 初期化
void SceneParticleEditor::OnInitialize()
{
	// パーティクル用アクター生成
	auto particleActor = RegisterActor<Actor>(u8"ParticleEmiter", ActorTag::System);
	auto effectController = particleActor->AddComponent<EffectController>();
	effectController->LoadParticleEffect(0, u8"./Data/Particle/ParticleEmiter.json");
	((EffectController::ParticleEffectData*)effectController->GetEffectData(0))->SetShowEditorGui(true);

	// グリッド表示
	SetShowGrid(true);
}
