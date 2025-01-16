#include "SceneDebug.h"

#include <imgui.h>

#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Input/Input.h"
#include "../../Library/DebugSupporter/DebugSupporter.h"
#include "../../Library/Camera/Camera.h"

#include "../../Library/Renderer/ModelRenderer.h"
#include "../../Library/Renderer/PrimitiveRenderer.h"

#include "../../Library/Actor/ActorManager.h"

//初期化
void SceneDebug::Initialize()
{
    float screenWidth = Graphics::Instance().GetScreenWidth();
    float screenHeight = Graphics::Instance().GetScreenHeight();
    // カメラ初期設定
    Camera::Instance().SetPerspectiveFov(
        DirectX::XMConvertToRadians(50),	// 画角
        screenWidth / screenHeight,			// 画面アスペクト比
        0.1f,								// ニアクリップ
        1000.0f								// ファークリップ
    );
    Camera::Instance().SetLookAt(
        { 0, 10, -10 },		// 視点
        { 0, 0, 0 },		// 注視点
        { 0, 1, 0 }			// 上ベクトル
    );
    ID3D11Device* device = Graphics::Instance().GetDevice();

    testModel = std::make_unique<Model>(device, "./Data/Model/Player/Mixamo.fbx");
}

//終了化 
void SceneDebug::Finalize()
{
    ActorManager::Clear();
}

//更新処理
void SceneDebug::Update(float elapsedTime)
{
    // ゲームオブジェクトの更新
    ActorManager::Update(elapsedTime);
    // 当たり判定処理
    ActorManager::Judge();

    testModel->UpdateTransform({
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1
        });
}

//描画処理
void SceneDebug::Render()
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
    dc->OMSetBlendState(renderState->GetBlendState(BlendState::Transparency), nullptr, 0xFFFFFFFF);
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

    // 描画の前処理
    ActorManager::RenderPreprocess(rc);

    // シーン定数バッファ、ライト定数バッファの更新
    cbManager->Update(rc);
    // シーン定数バッファの設定
    cbManager->SetCB(dc, 0, ConstantBufferType::SceneCB, ConstantUpdateTarget::ALL);
    // ライト定数バッファの設定
    cbManager->SetCB(dc, 3, ConstantBufferType::LightCB, ConstantUpdateTarget::ALL);

    ////--------------------------------------------------------------------------------------
    //// レンダーターゲットをフレームバッファ0番に設定
    //FrameBuffer* modelRenderBuffer = graphics.GetFrameBuffer(0);
    //modelRenderBuffer->ClearAndActive(dc);
    //{
    //    // ゲームオブジェクトの描画
    ActorManager::Render(rc);
    ModelRenderer::Draw(testModel.get(), VECTOR4_WHITE, ShaderId::Basic, ModelRenderType::Dynamic);
        // モデルの描画
    ModelRenderer::Render(rc);

        // プリミティブ描画
    PrimitiveRenderer::Render(dc, rc.camera->view_, rc.camera->projection_);

    //    // デバッグ描画
    //    Debug::Render(Camera::Instance().GetView(), Camera::Instance().GetProjection());
    //}
    //modelRenderBuffer->Deactivate(dc);
    //// フレームバッファ0番の処理終了
    ////--------------------------------------------------------------------------------------

    ////--------------------------------------------------------------------------------------
    //// カスケードシャドウマップ作成
    //CascadedShadowMap* cascadedShadowMap = graphics.GetCascadedShadowMap();
    //cascadedShadowMap->ClearAndActive(rc, 3/*cb_slot*/);
    //{
    //    // ゲームオブジェクトの影描画処理
    //    Actors::CastShadow(rc);

    //    // モデルの影描画処理
    //    modelRenderer->CastShadow(rc);

    //}
    //cascadedShadowMap->Deactivate(rc);
    //// カスケードシャドウマップの処理終了
    ////--------------------------------------------------------------------------------------

    ////--------------------------------------------------------------------------------------
    //// レンダーターゲットをフレームバッファ1番に設定
    //FrameBuffer* modelAndShadowRenderBuffer = graphics.GetFrameBuffer(1);
    //modelAndShadowRenderBuffer->ClearAndActive(dc);
    //{
    //    // 影の描画
    //    // cascadedShadowMapにある深度情報から
    //    // 0番のフレームバッファにあるシェーダーリソースに影を足して描画
    //    ID3D11ShaderResourceView* srvs[]
    //    {
    //        modelRenderBuffer->GetColorSRV().Get(), // color_map
    //        modelRenderBuffer->GetDepthSRV().Get(), // depth_map
    //        cascadedShadowMap->GetDepthMap().Get() // cascaded_shadow_maps
    //    };
    //    // cascadedShadowMapの定数バッファ更新
    //    cascadedShadowMap->UpdateCSMConstants(rc);
    //    // レンダーステート設定
    //    dc->OMSetDepthStencilState(rc.renderState->GetDepthStencilState(DepthState::NoTestNoWrite), 0);
    //    dc->RSSetState(rc.renderState->GetRasterizerState(RasterizerState::SolidCullNone));
    //    dc->OMSetBlendState(rc.renderState->GetBlendState(BlendState::None), nullptr, 0xFFFFFFFF);
    //    // サンプラーステート設定
    //    dc->PSSetSamplers(4, 1, rc.renderState->GetAddressOfSamplerState(SamplerState::BorderPoint));
    //    dc->PSSetSamplers(5, 1, rc.renderState->GetAddressOfSamplerState(SamplerState::Comparison));

    //    graphics.Blit(
    //        srvs,
    //        0, _countof(srvs),
    //        FullscreenQuadPS::CascadedPS);
    //}
    //modelAndShadowRenderBuffer->Deactivate(dc);
    //// フレームバッファ1番の処理終了
    ////--------------------------------------------------------------------------------------

    ////--------------------------------------------------------------------------------------
    //// フレームバッファ2番にブルームをかけたものを描画
    //FrameBuffer* bloomRenderBuffer = graphics.GetFrameBuffer(2);
    //bloomRenderBuffer->ClearAndActive(dc);
    //{
    //    PostProcessManager* postProcessManager = Graphics::Instance().GetPostProcessManager();
    //    // ブルーム処理
    //    PostProcessBase* bloomPP = postProcessManager->GetPostProcess(PostProcessType::BloomPP);
    //    bloomPP->Render(dc,
    //        modelAndShadowRenderBuffer->GetColorSRV().GetAddressOf(),
    //        0, 1);

    //    ID3D11ShaderResourceView* srvs[] =
    //    {
    //        modelAndShadowRenderBuffer->GetColorSRV().Get(),
    //        bloomPP->GetColorSRV().Get()
    //    };
    //    // 高輝度抽出し、ぼかしたものを加算描画
    //    dc->OMSetBlendState(rc.renderState->GetBlendState(BlendState::None), nullptr, 0xFFFFFFFF);
    //    dc->PSSetSamplers(0, 1, rc.renderState->GetAddressOfSamplerState(SamplerState::BorderPoint));
    //    graphics.Blit(
    //        srvs,
    //        0, _countof(srvs),
    //        FullscreenQuadPS::AddBloomPS);
    //}
    //bloomRenderBuffer->Deactivate(dc);
    //// フレームバッファ2番の処理終了
    ////--------------------------------------------------------------------------------------

    ////--------------------------------------------------------------------------------------
    //// ポストエフェクトの処理
    //PostProcessManager* postProcessManager = Graphics::Instance().GetPostProcessManager();
    //// ラジアルブラー
    //PostProcessBase* radialBlurPP = postProcessManager->GetPostProcess(PostProcessType::RadialBlurPP);
    //radialBlurPP->Render(dc,
    //    bloomRenderBuffer->GetColorSRV().GetAddressOf(),
    //    0, 1);
    //// ヴィネット
    //PostProcessBase* vignettePP = postProcessManager->GetPostProcess(PostProcessType::VignettePP);
    //vignettePP->Render(dc,
    //    radialBlurPP->GetColorSRV().GetAddressOf(),
    //    0, 1);
    //// 色収差
    //PostProcessBase* chromaticAberrationPP = postProcessManager->GetPostProcess(PostProcessType::ChromaticAberrationPP);
    //chromaticAberrationPP->Render(dc,
    //    vignettePP->GetColorSRV().GetAddressOf(),
    //    0, 1);
    //// カラーフィルター
    //PostProcessBase* colorFilterPP = postProcessManager->GetPostProcess(PostProcessType::ColorFilterPP);
    //colorFilterPP->Render(dc,
    //    chromaticAberrationPP->GetColorSRV().GetAddressOf(),
    //    0, 1);
    //// トーンマッピング
    //PostProcessBase* tonemappingPP = postProcessManager->GetPostProcess(PostProcessType::TonemappingPP);
    //tonemappingPP->Render(dc,
    //    colorFilterPP->GetColorSRV().GetAddressOf(),
    //    0, 1);
    //// ポストエフェクトの処理終了
    ////--------------------------------------------------------------------------------------

    //// サンプラーステート設定
    //{
    //    ID3D11SamplerState* samplerStates[] =
    //    {
    //        renderState->GetSamplerState(SamplerState::PointWrap),
    //        renderState->GetSamplerState(SamplerState::PointClamp),
    //        renderState->GetSamplerState(SamplerState::LinearWrap),
    //        renderState->GetSamplerState(SamplerState::LinearClamp)
    //    };
    //    dc->PSSetSamplers(0, _countof(samplerStates), samplerStates);
    //}
    //// バックバッファに描画
    //dc->OMSetBlendState(rc.renderState->GetBlendState(BlendState::None), nullptr, 0xFFFFFFFF);
    //graphics.Blit(
    //    tonemappingPP->GetColorSRV().GetAddressOf(),
    //    0, 1,
    //    FullscreenQuadPS::EmbeddedPS);

    // レンダーステート設定
    dc->OMSetBlendState(renderState->GetBlendState(BlendState::Alpha), nullptr, 0xFFFFFFFF);
    dc->OMSetDepthStencilState(renderState->GetDepthStencilState(DepthState::TestAndWrite), 0);
    dc->RSSetState(renderState->GetRasterizerState(RasterizerState::SolidCullBack));

    // 3D描画後の描画処理
    ActorManager::DelayedRender(rc);
}

// デバッグ用Gui描画
void SceneDebug::DrawGui()
{
    //　ゲームオブジェクトのGui表示
    ActorManager::DrawGui();

    //Graphics::Instance().GetCascadedShadowMap()->DrawGui();

    if (ImGui::Begin("tes"))
    {
        testModel->DrawGui();
    }
    ImGui::End();
}
