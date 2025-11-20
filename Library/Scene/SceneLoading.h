#pragma once

#include "SceneManager.h"

#include <thread>
#include <memory>

class SceneLoading : public Scene
{
public:
	SceneLoading(std::shared_ptr<Scene>&& nextScene) :
		_nextScene(std::move(nextScene)) {}
	~SceneLoading()override {}

	// 名前取得
	const char* GetName()const { return u8"Loading"; }
	// 階層取得(アクセスすることがないのでEndを返す)
	SceneMenuLevel GetLevel() const { return SceneMenuLevel::LevelEnd; }

	//初期化
	void OnInitialize()override;
	//終了化 
	void OnFinalize()override;
	// 自身を新規のポインタで渡す
	std::shared_ptr<Scene> GetNewShared() override
	{
		return nullptr;
	}

	// 進捗取得
	float GetCompletionLoading() const
	{
		return std::clamp(_nextScene->GetCompletionLoading(), 0.0f, 1.0f);
	}

    // シーン切り替え
	void ChangeNextScene();
    // 準備完了しているか
	bool IsNextSceneReady() const
	{
		return _nextScene->IsReady();
    }
private:
    // ローディングスレッド
	static void LoadingThread(SceneLoading* scene);

private:
	std::shared_ptr<Scene> _nextScene;
	std::thread* _thread = nullptr;
};



