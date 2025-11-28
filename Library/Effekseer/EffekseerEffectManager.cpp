#include "EffekseerEffectManager.h"

#include "../../Graphics/Graphics.h"
#include "../../Library/JobSystem/JobSystem.h"

/// 初期化
void EffekseerEffectManager::Initialize(ID3D11Device* device, ID3D11DeviceContext* dc)
{
    //Effekseerレンダラを作成
    effekseerRenderer = EffekseerRendererDX11::Renderer::Create(
        device, dc, 2048
    );

    //Effekseerマネージャー生成
    effekseerManager = Effekseer::Manager::Create(2048);

    //Effekseerレンダラの各種設定（特別なカスタマイズをしない場合は定型的に以下の設定でOK）
    effekseerManager->SetSpriteRenderer(effekseerRenderer->CreateSpriteRenderer());
    effekseerManager->SetRibbonRenderer(effekseerRenderer->CreateRibbonRenderer());
    effekseerManager->SetRingRenderer(effekseerRenderer->CreateRingRenderer());
    effekseerManager->SetTrackRenderer(effekseerRenderer->CreateTrackRenderer());
    effekseerManager->SetModelRenderer(effekseerRenderer->CreateModelRenderer());

    //Effekseer内でのローダーの設定（特別なカスタマイズをしない場合は以下の設定でＯＫ）
    effekseerManager->SetTextureLoader(effekseerRenderer->CreateTextureLoader());
    effekseerManager->SetModelLoader(effekseerRenderer->CreateModelLoader());
    effekseerManager->SetMaterialLoader(effekseerRenderer->CreateMaterialLoader());

    //Effekseerを左手座標系で計算する
    effekseerManager->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);
}

/// 更新処理
void EffekseerEffectManager::Update(float elapsedTime)
{
    effekseerManager->Update(elapsedTime * 60.0f);
}

/// 描画処理
void EffekseerEffectManager::Render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{
    ProfileScopedSection_3(0, "TerrainRenderer::Render", ImGuiControl::Profiler::Green);

    //ビュー&プロジェクション行列をEffekseerレンダラに設定
    effekseerRenderer->SetCameraMatrix(*reinterpret_cast<const Effekseer::Matrix44*>(&view));
    effekseerRenderer->SetProjectionMatrix(*reinterpret_cast<const Effekseer::Matrix44*>(&projection));

    //Effekseer描画開始
    effekseerRenderer->BeginRendering();

    //Effekseer描画実行
    //マネージャー単位で描画するので描画順を制御する場合はマネージャーを複数個作成し、
    //Draw()関数を実行する順序で制御できそう
    effekseerManager->Draw();

    //Effekseer描画終了
    effekseerRenderer->EndRendering();
}
