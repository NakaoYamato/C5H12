#include "ParticleController.h"

#include "../../Library/Scene/Scene.h"

#include <imgui.h>

ParticleController::ParticleController(std::string filename)
{
	// パーティクルエミッターの初期化
	_particleEmiter = std::make_unique<ParticleEmiter>();
	_particleEmiter->Inport(filename);
}

void ParticleController::Update(float elapsedTime)
{
	_particleEmiter->Update(elapsedTime, GetActor()->GetScene()->GetParticleRenderer(), GetActor()->GetTransform().GetMatrix());

	if (_showEditorGui)
	{
#ifdef USE_IMGUI
		if (ImGui::Begin(u8"パーティクルエミッター"))
		{
			// パーティクルエミッターのGUI描画
			_particleEmiter->DrawGui(GetActor()->GetScene()->GetParticleRenderer());
		}
		ImGui::End();
#endif // USE_IMGUI
	}
}

void ParticleController::DrawGui()
{
	if (ImGui::Button(u8"再生"))
		Play();
	ImGui::SameLine();
	if (ImGui::Button(u8"停止"))
		Stop();
	ImGui::Checkbox(u8"エディターGUI表示", &_showEditorGui);
}

void ParticleController::Play()
{
	if (_particleEmiter)
	{
		_particleEmiter->Play(GetActor()->GetScene()->GetParticleRenderer(), GetActor()->GetTransform().GetMatrix());
	}
}

void ParticleController::Stop()
{
	if (_particleEmiter)
	{
		_particleEmiter->Stop();
	}
}
