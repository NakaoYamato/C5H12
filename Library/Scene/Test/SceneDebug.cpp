#include "SceneDebug.h"

#include <imgui.h>

#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Input/Input.h"
#include "../../Library/DebugSupporter/DebugSupporter.h"
#include "../../Library/Camera/Camera.h"
#include "../../Library/PostProcess/PostProcessManager.h"

#include "../../Library/Renderer/ModelRenderer.h"
#include "../../Library/Renderer/PrimitiveRenderer.h"
#include "../../Library/Renderer/ShapeRenderer.h"

#include "../../Library/Actor/ActorManager.h"

// コンポーネント
#include "../../Library/Component/SkyMapController.h"
#include "../../Library/Component/Light/LightController.h"
#include "../../Library/Component/Light/PointLightController.h"

#include "../../Library/Component/ShapeController.h"
#include "../../Library/Component/ModelController.h"
#include "../../Library/Component/Animator.h"

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

    // オブジェクト作成
    {
        std::shared_ptr<Actor> skyMap = ActorManager::Create(u8"スカイマップ", ActorTag::DrawContextParameter);
        skyMap->AddComponent<SkyMapController>(L"./Data/SkyMap/S0.dds");
    }
    {
        std::shared_ptr<Actor> light = ActorManager::Create(u8"Light", ActorTag::DrawContextParameter);
        auto lc = light->AddComponent<LightController>();
        lc->GetLight().SetDirection({ -0.012f,-0.819f,0.574f, 0.0f });
    }
    {
        std::shared_ptr<Actor> light = ActorManager::Create(u8"PointLight0", ActorTag::DrawContextParameter);
        light->AddComponent<PointLightController>();
    }
    {
        std::shared_ptr<Actor> light = ActorManager::Create(u8"PointLight1", ActorTag::DrawContextParameter);
        light->AddComponent<PointLightController>();
    }
    {
        auto stage = ActorManager::Create("Stage", ActorTag::Stage);
        stage->GetTransform().SetPositionY(-2.0f);
        auto modelCont = stage->AddComponent<ModelController>("./Data/Model/Stage/Land/Land1.fbx");
    }
    {
        auto player = ActorManager::Create("Player", ActorTag::Player);
        player->GetTransform().SetLengthScale(0.01f);
        //auto modelCont = player->AddComponent<ModelController>("./Data/Model/Player/HPmaid1.fbx");
        //auto modelCont = player->AddComponent<ModelController>("./Data/Model/Player/Test/Test.fbx");
        auto modelCont = player->AddComponent<ModelController>("./Data/Model/Player/HPmaidEyeBone.fbx");
        auto animator = player->AddComponent<Animator>(modelCont->GetModel());
    }
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

    //--------------------------------------------------------------------------------------
    // レンダーターゲットをフレームバッファ0番に設定
    FrameBuffer* modelRenderBuffer = graphics.GetFrameBuffer(0);
    modelRenderBuffer->ClearAndActive(dc);
    {
        // ゲームオブジェクトの描画
        ActorManager::Render(rc);
        // モデルの描画
        ModelRenderer::Render(rc);

        // シェイプ描画
        ShapeRenderer::Render(dc, rc.camera->view_, rc.camera->projection_);

        // プリミティブ描画
        PrimitiveRenderer::Render(dc, rc.camera->view_, rc.camera->projection_);

        // デバッグ描画
        Debug::Renderer::Render(Camera::Instance().GetView(), Camera::Instance().GetProjection());
    }
    modelRenderBuffer->Deactivate(dc);
    // フレームバッファ0番の処理終了
    //--------------------------------------------------------------------------------------

    //--------------------------------------------------------------------------------------
    // カスケードシャドウマップ作成
    CascadedShadowMap* cascadedShadowMap = graphics.GetCascadedShadowMap();
    cascadedShadowMap->ClearAndActive(rc, 3/*cb_slot*/);
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
    modelAndShadowRenderFrame->ClearAndActive(dc);
    {
        // 影の描画
        // cascadedShadowMapにある深度情報から
        // 0番のフレームバッファにあるシェーダーリソースに影を足して描画
        ID3D11ShaderResourceView* srvs[]
        {
            modelRenderBuffer->GetColorSRV().Get(), // color_map
            modelRenderBuffer->GetDepthSRV().Get(), // depth_map
            cascadedShadowMap->GetDepthMap().Get() // cascaded_shadow_maps
        };
        // cascadedShadowMapの定数バッファ更新
        cascadedShadowMap->UpdateCSMConstants(rc);
        // レンダーステート設定
        dc->OMSetDepthStencilState(rc.renderState->GetDepthStencilState(DepthState::NoTestNoWrite), 0);
        dc->RSSetState(rc.renderState->GetRasterizerState(RasterizerState::SolidCullNone));
        dc->OMSetBlendState(rc.renderState->GetBlendState(BlendState::None), nullptr, 0xFFFFFFFF);
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
    PostProcessManager::Instance().ApplyEffect(rc, modelAndShadowRenderFrame->GetColorSRV().GetAddressOf());
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
void SceneDebug::DrawGui()
{
    //　ゲームオブジェクトのGui表示
    ActorManager::DrawGui();
}
