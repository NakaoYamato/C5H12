#pragma once

#include "SceneManager.h"
#include <thread>
#include <memory>

class SceneLoading : public Scene
{
public:
	SceneLoading(std::shared_ptr<Scene>&& nextScene) :
		nextScene_(std::move(nextScene)) {}
	~SceneLoading()override {}

	// 名前取得
	const char* GetName()const { return u8"Loading"; }

	//初期化
	void Initialize()override;

	//終了化 
	void Finalize()override;

	//更新処理
	void Update(float elapsedTime)override;

	//描画処理
	void Render()override;

	// 自身を新規のポインタで渡す
	std::shared_ptr<Scene> GetNewShared() override
	{
		return nullptr;
	}

private:
	static void LoadingThread(SceneLoading* scene);

private:
	std::shared_ptr<Scene> nextScene_;
	std::thread* thread_ = nullptr;
};



