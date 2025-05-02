#include "Scene.h"

#include "../../Library/Graphics/Graphics.h"
#include "../../Library/JobSystem/JobSystem.h"
#include "../../Library/PostProcess/PostProcessManager.h"
#include "../../Library/DebugSupporter/DebugSupporter.h"

#include "../../Library/Renderer/MeshRenderer.h"
#include "../../Library/Renderer/PrimitiveRenderer.h"

#include "../../Library/Component/Light/LightController.h"
#include "../../Library/Actor/Camera/MainCamera.h"

// 初期化
void Scene::Initialize()
{
    _fullscreenQuad = std::make_unique<Sprite>(Graphics::Instance().GetDevice(),
        L"",
        "./Data/Shader/FullscreenQuadVS.cso",
        "./Data/Shader/SpritePS.cso");

    // 必須オブジェクト生成
    ActorManager& actorManager = GetActorManager();
    {
        std::shared_ptr<Actor> light = RegisterActor<Actor>(u8"Light", ActorTag::DrawContextParameter);
        _directionalLight = light->AddComponent<LightController>();
		light->GetTransform().SetPositionY(1.0f);
        light->GetTransform().SetAngleX(DirectX::XMConvertToRadians(60.0f));
    }
    {
        auto mainCamera = RegisterActor<MainCamera>(u8"MainCamera", ActorTag::DrawContextParameter);
    }
}

// 終了化
void Scene::Finalize()
{
    _actorManager.Clear();
}

//更新処理
void Scene::Update(float elapsedTime)
{
    // RCのデータをクリア
	GetRenderContext().pointLights.clear();

    // ゲームオブジェクトの更新
    _actorManager.Update(elapsedTime);

    // ジョブシステムの処理待機
	//JobSystem::Instance().WaitCalculationJob();

    // 当たり判定処理
    _actorManager.Judge();
}

/// 一定間隔の更新処理
void Scene::FixedUpdate()
{
    // ゲームオブジェクトの更新
    _actorManager.FixedUpdate();
}

//描画処理
void Scene::Render()
{
    Graphics& graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = graphics.GetDeviceContext();
    ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
    ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

    // 各種マネジャー取得
    RenderState* renderState = graphics.GetRenderState();
    ConstantBufferManager* cbManager = graphics.GetConstantBufferManager();

    // グレーで初期化
    FLOAT color[] = { 0.2f,0.2f,0.2f,1.0f };
    dc->ClearRenderTargetView(rtv, color);
    dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    dc->OMSetRenderTargets(1, &rtv, dsv);

    // サンプラーステート設定
    {
        std::vector<ID3D11SamplerState*> samplerStates;
        for (size_t index = 0; index < static_cast<int>(SamplerState::EnumCount); ++index)
        {
            samplerStates.push_back(renderState->GetSamplerState(static_cast<SamplerState>(index)));
        }
        dc->PSSetSamplers(0, static_cast<UINT>(samplerStates.size()), samplerStates.data());
    }

    // レンダーコンテキスト作成
    RenderContext& rc = GetRenderContext();
    rc.deviceContext = dc;
    rc.renderState = graphics.GetRenderState();
    rc.camera = GetMainCamera();
    rc.lightDirection = _VECTOR4_RIGHT;
    rc.lightColor = _VECTOR4_WHITE;
    rc.lightAmbientColor = _VECTOR4_BLACK;
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

    // レンダーステート設定
    dc->OMSetBlendState(renderState->GetBlendState(BlendState::Alpha), nullptr, 0xFFFFFFFF);
    dc->OMSetDepthStencilState(renderState->GetDepthStencilState(DepthState::TestAndWrite), 0);
    dc->RSSetState(renderState->GetRasterizerState(RasterizerState::SolidCullBack));

    // シーン定数バッファ、ライト定数バッファの更新
    cbManager->Update(rc);
    // シーン定数バッファの設定
    cbManager->SetCB(dc, _SCENE_CB_SLOT_INDEX, ConstantBufferType::SceneCB, ConstantUpdateTarget::ALL);
    // ライト定数バッファの設定
    cbManager->SetCB(dc, _LIGHT_CB_SLOT_INDEX, ConstantBufferType::LightCB, ConstantUpdateTarget::ALL);

    // ゲームオブジェクトの描画
    _actorManager.Render(rc);

    //--------------------------------------------------------------------------------------
    // GBuffer生成
    GBuffer* gBuffer = graphics.GetGBuffer();
    if (graphics.RenderingDeferred())
    {
        gBuffer->ClearAndActivate(dc);
        {
            // モデルの描画
            MeshRenderer::RenderOpaque(rc, true);
        }
        gBuffer->Deactivate(dc);
    }
    // GBuffer生成終了
    //--------------------------------------------------------------------------------------

    //--------------------------------------------------------------------------------------
    // フレームバッファ0番に空、GBuffer、その他レンダラーを描画
    FrameBuffer* renderFrame = graphics.GetFrameBuffer(_RENDER_FRAME_INDEX);
    renderFrame->ClearAndActivate(dc, _VECTOR4_ZERO, 1.0f);
    {
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

            gBuffer->Blit(dc);
        }
        else
        {
            // フォワードレンダリング
            MeshRenderer::RenderOpaque(rc, false);
        }

        // モデルの描画
        MeshRenderer::RenderAlpha(rc);

        // プリミティブ描画
        PrimitiveRenderer::Render(dc, rc.camera->GetView(), rc.camera->GetProjection());

        // デバッグ描画
        Debug::Renderer::Render(rc.camera->GetView(), rc.camera->GetProjection());
    }
    renderFrame->Deactivate(dc);
    // フレームバッファ0番の処理終了
    //--------------------------------------------------------------------------------------

    //--------------------------------------------------------------------------------------
    // カスケードシャドウマップ作成
    CascadedShadowMap* cascadedShadowMap = graphics.GetCascadedShadowMap();
    cascadedShadowMap->ClearAndActivate(rc);
    {
        // ゲームオブジェクトの影描画処理
        _actorManager.CastShadow(rc);

        // モデルの影描画処理
        MeshRenderer::CastShadow(rc);
    }
    cascadedShadowMap->Deactivate(rc);
    // カスケードシャドウマップの処理終了
    //--------------------------------------------------------------------------------------

    //--------------------------------------------------------------------------------------
    // レンダーターゲットをフレームバッファ1番に設定
    FrameBuffer* modelAndShadowRenderFrame = graphics.GetFrameBuffer(_APPLY_SHADOW_FRAME_INDEX);
    modelAndShadowRenderFrame->ClearAndActivate(dc, _VECTOR4_ZERO, 0.0f);
    {
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
    // ポストエフェクトの処理
    PostProcessManager::Instance().ApplyEffect(rc, 
        modelAndShadowRenderFrame->GetColorSRV().Get(),
        renderFrame->GetDepthSRV().Get());
    // ポストエフェクトの処理終了
    //--------------------------------------------------------------------------------------
    
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
    dc->OMSetBlendState(rc.renderState->GetBlendState(BlendState::None), nullptr, 0xFFFFFFFF);
    // バックバッファに描画
    _fullscreenQuad->Blit(
        dc,
        PostProcessManager::Instance().GetAppliedEffectSRV().GetAddressOf(),
        0, 1
    );

    // レンダーステート設定
    dc->OMSetBlendState(renderState->GetBlendState(BlendState::Alpha), nullptr, 0xFFFFFFFF);
    dc->OMSetDepthStencilState(renderState->GetDepthStencilState(DepthState::TestAndWrite), 0);
    dc->RSSetState(renderState->GetRasterizerState(RasterizerState::SolidCullBack));

    // 3D描画後の描画処理
    _actorManager.DelayedRender(rc);
}

// デバッグ用Gui描画
void Scene::DrawGui()
{
    //　ゲームオブジェクトのGui表示
    _actorManager.DrawGui();

    if(_skyMap)
        _skyMap->DrawGui();
}

// スカイマップ設定
void Scene::SetSkyMap(const wchar_t* filename, const wchar_t* diffuseIEM, const wchar_t* specularIDM)
{
    _skyMap = std::make_unique<SkyMap>(Graphics::Instance().GetDevice(), filename, diffuseIEM, specularIDM);
}
