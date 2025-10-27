#include "SceneLoading.h"

#include "../Graphics/Graphics.h"
#include "../../Library/Input/Input.h"

#include <imgui.h>

//初期化
void SceneLoading::Initialize()
{
	//スレッド開始
	_thread = new std::thread(LoadingThread, this);

	// レンダラー作成
	{
		std::lock_guard<std::mutex> lock(Graphics::Instance().GetMutex());
		GetTextRenderer().Initialize(Graphics::Instance().GetDevice(),
			Graphics::Instance().GetDeviceContext());
	}

	// ロードバーの読み込み
	_sprites["LoadingBar"].LoadTexture(L"./Data/Texture/Loading/LoadingBar.png", Sprite::CenterAlignment::LeftCenter);
	_sprites["LoadingBar"].SetPosition(Vector2(870.0f, 40.0f));
}
//終了化 
void SceneLoading::Finalize()
{
	if (_thread != nullptr && _thread->joinable()) {
		_thread->join();
		delete _thread;
		_thread = nullptr;
	}
}
//更新処理
void SceneLoading::Update(float elapsedTime)
{
	if (_nextScene->IsReady())
	{
		GetTextRenderer().Draw(
			FontType::MSGothic,
			"Spaceで開始",
			_completeTextPosition,
			_completeTextColor,
			0.0f,
			Vector2::Zero,
			_completeTextScale
		);
		GetTextRenderer().Draw(
			FontType::MSGothic,
			std::to_string(_loadingTimer).c_str(),
			_completeTextPosition + Vector2(0.0f, 100.0f),
			_completeTextColor,
			0.0f,
			Vector2::Zero,
			_completeTextScale
		);

		// 入力によってシーンを切り替える
		if (_INPUT_TRIGGERD("Evade"))
		{
			SceneManager::Instance().ChangeScene(_nextScene);
		}
	}
	else
	{
		_loadingTimer += elapsedTime;
	}

	// ロードバーの進捗更新
	_loadingBarWidth = EasingLerp( 
		_loadingBarWidth,
		std::clamp(_nextScene->GetCompletionLoading(), 0.0f, 1.0f),
		_loadingBarSpeed * elapsedTime);
	_sprites["LoadingBar"].SetScale(Vector2(_loadingBarWidth, 1.0f));
}
//描画処理
void SceneLoading::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();
	RenderState* renderState = graphics.GetRenderState();
	ConstantBufferManager* cbManager = graphics.GetConstantBufferManager();
	float screenWidth = graphics.GetScreenWidth();
	float screenHeight = graphics.GetScreenHeight();

	FLOAT color[] = { 0.0f,0.0f,0.0f,1.0f };//RGBA(0.0~1.0)
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);

	// レンダーコンテキスト作成
	RenderContext& rc = GetRenderContext();
	rc.deviceContext = dc;
	rc.renderState = graphics.GetRenderState();
	rc.camera = GetMainCamera();
	rc.lightDirection = Vector4::Right;
	rc.lightColor = Vector4::White;
	rc.lightAmbientColor = Vector4::Black;

	// サンプラーステート設定
	{
		ID3D11SamplerState* samplerStates[] =
		{
			renderState->GetSamplerState(SamplerState::PointWrap),
			renderState->GetSamplerState(SamplerState::PointClamp),
			renderState->GetSamplerState(SamplerState::LinearWrap),
			renderState->GetSamplerState(SamplerState::LinearClamp)
		};
		dc->PSSetSamplers(0, _countof(samplerStates), samplerStates);
	}
	// レンダーステート設定
	dc->OMSetBlendState(renderState->GetBlendState(BlendState::Alpha), nullptr, 0xFFFFFFFF);
	dc->OMSetDepthStencilState(renderState->GetDepthStencilState(DepthState::TestAndWrite), 1);
	dc->RSSetState(renderState->GetRasterizerState(RasterizerState::SolidCullNone));

	// スプライト描画
	{
		// フレーム部分の描画
		rc.deviceContext->OMSetDepthStencilState(renderState->GetDepthStencilState(DepthState::TestAndWrite), 1);
		_sprites["LoadingBar"].Render(rc, Vector2::Zero, Vector2::One);

		// マスク部分の描画
		rc.deviceContext->OMSetDepthStencilState(renderState->GetDepthStencilState(DepthState::SpriteMask), 1);
		//SpriteRender(MaskSprite, rc, offset, offsetScale);

		// ゲージ部分の描画
		rc.deviceContext->OMSetDepthStencilState(renderState->GetDepthStencilState(DepthState::SpriteApplyMask), 0);
		//SpriteRender(DamageGaugeSprite, rc, offset, offsetScale);
		//SpriteRender(GaugeSprite, rc, offset, offsetScale);
	
		rc.deviceContext->OMSetDepthStencilState(renderState->GetDepthStencilState(DepthState::TestAndWrite), 1);
	}

	// テキスト描画
	dc->OMSetBlendState(renderState->GetBlendState(BlendState::Alpha), nullptr, 0xFFFFFFFF);
	dc->OMSetDepthStencilState(renderState->GetDepthStencilState(DepthState::TestAndWrite), 1);
	GetTextRenderer().Render(rc.camera->GetView(), rc.camera->GetProjection(), screenWidth, screenHeight);
}
// GUI描画処理
void SceneLoading::DrawGui()
{
	if (ImGui::Begin("Loading"))
	{
		ImGui::DragFloat(u8"ロードバー横幅", &_loadingBarWidth, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat(u8"ロードバー速度", &_loadingBarSpeed, 0.01f, 0.0f, 10.0f);
		ImGui::Separator();

		ImGui::DragFloat2(u8"完了テキスト位置", &_completeTextPosition.x, 1.0f, 0.0f);
		ImGui::DragFloat2(u8"完了テキストサイズ", &_completeTextScale.x, 0.01f, 0.0f);
		ImGui::ColorEdit4(u8"完了テキスト色", &_completeTextColor.x);
		ImGui::Separator();

		for (auto& [name, sprite] : _sprites)
		{
			if (ImGui::TreeNode(name.c_str()))
			{
				sprite.DrawGui();
				ImGui::TreePop();
			}
		}
	}
	ImGui::End();
}

void SceneLoading::LoadingThread(SceneLoading* scene)
{
	//COM関連の初期化でスレッド毎に呼ぶ必要がある
	// 戻り値を使わないので無視
	(void)CoInitialize(nullptr);

	//次のシーンの初期化を行う
	scene->_nextScene->Initialize();

	CoUninitialize();

	scene->_nextScene->SetCompletionLoading(1.0f);
}
