#include "Scene.h"

#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Camera/Camera.h"
#include "../../Library/PostProcess/PostProcessManager.h"
#include "../../Library/DebugSupporter/DebugSupporter.h"

#include "../../Library/Renderer/MeshRenderer.h"
#include "../../Library/Renderer/PrimitiveRenderer.h"
#include "../../Library/Renderer/ShapeRenderer.h"

// 終了化
void Scene::Finalize()
{
    _actorManager.Clear();
}

//更新処理
void Scene::Update(float elapsedTime)
{
    // ゲームオブジェクトの更新
    _actorManager.Update(elapsedTime);
    // 当たり判定処理
    _actorManager.Judge();
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
    RenderingManager* renderingManager = graphics.GetRenderingManager();

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
    if (_skyMap)
    {
        rc.environmentMap = _skyMap->GetSRV().GetAddressOf();
        // スカイマップのSRVを設定
        dc->PSSetShaderResources(10, 1, _skyMap->GetSRV().GetAddressOf());
        dc->PSSetShaderResources(11, 1, _skyMap->GetDiffuseSRV().GetAddressOf());
        dc->PSSetShaderResources(12, 1, _skyMap->GetSpecularSRV().GetAddressOf());
        dc->PSSetShaderResources(13, 1, _skyMap->GetLutGGXSRV().GetAddressOf());
    }

    // 描画の前処理
    _actorManager.RenderPreprocess(rc);

    // シーン定数バッファ、ライト定数バッファの更新
    cbManager->Update(rc);
    // シーン定数バッファの設定
    cbManager->SetCB(dc, 0, ConstantBufferType::SceneCB, ConstantUpdateTarget::ALL);
    // ライト定数バッファの設定
    cbManager->SetCB(dc, 3, ConstantBufferType::LightCB, ConstantUpdateTarget::ALL);

    // ゲームオブジェクトの描画
    _actorManager.Render(rc);

    //--------------------------------------------------------------------------------------
    // GBuffer生成
    GBuffer* gBuffer = renderingManager->GetGBuffer();
    if (renderingManager->RenderingDeferred())
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
    FrameBuffer* renderFrame = renderingManager->GetFrameBuffer(0);
    renderFrame->ClearAndActivate(dc, Vector4(0.0f, 0.0f, 0.0f, 0.0f), 1.0f);
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
        if (renderingManager->RenderingDeferred())
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

        // シェイプ描画
        ShapeRenderer::Render(dc, rc.camera->view_, rc.camera->projection_);

        // プリミティブ描画
        PrimitiveRenderer::Render(dc, rc.camera->view_, rc.camera->projection_);
    }
    renderFrame->Deactivate(dc);
    // フレームバッファ0番の処理終了
    //--------------------------------------------------------------------------------------

    //--------------------------------------------------------------------------------------
    // カスケードシャドウマップ作成
    CascadedShadowMap* cascadedShadowMap = renderingManager->GetCascadedShadowMap();
    cascadedShadowMap->ClearAndActivate(rc, 3/*cb_slot*/);
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
    FrameBuffer* modelAndShadowRenderFrame = renderingManager->GetFrameBuffer(1);
    modelAndShadowRenderFrame->ClearAndActivate(dc, Vector4(0.0f,0.0f,0.0f,0.0f), 0.0f);
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
        // サンプラーステート設定
        dc->PSSetSamplers(4, 1, rc.renderState->GetAddressOfSamplerState(SamplerState::BorderPoint));
        dc->PSSetSamplers(5, 1, rc.renderState->GetAddressOfSamplerState(SamplerState::Comparison));

        renderingManager->Blit(
            dc,
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
    renderingManager->Blit(
        dc,
        PostProcessManager::Instance().GetAppliedEffectSRV().GetAddressOf(),
        0, 1,
        FullscreenQuadPS::EmbeddedPS);

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
}

// スカイマップ設定
void Scene::SetSkyMap(const wchar_t* filename, const wchar_t* diffuseIEM, const wchar_t* specularIDM)
{
    _skyMap = std::make_unique<SkyMap>(Graphics::Instance().GetDevice(), filename, diffuseIEM, specularIDM);
}
