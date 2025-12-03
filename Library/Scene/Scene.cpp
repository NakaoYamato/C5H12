#include "Scene.h"

#include "SceneManager.h"
#include "../../Library/Graphics/Graphics.h"
#include "../../Library/JobSystem/JobSystem.h"
#include "../../Library/PostProcess/PostProcessManager.h"
#include "../../Library/Exporter/Exporter.h"
#include "../../Library/Graphics/GpuResourceManager.h"

#include "../../Library/Component/Light/LightController.h"

#include <imgui.h>

// 初期化
void Scene::Initialize()
{
    ProfileScopedSection_3(0, "Scene::Initialize", ImGuiControl::Profiler::Dark);
    Graphics& graphics = Graphics::Instance();
	ID3D11Device* device = graphics.GetDevice();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();

    // レンダーコンテキスト初期化
    _renderContext.deviceContext = dc;
    _renderContext.depthStencilView = graphics.GetDepthStencilView();
    _renderContext.renderState = Graphics::Instance().GetRenderState();
    _renderContext.camera = GetMainCamera();
    _renderContext.lightDirection = Vector4::Right;
    _renderContext.lightColor = Vector4::White;
    _renderContext.lightAmbientColor = Vector4::Black;
    _renderContext.environmentMap = nullptr;
    _renderContext.pointLights.clear();
    // サンプラーステート設定
    {
        std::lock_guard<std::mutex> lock(Graphics::Instance().GetMutex());
        RenderState* renderState = graphics.GetRenderState();
        std::vector<ID3D11SamplerState*> samplerStates;
        for (size_t index = 0; index < static_cast<int>(SamplerState::EnumCount); ++index)
        {
            samplerStates.push_back(renderState->GetSamplerState(static_cast<SamplerState>(index)));
        }
        dc->DSSetSamplers(0, static_cast<UINT>(samplerStates.size()), samplerStates.data());
        dc->GSSetSamplers(0, static_cast<UINT>(samplerStates.size()), samplerStates.data());
        dc->PSSetSamplers(0, static_cast<UINT>(samplerStates.size()), samplerStates.data());
    }

    // Effekseerエフェクトマネージャー作成
    {
        std::lock_guard<std::mutex> lock(Graphics::Instance().GetMutex());
        _effekseerEffectManager.Initialize(device, dc);
    }

    // 必須オブジェクト生成
    ActorManager& actorManager = GetActorManager();
    {
        std::shared_ptr<Actor> light = RegisterActor<Actor>(u8"Light", ActorTag::System);
        _directionalLight = light->AddComponent<LightController>();
		light->GetTransform().SetPositionY(1.0f);
        light->GetTransform().SetAngleX(DirectX::XMConvertToRadians(60.0f));
    }
    {
        _mainCameraActor = RegisterActor<MainCamera>(u8"MainCamera", ActorTag::System);
    }

    OnInitialize();

    // Terrainの頂点書き出し
    {
        std::lock_guard<std::mutex> lock(Graphics::Instance().GetMutex());
        GetTerrainRenderer().ExportVertex(GetRenderContext());
    }

	// コリジョンの初期化
    _collisionManager.Setup();

	// アクターマネージャーの開始関数を呼び出し
    _actorManager.Start();
}

// 終了化
void Scene::Finalize()
{
    _actorManager.Clear();

    OnFinalize();
}

//更新処理
void Scene::Update(float elapsedTime)
{
    // RCのデータをクリア
	GetRenderContext().pointLights.clear();

    // ゲームオブジェクトの更新
    _actorManager.Update(elapsedTime);

    // 当たり判定処理
	_collisionManager.Update();

    // ゲームオブジェクトの遅延更新処理
    _actorManager.LateUpdate(elapsedTime);

	// Effekseerの更新
    GetEffekseerEffectManager().Update(elapsedTime);

    {
        std::lock_guard<std::mutex> lock(Graphics::Instance().GetMutex());
        // パーティクルの更新
        GetParticleRenderer().Update(Graphics::Instance().GetDeviceContext(), elapsedTime);
    }

    // フェード更新
    GetFade()->Update(elapsedTime);

	// 時間の更新
	_time += elapsedTime;

    // 定数バッファの更新
    ConstantBufferManager* cbManager = Graphics::Instance().GetConstantBufferManager();
	cbManager->GetSceneCB().totalElapsedTime = _time;
	cbManager->GetSceneCB().deltaTime = elapsedTime;

    // グリッド表示
    if (_showGrid)
        Debug::Renderer::DrawGrid(10);

    // F1ボタンが有効ならImGuiのウィンドウに描画
    _isImGuiRendering = Debug::Input::IsActive(DebugInput::BTN_F1);
	// ImGuiのウィンドウに描画していないときはシーンImGuiウィンドウ選択フラグをオフにする
    if (!_isImGuiRendering)
        _isImGuiSceneWindowSelected = false;

	// シーンの更新処理
	OnUpdate(elapsedTime);

    // F2ボタンが有効ならスクリーンショット
	if (Debug::Input::IsActive(DebugInput::BTN_F2))
	{
        // 画面キャプチャ
        std::wstring filename = L"./Data/Debug/Capture/" + std::to_wstring(GetTickCount64()) + L".dds";

        if (Exporter::SaveDDSFile(
            Graphics::Instance().GetDevice(),
            Graphics::Instance().GetDeviceContext(),
            PostProcessManager::Instance().GetAppliedEffectSRV().Get(),
            filename))
        {
            // F2ボタンを無効化
            Debug::GetDebugInput()->buttonData ^= DebugInput::BTN_F2;
        }
	}
}

/// 一定間隔の更新処理
void Scene::FixedUpdate()
{
    // ゲームオブジェクトの更新
    _actorManager.FixedUpdate();

    OnFixedUpdate();
}

//描画処理
void Scene::Render()
{
    Graphics& graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = graphics.GetDeviceContext();
    ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
    ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();
    float screenWidth = graphics.GetScreenWidth();
	float screenHeight = graphics.GetScreenHeight();

    // 各種マネジャー取得
    RenderState* renderState = graphics.GetRenderState();
    ConstantBufferManager* cbManager = graphics.GetConstantBufferManager();

    // グレーで初期化
    FLOAT color[] = { 0.2f,0.2f,0.2f,1.0f };
    dc->ClearRenderTargetView(rtv, color);
    dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    dc->OMSetRenderTargets(1, &rtv, dsv);

	// ビューポート設定
	D3D11_VIEWPORT viewport{};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = screenWidth;
	viewport.Height = screenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	dc->RSSetViewports(1, &viewport);

    // サンプラーステート設定
    {
        std::vector<ID3D11SamplerState*> samplerStates;
        for (size_t index = 0; index < static_cast<int>(SamplerState::EnumCount); ++index)
        {
            samplerStates.push_back(renderState->GetSamplerState(static_cast<SamplerState>(index)));
        }
        dc->DSSetSamplers(0, static_cast<UINT>(samplerStates.size()), samplerStates.data());
        dc->GSSetSamplers(0, static_cast<UINT>(samplerStates.size()), samplerStates.data());
        dc->PSSetSamplers(0, static_cast<UINT>(samplerStates.size()), samplerStates.data());
    }

    // レンダーコンテキスト作成
    RenderContext& rc = GetRenderContext();
    rc.deviceContext = dc;
	rc.depthStencilView = dsv;
    rc.renderState = graphics.GetRenderState();
    rc.camera = GetMainCamera();
    rc.lightDirection = Vector4::Right;
    rc.lightColor = Vector4::White;
    rc.lightAmbientColor = Vector4::Black;
    if (_skyMap)
    {
        rc.environmentMap = _skyMap->GetSRV().GetAddressOf();
        // スカイマップのSRVを設定
        dc->PSSetShaderResources(_SKYMAP_COLOR_SRV_SLOT_INDEX,      1, _skyMap->GetSRV().GetAddressOf());
        dc->PSSetShaderResources(_SKYMAP_DIFFUSE_SRV_SLOT_INDEX,    1, _skyMap->GetDiffuseSRV().GetAddressOf());
        dc->PSSetShaderResources(_SKYMAP_SPECULAR_SRV_SLOT_INDEX,   1, _skyMap->GetSpecularSRV().GetAddressOf());
        dc->PSSetShaderResources(_SKYMAP_LUT_SRV_SLOT_INDEX,        1, _skyMap->GetLutSRV().GetAddressOf());
    }
    if (_directionalLight.lock())
    {
		auto light = _directionalLight.lock();
		rc.lightDirection = light->GetDirection();
		rc.lightColor = light->GetColor();
		rc.lightAmbientColor = light->GetAmbientColor();
	}

    // シーン定数バッファ、ライト定数バッファの更新
    cbManager->Update(rc);
    // シーン定数バッファの設定
    cbManager->SetCB(dc, _SCENE_CB_SLOT_INDEX, ConstantBufferType::SceneCB, ConstantUpdateTarget::ALL);
    // ライト定数バッファの設定
    cbManager->SetCB(dc, _LIGHT_CB_SLOT_INDEX, ConstantBufferType::LightCB, ConstantUpdateTarget::ALL);

    //--------------------------------------------------------------------------------------
    // GBuffer生成
    GBuffer* gBuffer = graphics.GetGBuffer();
    if (graphics.RenderingDeferred())
    {
        ProfileScopedSection_3(0, "GBuffer", ImGuiControl::Profiler::Blue);

        rc.depthStencilView = gBuffer->GetDepthStencilView().Get();

        // レンダーステート設定
        dc->OMSetDepthStencilState(renderState->GetDepthStencilState(DepthState::TestAndWrite), 0);
        dc->RSSetState(rc.renderState->GetRasterizerState(RasterizerState::SolidCullNone));
        dc->OMSetBlendState(renderState->GetBlendState(BlendState::MultipleRenderTargets), nullptr, 0xFFFFFFFF);

        gBuffer->ClearAndActivate(dc);
        {
            // ゲームオブジェクトの描画
            _actorManager.Render(rc);

            // モデルの描画
            GetMeshRenderer().RenderOpaque(rc, true);

            // Terrainの頂点書き出し
            GetTerrainRenderer().ExportVertex(GetRenderContext());

			// テレインの描画
            GetTerrainRenderer().Render(rc, true);
        }
        gBuffer->Deactivate(dc);

        // デカール描画
        GetDecalRenderer().Render(gBuffer, Graphics::Instance().GetDevice(), rc);
    }
    // GBuffer生成終了
    //--------------------------------------------------------------------------------------

    //--------------------------------------------------------------------------------------
    // フレームバッファ0番に空、GBuffer、その他レンダラーを描画
    FrameBuffer* renderFrame = graphics.GetFrameBuffer(_RENDER_FRAME_INDEX);
    renderFrame->ClearAndActivate(dc, Vector4::Zero, 1.0f);
    {
        ProfileScopedSection_3(0, "FrameBuffer0", ImGuiControl::Profiler::Blue);

        rc.depthStencilView = renderFrame->GetDSV().Get();

        // 空の描画
        if (_skyMap)
        {
            // ブレンドなし
            dc->OMSetBlendState(rc.renderState->GetBlendState(BlendState::None), nullptr, 0xFFFFFFFF);
            // 深度テストOFF
            dc->OMSetDepthStencilState(rc.renderState->GetDepthStencilState(DepthState::NoTestNoWrite), 1);
            // カリングを行わない
            dc->RSSetState(rc.renderState->GetRasterizerState(RasterizerState::SolidCullNone));

            _skyMap->Blit(rc);
        }

        // GBufferのデータを書き出し
        if (graphics.RenderingDeferred())
        {
            // レンダーステート設定
            dc->OMSetDepthStencilState(rc.renderState->GetDepthStencilState(DepthState::TestAndWrite), 0);
            dc->RSSetState(rc.renderState->GetRasterizerState(RasterizerState::SolidCullNone));
            dc->OMSetBlendState(rc.renderState->GetBlendState(BlendState::Alpha), nullptr, 0xFFFFFFFF);

            gBuffer->Blit(GetTextureRenderer(), dc);
        }
        else
        {
            // レンダーステート設定
            dc->OMSetDepthStencilState(renderState->GetDepthStencilState(DepthState::TestAndWrite), 0);
            dc->RSSetState(renderState->GetRasterizerState(RasterizerState::SolidCullBack));
            dc->OMSetBlendState(renderState->GetBlendState(BlendState::Opaque), nullptr, 0xFFFFFFFF);

            // ゲームオブジェクトの描画
            _actorManager.Render(rc);

            // フォワードレンダリング
            GetMeshRenderer().RenderOpaque(rc, false);

            // テレインの描画
            GetTerrainRenderer().Render(rc, true);
        }

        // モデルの描画
        GetMeshRenderer().RenderAlpha(rc);

		// Effekseerの描画
        GetEffekseerEffectManager().Render(rc.camera->GetView(), rc.camera->GetProjection());

		// パーティクルの描画
        dc->OMSetBlendState(rc.renderState->GetBlendState(BlendState::Additive), nullptr, 0xFFFFFFFF);
        GetParticleRenderer().Render(rc.deviceContext);
        dc->OMSetBlendState(rc.renderState->GetBlendState(BlendState::Alpha), nullptr, 0xFFFFFFFF);

        // プリミティブ描画
        {
            Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> copyColorSRV;
            D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
            srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Texture2D.MipLevels = 1;

            srvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
            GpuResourceManager::CreateShaderResourceViewCopy(
                Graphics::Instance().GetDevice(), dc,
                renderFrame->GetColorSRV().Get(),
                &srvDesc,
                copyColorSRV.ReleaseAndGetAddressOf()
            );
            //dc->PSSetShaderResources(2, 1, gBuffer->GetRenderTargetSRV(GBUFFER_COLOR_MAP_INDEX).GetAddressOf());
            dc->PSSetShaderResources(2, 1, copyColorSRV.GetAddressOf());
        }
        dc->OMSetBlendState(rc.renderState->GetBlendState(BlendState::Alpha), nullptr, 0xFFFFFFFF);
        GetPrimitiveRenderer().Render(rc);
        ID3D11ShaderResourceView* nullsrvs[] = { nullptr };
        dc->PSSetShaderResources(2, 1, nullsrvs);

        // デバッグ描画
        Debug::Renderer::Render(rc.camera->GetView(), rc.camera->GetProjection());
    }
    renderFrame->Deactivate(dc);
    // フレームバッファ0番の処理終了
    //--------------------------------------------------------------------------------------

    //--------------------------------------------------------------------------------------
    // カスケードシャドウマップ作成
    CascadedShadowMap* cascadedShadowMap = graphics.GetCascadedShadowMap();
    if (cascadedShadowMap->IsCreateShadow())
    {
        ProfileScopedSection_3(0, "CascadedShadowMap", ImGuiControl::Profiler::Blue);

        rc.depthStencilView = nullptr;

        cascadedShadowMap->ClearAndActivate(rc);
        {
            // ゲームオブジェクトの影描画処理
            _actorManager.CastShadow(rc);

            // モデルの影描画処理
            GetMeshRenderer().CastShadow(rc);

            // テレインの影描画処理
            GetTerrainRenderer().CastShadow(rc);
        }
        cascadedShadowMap->Deactivate(rc);
    }
    // カスケードシャドウマップの処理終了
    //--------------------------------------------------------------------------------------

    //--------------------------------------------------------------------------------------
    // レンダーターゲットをフレームバッファ1番に設定
    FrameBuffer* modelAndShadowRenderFrame = graphics.GetFrameBuffer(_APPLY_SHADOW_FRAME_INDEX);
    modelAndShadowRenderFrame->ClearAndActivate(dc, Vector4::Zero, 0.0f);
    {
        ProfileScopedSection_3(0, "FrameBuffer1", ImGuiControl::Profiler::Blue);

        rc.depthStencilView = modelAndShadowRenderFrame->GetDSV().Get();

        // 影の描画
        // cascadedShadowMapにある深度情報から
        // 0番のフレームバッファにあるシェーダーリソースに影を足して描画
        ID3D11ShaderResourceView* srvs[]
        {
            renderFrame->GetColorSRV().Get(), // color_map
            renderFrame->GetDepthSRV().Get(), // depth_map
            cascadedShadowMap->GetDepthMap().Get() // cascaded_shadow_maps
        };
        // cascadedShadowMapの定数バッファ更新
        cascadedShadowMap->UpdateCSMConstants(rc);
        // レンダーステート設定
        dc->OMSetDepthStencilState(rc.renderState->GetDepthStencilState(DepthState::TestAndWrite), 0);
        dc->RSSetState(rc.renderState->GetRasterizerState(RasterizerState::SolidCullNone));
        dc->OMSetBlendState(rc.renderState->GetBlendState(BlendState::Alpha), nullptr, 0xFFFFFFFF);

        cascadedShadowMap->Blit(dc,
            renderFrame->GetColorSRV().GetAddressOf(),
            renderFrame->GetDepthSRV().GetAddressOf());
    }
    modelAndShadowRenderFrame->Deactivate(dc);
    // フレームバッファ1番の処理終了
    //--------------------------------------------------------------------------------------

    //--------------------------------------------------------------------------------------
    // フレームバッファ2番にUIを含めたシーン画面を描画
    FrameBuffer* sceneFrame = graphics.GetFrameBuffer(_SCENE_FRAME_INDEX);
    sceneFrame->ClearAndActivate(dc, Vector4::Zero, 0.0f);
    {
        ProfileScopedSection_3(0, "FrameBuffer2", ImGuiControl::Profiler::Blue);

        rc.depthStencilView = sceneFrame->GetDSV().Get();

        //--------------------------------------------------------------------------------------
        // ポストエフェクトの処理
        PostProcessManager::Instance().ApplyEffect(rc,
            modelAndShadowRenderFrame->GetColorSRV().Get(),
            renderFrame->GetDepthSRV().Get());
        // ポストエフェクトの処理終了
        //--------------------------------------------------------------------------------------

        dc->OMSetBlendState(rc.renderState->GetBlendState(BlendState::None), nullptr, 0xFFFFFFFF);
        // バックバッファに描画
        GetTextureRenderer().Blit(
            dc,
            PostProcessManager::Instance().GetAppliedEffectSRV().GetAddressOf(),
            0, 1
        );

        // レンダーステート設定
        dc->OMSetBlendState(renderState->GetBlendState(BlendState::Alpha), nullptr, 0xFFFFFFFF);
        dc->OMSetDepthStencilState(renderState->GetDepthStencilState(DepthState::TestAndWrite), 0);
        dc->RSSetState(renderState->GetRasterizerState(RasterizerState::SolidCullNone));

        // 3D描画後の描画処理
        _actorManager.DelayedRender(rc);

        // インプットUI描画
        dc->OMSetBlendState(renderState->GetBlendState(BlendState::Alpha), nullptr, 0xFFFFFFFF);
        dc->OMSetDepthStencilState(renderState->GetDepthStencilState(DepthState::TestAndWrite), 0);
        dc->RSSetState(renderState->GetRasterizerState(RasterizerState::SolidCullNone));
        rc.deviceContext->ClearDepthStencilView(rc.depthStencilView, D3D11_CLEAR_STENCIL, 1.0f, 0);
        GetInputUI()->Render(rc, GetTextureRenderer());

        // テキスト描画
        GetTextRenderer().Render(rc, screenWidth, screenHeight);

		// フェード描画
		Vector4 fadeColor = Vector4::Black;
		fadeColor.w = GetFade()->GetAlpha();
        GetPrimitive()->Rect(dc,
            Vector2::Zero,
            Vector2(screenWidth, screenHeight),
            Vector2::Zero,
            0.0f,
            fadeColor);

        OnRender();
    }
    sceneFrame->Deactivate(dc);
    // フレームバッファ2番の処理終了
    //--------------------------------------------------------------------------------------

	// 描画先をバックバッファに戻す
    dc->OMSetRenderTargets(1, &rtv, dsv);
    dc->RSSetViewports(1, &viewport);
    // ImGuiに描画フラグが無効ならバックバッファに描画
    if (!_isImGuiRendering)
    {
        GetTextureRenderer().Blit(
            dc,
            sceneFrame->GetColorSRV().GetAddressOf(),
            0, 1
        );
    }
}

// デバッグ用Gui描画
void Scene::DrawGui()
{
    // ImGuiに描画フラグが無効ならバックバッファに描画
    if (_isImGuiRendering)
    {
        _isImGuiSceneWindowSelected = false;
        if (ImGui::Begin(u8"シーン", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
        {
            // ウィンドウ内の描画領域を取得
            ImVec2 windowPos = ImGui::GetWindowPos();
            ImVec2 windowSize = ImGui::GetWindowSize();
            windowPos.y += ImGui::GetFrameHeight();
            windowSize.y -= ImGui::GetFrameHeight();
            // 描画
            ImGui::GetWindowDrawList()->AddImage(
                Graphics::Instance().GetFrameBuffer(_SCENE_FRAME_INDEX)->GetColorSRV().Get(),
                windowPos,
                ImVec2(windowPos.x + windowSize.x, windowPos.y + windowSize.y)
            );
			// ImGuiのウィンドウが選択されているか
            _isImGuiSceneWindowSelected = 
                ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) ||
                ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows);
        }
        ImGui::End();
    }

    //　ゲームオブジェクトのGui表示
    _actorManager.DrawGui();

	//　当たり判定のGui表示
	_collisionManager.DrawGui();

	// パーティクルのGui表示
    {
        std::lock_guard<std::mutex> lock(Graphics::Instance().GetMutex());
        GetParticleRenderer().DrawGui(Graphics::Instance().GetDevice(), Graphics::Instance().GetDeviceContext());
    }

	// テレインレンダラーのGui表示
    GetTerrainRenderer().DrawGui();

    if(_skyMap)
        _skyMap->DrawGui();

    GetInputUI()->DrawGui();

    OnDrawGui();
}

// スカイマップ設定
void Scene::SetSkyMap(const wchar_t* filename, const wchar_t* diffuseIEM, const wchar_t* specularIDM)
{
    _skyMap = std::make_unique<SkyMap>(Graphics::Instance().GetDevice(), filename, diffuseIEM, specularIDM);
}

// 画面サイズ取得
float Scene::GetScreenWidth() const
{
    return Graphics::Instance().GetScreenWidth();
}

// 画面サイズ取得
float Scene::GetScreenHeight() const
{
    return Graphics::Instance().GetScreenHeight();
}

// プリミティブ取得
Primitive* Scene::GetPrimitive()
{
    return SceneManager::Instance().GetPrimitive();
}
// Effekseerエフェクトマネージャー取得
EffekseerEffectManager& Scene::GetEffekseerEffectManager()
{
    return _effekseerEffectManager;
}
// フェード情報取得
Fade* Scene::GetFade()
{
    return SceneManager::Instance().GetFade();
}
// 入力UI取得
InputUI* Scene::GetInputUI()
{
    return SceneManager::Instance().GetInputUI();
}
#pragma region 各種レンダラー取得
MeshRenderer& Scene::GetMeshRenderer()
{
    return SceneManager::Instance().GetMeshRenderer();
}

TextureRenderer& Scene::GetTextureRenderer()
{
    return SceneManager::Instance().GetTextureRenderer();
}

TextRenderer& Scene::GetTextRenderer()
{
    return SceneManager::Instance().GetTextRenderer();
}

TerrainRenderer& Scene::GetTerrainRenderer()
{
    return SceneManager::Instance().GetTerrainRenderer();
}

ParticleRenderer& Scene::GetParticleRenderer()
{
    return SceneManager::Instance().GetParticleRenderer();
}

PrimitiveRenderer& Scene::GetPrimitiveRenderer()
{
    return SceneManager::Instance().GetPrimitiveRenderer();
}

DecalRenderer& Scene::GetDecalRenderer()
{
    return SceneManager::Instance().GetDecalRenderer();
}
#pragma endregion
