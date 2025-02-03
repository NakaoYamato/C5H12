#include "Scene.h"

#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Camera/Camera.h"
#include "../../Library/PostProcess/PostProcessManager.h"
#include "../../Library/DebugSupporter/DebugSupporter.h"

#include "../../Library/Renderer/ModelRenderer.h"
#include "../../Library/Renderer/PrimitiveRenderer.h"
#include "../../Library/Renderer/ShapeRenderer.h"

#include "../../Library/Actor/ActorManager.h"

//更新処理
void Scene::Update(float elapsedTime)
{
    // ゲームオブジェクトの更新
    ActorManager::Update(elapsedTime);
    // 当たり判定処理
    ActorManager::Judge();
}

//描画処理
void Scene::Render()
{
    Graphics& graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = graphics.GetDeviceContext();
    ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
    ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

    RenderState* renderState = graphics.GetRenderState();
    ConstantBufferManager* cbManager = graphics.GetConstantBufferManager();

    FLOAT color[] = { 0.2f,0.2f,0.2f,1.0f };
    dc->ClearRenderTargetView(rtv, color);
    dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    dc->OMSetRenderTargets(1, &rtv, dsv);

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
    dc->OMSetDepthStencilState(renderState->GetDepthStencilState(DepthState::TestAndWrite), 0);
    dc->RSSetState(renderState->GetRasterizerState(RasterizerState::SolidCullBack));

    // レンダーコンテキスト作成
    RenderContext rc{};
    rc.deviceContext = dc;
    rc.renderState = graphics.GetRenderState();
    rc.camera = &Camera::Instance().GetDate();
    rc.lightDirection = { 1,0,0,0 };
    rc.lightColor = { 1,1,1,1 };
    rc.lightAmbientColor = { 0,0,0,0 };
    if (skyMap_)
        rc.environmentMap = skyMap_->GetSRV();

    // 描画の前処理
    ActorManager::RenderPreprocess(rc);

    // シーン定数バッファ、ライト定数バッファの更新
    cbManager->Update(rc);
    // シーン定数バッファの設定
    cbManager->SetCB(dc, 0, ConstantBufferType::SceneCB, ConstantUpdateTarget::ALL);
    // ライト定数バッファの設定
    cbManager->SetCB(dc, 3, ConstantBufferType::LightCB, ConstantUpdateTarget::ALL);

    // ゲームオブジェクトの描画
    ActorManager::Render(rc);

    //--------------------------------------------------------------------------------------
    // GBufferに書き込み
    GBuffer* gBuffer = graphics.GetGBuffer();
    gBuffer->ClearAndActivate(dc);
    {
        // モデルの描画
        ModelRenderer::Render(rc);

        // シェイプ描画
        ShapeRenderer::Render(dc, rc.camera->view_, rc.camera->projection_);

        // プリミティブ描画
        PrimitiveRenderer::Render(dc, rc.camera->view_, rc.camera->projection_);
    }
    gBuffer->Deactivate(dc);
    // GBufferに書き込み終了
    //--------------------------------------------------------------------------------------
    
    //--------------------------------------------------------------------------------------
    // カスケードシャドウマップ作成
    CascadedShadowMap* cascadedShadowMap = graphics.GetCascadedShadowMap();
    cascadedShadowMap->ClearAndActivate(rc, 3/*cb_slot*/);
    {
        // ゲームオブジェクトの影描画処理
        ActorManager::CastShadow(rc);

        // モデルの影描画処理
        ModelRenderer::CastShadow(rc);
    }
    cascadedShadowMap->Deactivate(rc);
    // カスケードシャドウマップの処理終了
    //--------------------------------------------------------------------------------------

    //--------------------------------------------------------------------------------------
    // レンダーターゲットをフレームバッファ1番に設定
    FrameBuffer* modelAndShadowRenderFrame = graphics.GetFrameBuffer(1);
    modelAndShadowRenderFrame->ClearAndActivate(dc);
    {
        // 空の描画
        if (skyMap_)
            skyMap_->Blit(rc);

        // 影の描画
        // cascadedShadowMapにある深度情報から
        // 0番のフレームバッファにあるシェーダーリソースに影を足して描画
        ID3D11ShaderResourceView* srvs[]
        {
            gBuffer->GetColorSRV().Get(), // color_map
            gBuffer->GetDepthStencilSRV().Get(), // depth_map
            cascadedShadowMap->GetDepthMap().Get() // cascaded_shadow_maps
        };
        // cascadedShadowMapの定数バッファ更新
        cascadedShadowMap->UpdateCSMConstants(rc);
        // レンダーステート設定
        dc->OMSetDepthStencilState(rc.renderState->GetDepthStencilState(DepthState::TestAndWrite), 0);
        dc->RSSetState(rc.renderState->GetRasterizerState(RasterizerState::SolidCullNone));
        dc->OMSetBlendState(rc.renderState->GetBlendState(BlendState::Alpha), nullptr, 0xFFFFFFFF);
        // サンプラーステート設定
        dc->PSSetSamplers(4, 1, rc.renderState->GetAddressOfSamplerState(SamplerState::BorderPoint));
        dc->PSSetSamplers(5, 1, rc.renderState->GetAddressOfSamplerState(SamplerState::Comparison));

        graphics.Blit(
            srvs,
            0, _countof(srvs),
            FullscreenQuadPS::CascadedPS);
    }
    modelAndShadowRenderFrame->Deactivate(dc);
    // フレームバッファ1番の処理終了
    //--------------------------------------------------------------------------------------

    //--------------------------------------------------------------------------------------
    // ポストエフェクトの処理
    ID3D11ShaderResourceView* srv[] =
    {
        modelAndShadowRenderFrame->GetColorSRV().Get(),
        modelAndShadowRenderFrame->GetDepthSRV().Get(),
    };
    PostProcessManager::Instance().ApplyEffect(rc, srv);
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
    graphics.Blit(
        PostProcessManager::Instance().GetAppliedEffectSRV().GetAddressOf(),
        0, 1,
        FullscreenQuadPS::EmbeddedPS);

    // レンダーステート設定
    dc->OMSetBlendState(renderState->GetBlendState(BlendState::Alpha), nullptr, 0xFFFFFFFF);
    dc->OMSetDepthStencilState(renderState->GetDepthStencilState(DepthState::TestAndWrite), 0);
    dc->RSSetState(renderState->GetRasterizerState(RasterizerState::SolidCullBack));

    // 3D描画後の描画処理
    ActorManager::DelayedRender(rc);
}

// デバッグ用Gui描画
void Scene::DrawGui()
{
    //　ゲームオブジェクトのGui表示
    ActorManager::DrawGui();
}

// スカイマップ設定
void Scene::SetSkyMap(const wchar_t* filename)
{
    skyMap_ = std::make_unique<SkyMap>(Graphics::Instance().GetDevice(), filename);
}
