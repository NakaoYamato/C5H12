#include "AudioSystem.h"

#include <imgui.h>

#include "../HrTrace.h"

AudioSystem::AudioSystem()
{
	masterVoice_ = std::make_unique<MasterVoice>();

	audioGroups_.insert(std::make_pair("Music", std::make_unique<SubmixVoice>(masterVoice_->GetXAudio2())));
	audioGroups_.insert(std::make_pair("Sound", std::make_unique<SubmixVoice>(masterVoice_->GetXAudio2())));
}

AudioSystem::~AudioSystem()
{
}

// 終了処理
void AudioSystem::Finalize()
{
	for (const auto& voice : activeVoices_)
		voice->Stop();

	// ソースボイスのクリア
	activeVoices_.clear();

	// グループのクリア
	audioGroups_.clear();

	// リソースのクリア
	resources_.clear();

	// マスターボイスの初期化
	masterVoice_ = nullptr;
}

// 更新処理
void AudioSystem::Update()
{
	auto it = activeVoices_.begin();
	while (it != activeVoices_.end())
	{
		// 再生されていないか参照数が2より少なかったら削除
		if (!it->get()->IsQueued())
		{
			it->get()->Stop();
			it = activeVoices_.erase(it);
		}

		else
			it++;
	}
}

// リソースの読み込み
AudioResource* AudioSystem::LoadResource(const char* filePath)
{
	auto it = resources_.find(filePath);
	if (it != resources_.end())
		return it->second.get();

	resources_.insert(
		std::make_pair(filePath,
			std::make_unique<AudioResource>(masterVoice_->GetXAudio2(), filePath)
		)
	);

	return resources_[filePath].get();
}

// 再生
SourceVoiceRef AudioSystem::Play(const char* filePath, const char* groupName, float volume)
{
	// ソースボイス作成
	SourceVoiceRef sourceVoice = std::make_shared<SourceVoice>(
		masterVoice_->GetXAudio2(),
		resources_.at(filePath).get(),
		audioGroups_.at(groupName).get()
	);

	// リストに追加
	activeVoices_.emplace_back(sourceVoice);

	// 再生
	sourceVoice->Play(volume);

	// ポインタを返す
	return sourceVoice;
}

// デバッグGui描画
void AudioSystem::DrawDebugGui(bool& isOpen)
{
	ImGui::Begin("AudioSystem", &isOpen);

	if (ImGui::TreeNodeEx("Master", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// 音量の設定
		float vol = masterVoice_->GetVolume();
		ImGui::SliderFloat("Volume", &vol, 0.0f, 1.0f);
		masterVoice_->SetVolume(vol);

		ImGui::TreePop();
	}

	if (ImGui::TreeNodeEx("Group", ImGuiTreeNodeFlags_DefaultOpen))
	{
		for (const auto& [name, group] : audioGroups_)
		{
			// グループごとにツリー分け
			if(ImGui::TreeNodeEx(name.c_str()))
			{
				// 音量の設定
				float vol = group->GetVolume();
				ImGui::SliderFloat("Volume", &vol, 0.0f, 1.0f);
				group->SetVolume(vol);

				// 停止状態からの再開ボタン
				if (ImGui::Button(u8"一括再開", { 60.0f, 30.0f }))
				{
					for (auto& voice : activeVoices_)
					{
						if (voice->IsSameGroup(group.get()))
							voice->Resume();
					}
				}

				ImGui::SameLine();

				// 停止ボタン
				if (ImGui::Button(u8"一括停止", { 60.0f, 30.0f }))
				{
					for (auto& voice : activeVoices_)
					{
						if (voice->IsSameGroup(group.get()))
							voice->Stop(false);
					}
				}

				ImGui::TreePop();
			}
		}

		ImGui::TreePop();
	}

	ImGui::Text(u8"アクティブな音声の数 : %d", static_cast<int>(activeVoices_.size()));

	ImGui::End();
}
