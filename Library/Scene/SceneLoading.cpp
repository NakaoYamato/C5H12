#include "SceneLoading.h"
#include "../Graphics/Graphics.h"

void SceneLoading::Initialize()
{
	//スレッド開始
	_thread = new std::thread(LoadingThread, this);
}

void SceneLoading::Finalize()
{
	if (_thread != nullptr && _thread->joinable()) {
		_thread->join();
		delete _thread;
		_thread = nullptr;
	}
}

void SceneLoading::Update(float elapsedTime)
{
	//点滅速度(大きいほど早い)
	float blink_speed = 0.3f;
	
	//次のシーンの準備が完了したらシーンを切り替える
	if (_nextScene->IsReady())
	{
		SceneManager::Instance().ChangeScene(_nextScene);
	}
}

void SceneLoading::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

	FLOAT color[] = { 0.0f,0.0f,0.0f,1.0f };//RGBA(0.0~1.0)
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);
}

void SceneLoading::LoadingThread(SceneLoading* scene)
{
	//COM関連の初期化でスレッド毎に呼ぶ必要がある
	// 戻り値を使わないので無視
	(void)CoInitialize(nullptr);

	//次のシーンの初期化を行う
	scene->_nextScene->Initialize();

	CoUninitialize();

	scene->_nextScene->SetReady();
}
