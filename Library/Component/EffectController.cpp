#include "EffectController.h"

#include "../../Library/Scene/Scene.h"
#include "../../Library/Graphics/Graphics.h"
#include "../../Library/DebugSupporter/DebugSupporter.h"

#include <imgui.h>
#include <Mygui.h>

#pragma region 各エフェクトデータ
void EffectController::EffectData::DrawGui()
{
	ImGui::DragFloat3("position", &position.x, 0.1f);
	ImGui::DragFloat3("scale", &scale.x, 0.1f);
	Vector3 degree = Vector3::ToDegrees(rotation);
	ImGui::DragFloat3("angle", &degree.x);
	rotation = Vector3::ToRadians(degree);
}

#pragma region Effekseerエフェクトデータ
void EffectController::EffekseerEffectData::Load(std::string filepath)
{
	this->filepath = filepath;
    //エフェクトを読み込みする前にロックする
    //※マルチスレッドでEffectを作成するとDeviceContextを同時アクセスして
    //　フリーズする可能性があるので排他制御する
    std::lock_guard<std::mutex> lock(Graphics::Instance().GetMutex());

    //Effekseerのリソースを読み込む
    //EffekseerはUTF-16のファイルパス以外は対応していないため文字コード変換が必要
    char16_t utf16Filename[256];
    Effekseer::ConvertUtf8ToUtf16(utf16Filename, 256, filepath.c_str());

    //Effekseer::Managerを取得
    Effekseer::ManagerRef effekseerManager = owner->GetScene()->GetEffekseerEffectManager().GetEffekseerManager();

    //Effekseerエフェクトを読み込み
    effekseerEffect = Effekseer::Effect::Create(effekseerManager, (EFK_CHAR*)utf16Filename);
}

void EffectController::EffekseerEffectData::Play()
{
    Effekseer::ManagerRef effekseerManager = owner->GetScene()->GetEffekseerEffectManager().GetEffekseerManager();

	handle = effekseerManager->Play(effekseerEffect, position.x, position.y, position.z);

	effekseerManager->SetRotation(handle, rotation.x, rotation.y, rotation.z);
    effekseerManager->SetScale(handle, scale.x, scale.y, scale.z);
}

void EffectController::EffekseerEffectData::Stop()
{
    Effekseer::ManagerRef effekseerManager = owner->GetScene()->GetEffekseerEffectManager().GetEffekseerManager();
    effekseerManager->StopEffect(handle);
	handle = -1;
}

void EffectController::EffekseerEffectData::SetPosition(const Vector3& position)
{
	EffectData::SetPosition(position);
	if (handle != -1)
	{
		Effekseer::ManagerRef effekseerManager = owner->GetScene()->GetEffekseerEffectManager().GetEffekseerManager();
		effekseerManager->SetLocation(handle, position.x, position.y, position.z);
	}
}

void EffectController::EffekseerEffectData::SetRotation(const Vector3& rotation)
{
	EffectData::SetRotation(rotation);
	if (handle != -1)
	{
		Effekseer::ManagerRef effekseerManager = owner->GetScene()->GetEffekseerEffectManager().GetEffekseerManager();
		effekseerManager->SetRotation(handle, rotation.x, rotation.y, rotation.z);
	}
}

void EffectController::EffekseerEffectData::SetScale(const Vector3& scale)
{
	EffectData::SetScale(scale);
	if (handle != -1)
	{
		Effekseer::ManagerRef effekseerManager = owner->GetScene()->GetEffekseerEffectManager().GetEffekseerManager();
		effekseerManager->SetScale(handle, scale.x, scale.y, scale.z);
	}
}
#pragma endregion

#pragma region パーティクルエフェクトデータ
void EffectController::ParticleEffectData::Load(std::string filepath)
{
	this->filepath = filepath;
	// パーティクルエミッターの初期化
	particleEmiter = std::make_unique<ParticleEmiter>();
	particleEmiter->Inport(filepath);
}

void EffectController::ParticleEffectData::Play()
{
	if (particleEmiter)
	{
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&position));
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
		DirectX::XMMATRIX S = DirectX::XMMatrixScalingFromVector(DirectX::XMLoadFloat3(&scale));

		DirectX::XMStoreFloat4x4(&transform,
			S *
			R *
			T
		);
		particleEmiter->Play(owner->GetScene()->GetParticleRenderer(), transform);
	}
}

void EffectController::ParticleEffectData::Stop()
{
	if (particleEmiter)
	{
		particleEmiter->Stop();
	}
}

void EffectController::ParticleEffectData::Update(float elapsedTime)
{
	if (particleEmiter && particleEmiter->IsPlaying())
	{
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&position));
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
		DirectX::XMMATRIX S = DirectX::XMMatrixScalingFromVector(DirectX::XMLoadFloat3(&scale));

		DirectX::XMStoreFloat4x4(&transform,
			S *
			R *
			T
		);
		particleEmiter->Update(elapsedTime, owner->GetScene()->GetParticleRenderer(), transform);
	}

	if (showEditorGui)
	{
#ifdef USE_IMGUI
		if (ImGui::Begin(u8"パーティクルエミッター"))
		{
			// パーティクルエミッターのGUI描画
			particleEmiter->DrawGui(owner->GetScene()->GetParticleRenderer());
		}
		ImGui::End();
#endif // USE_IMGUI
	}
}

void EffectController::ParticleEffectData::DrawGui()
{
	EffectData::DrawGui();
	ImGui::Checkbox(u8"エディターGUI表示", &showEditorGui);
}
#pragma endregion

#pragma endregion

// 更新処理
void EffectController::Update(float elapsedTime)
{
	for (auto& [id, effect] : _effectMap)
	{
		effect->Update(elapsedTime);
	}
}

// GUI描画
void EffectController::DrawGui()
{
	for (auto& [id, effect] : _effectMap)
	{
		if (ImGui::TreeNode((std::to_string(id)).c_str()))
		{
			ImGui::Text(u8"ファイルパス: %s", effect->GetFilePath().c_str());
			if (ImGui::Button(u8"再生"))
			{
				effect->SetPosition(GetActor()->GetTransform().GetWorldPosition());
				effect->Play();
			}
			ImGui::SameLine();
			if (ImGui::Button(u8"停止"))
				effect->Stop();
			effect->DrawGui();
			ImGui::TreePop();
		}
	}
	ImGui::Separator();
	if (ImGui::Button(u8"エフェクシア読み込み"))
	{
		const char* filter = "Effekseer Files(*.efk)\0*.efk;\0\0";
		std::string filepath;
		std::string currentDirectory;
		Debug::Dialog::DialogResult result = Debug::Dialog::OpenFileName(filepath, currentDirectory, filter);
		if (result == Debug::Dialog::DialogResult::Yes || result == Debug::Dialog::DialogResult::OK)
		{
			// 相対パス取得
			std::filesystem::path relativePath = std::filesystem::relative(filepath, currentDirectory);
			UINT id = static_cast<UINT>(_effectMap.size());
			LoadEffekseerEffect(id, relativePath.string());
		}
	}
	if (ImGui::Button(u8"パーティクル読み込み"))
	{
		const char* filter = "Particle Effect Files(*.json)\0*.json;\0\0";
		std::string filepath;
		std::string currentDirectory;
		Debug::Dialog::DialogResult result = Debug::Dialog::OpenFileName(filepath, currentDirectory, filter);
		if (result == Debug::Dialog::DialogResult::Yes || result == Debug::Dialog::DialogResult::OK)
		{
			// 相対パス取得
			std::filesystem::path relativePath = std::filesystem::relative(filepath, currentDirectory);
			UINT id = static_cast<UINT>(_effectMap.size());
			LoadParticleEffect(id, relativePath.string());
		}
	}
}

// エフェクト再生
void EffectController::Play(UINT id, const Vector3& position, const Vector3& rotation, const Vector3& scale)
{
	if (_effectMap.find(id) != _effectMap.end())
	{
		_effectMap[id]->SetPosition(position);
		_effectMap[id]->SetRotation(rotation);
		_effectMap[id]->SetScale(scale);
		_effectMap[id]->Play();
	}
}

// エフェクト停止
void EffectController::Stop(UINT id)
{
	if (_effectMap.find(id) != _effectMap.end())
	{
		_effectMap[id]->Stop();
	}
}

// エフェクトデータ取得
EffectController::EffectData* EffectController::GetEffectData(UINT id)
{
	return _effectMap[id].get();
}

// Effekseerエフェクト読み込み
void EffectController::LoadEffekseerEffect(UINT id, const std::string& filepath)
{
	if (_effectMap.find(id) == _effectMap.end())
	{
		_effectMap[id] = std::make_unique<EffekseerEffectData>(GetActor().get());
		_effectMap[id]->Load(filepath);
	}
}

// パーティクルエフェクト読み込み
void EffectController::LoadParticleEffect(UINT id, const std::string& filepath)
{
	if (_effectMap.find(id) == _effectMap.end())
	{
		_effectMap[id] = std::make_unique<ParticleEffectData>(GetActor().get());
		_effectMap[id]->Load(filepath);
	}
}
