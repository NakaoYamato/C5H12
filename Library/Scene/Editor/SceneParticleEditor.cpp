#include "SceneParticleEditor.h"

#include <imgui.h>

void SceneParticleEditor::Initialize()
{
    Scene::Initialize();
}

void SceneParticleEditor::Update(float elapsedTime)
{
    Scene::Update(elapsedTime);

	_particleEmiter.Update(elapsedTime, GetParticleRenderer());

	//	スパーク
	if (::GetAsyncKeyState('C') & 0x8000)
	{
		DirectX::XMFLOAT3 pos = DirectX::XMFLOAT3((rand() % 30 - 15) * 0.1f, rand() % 30 * 0.1f + 1, (rand() % 30 - 15) * 0.1f + 3);
		auto textureData = GetParticleRenderer().GetTextureData("Test");
		int max = 100;
		for (int i = 0; i < max; i++)
		{
			DirectX::XMFLOAT3 p;
			p.x = pos.x;
			p.y = pos.y;
			p.z = pos.z;

			DirectX::XMFLOAT3 v = { 0,0,0 };
			v.x = (rand() % 10001 - 5000) * 0.0001f;
			v.y = (rand() % 10001) * 0.0002f + 1.2f;
			v.z = (rand() % 10001 - 5000) * 0.0001f;

			DirectX::XMFLOAT3 f = { 0,-1.2f,0 };
			DirectX::XMFLOAT2 s = { 0.05f,0.05f };

			ParticleEmitData data{};
			// 更新タイプ
			data.renderType = ParticleRenderType::Billboard;
			data.texcoordIndex = 3;
			data.timer = 3.0f;
			// 発生位置
			data.position.x = p.x;
			data.position.y = p.y;
			data.position.z = p.z;
			// 発生方向
			data.velocity.x = v.x;
			data.velocity.y = v.y;
			data.velocity.z = v.z;
			// 加速力
			data.acceleration.x = f.x;
			data.acceleration.y = f.y;
			data.acceleration.z = f.z;
			// 大きさ
			data.scale.x = s.x;
			data.scale.y = s.y;
			data.scale.z = 0.0f;

			data.texPosition = textureData.texPosition;
			data.texSize = textureData.texSize;
			data.texSplit = textureData.texSplit;
			GetParticleRenderer().Emit(data);
		}
	}
}

void SceneParticleEditor::DrawGui()
{
	Scene::DrawGui();

	if (ImGui::Begin(u8"パーティクル"))
	{
		if (ImGui::Button(u8"再生"))
		{
			_particleEmiter.Play(GetParticleRenderer());
		}
		_particleEmiter.DrawGui(GetParticleRenderer());
	}
	ImGui::End();
}
