#pragma once

#include "AudioResource.h"
#include "MasterVoice.h"
#include "SourceVoice.h"
#include "SubmixVoice.h"

#include <string>
#include <unordered_map>

using SourceVoiceRef = std::shared_ptr<SourceVoice>;

class AudioSystem
{
private:
	AudioSystem();
	~AudioSystem();

public:
	static AudioSystem& Instance()
	{
		static AudioSystem instance;
		return instance;
	}

	// リソースの取得
	using Resources = std::unordered_map<std::string, std::unique_ptr<AudioResource>>;
	const Resources& GetResources() const { return resources_; }

	// グループの取得
	using Groups = std::unordered_map<std::string, std::unique_ptr<SubmixVoice>>;
	const Groups& GetGroups() const { return audioGroups_; }

	// 終了処理
	void Finalize();

	// 更新処理
	void Update();

	// リソースの読み込み
	AudioResource* LoadResource(const char* filePath);

	// 再生
	SourceVoiceRef Play(const char* filePath, const char* groupName, float volume);

	// デバッグGui描画
	void DrawDebugGui(bool& isOpen);

	IXAudio2* GetXAudio2() { return masterVoice_->GetXAudio2(); }
private:
	// すべてのオーディオの大本
	std::unique_ptr<MasterVoice> masterVoice_;

	// オーディオのリソース
	Resources resources_;

	// オーディオのグループ
	Groups audioGroups_;

	// 再生中のソースボイス
	std::list<std::shared_ptr<SourceVoice>> activeVoices_;
};