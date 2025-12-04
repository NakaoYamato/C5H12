#include "SceneLoading.h"

#include "../Graphics/Graphics.h"

#include "../../Source/Loading/LoadingSprController.h"

#include <imgui.h>

//初期化
void SceneLoading::OnInitialize()
{
	//スレッド開始
	_thread = new std::thread(LoadingThread, this);

	auto back = RegisterActor<UIActor>(u8"LoadingBack", ActorTag::UI);
	auto backController = back->AddComponent<LoadingSprController>();
}
//終了化 
void SceneLoading::OnFinalize()
{
	if (_thread != nullptr && _thread->joinable()) {
		_thread->join();
		delete _thread;
		_thread = nullptr;
	}
}
// シーン切り替え
void SceneLoading::ChangeNextScene()
{
	if (_nextScene->IsReady())
	{
		SceneManager::Instance().ChangeScene(_nextScene);
	}
	_nextScene.reset();
}
// ローディングスレッド
void SceneLoading::LoadingThread(SceneLoading* scene)
{
	//COM関連の初期化でスレッド毎に呼ぶ必要がある
	// 戻り値を使わないので無視
	(void)CoInitialize(nullptr);

	//次のシーンの初期化を行う
	scene->_nextScene->Initialize();

	CoUninitialize();
}
