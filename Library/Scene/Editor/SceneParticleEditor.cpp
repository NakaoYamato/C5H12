#include "SceneParticleEditor.h"

#include "../../Library/Component/ParticleController.h"

#include <imgui.h>

// 初期化
void SceneParticleEditor::OnInitialize()
{
	// パーティクル用アクター生成
	auto particleActor = RegisterActor<Actor>(u8"ParticleEmiter", ActorTag::DrawContextParameter);
	auto particleController = particleActor->AddComponent<ParticleController>(u8"./Data/Particle/ParticleEmiter.json");

	particleController->SetShowEditorGui(true);

	// グリッド表示
	SetShowGrid(true);
}
