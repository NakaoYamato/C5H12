#include "TerrainDeformer.h"

#include "../../Library/Input/Input.h"
#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Graphics/GpuResourceManager.h"
#include "../../Library/Scene/Scene.h"
#include "../../Library/Algorithm/Converter.h"
#include "../../Library/Exporter/Exporter.h"
#include "../../Library/Actor/Stage/Terrain/TerrainActor.h"

#include "../../Library/Terrain/Brush/ColorAdditionBrush.h"
#include "../../Library/Terrain/Brush/HeightTransformingBrush.h"
#include "../../Library/Terrain/Brush/CostTransformingBrush.h"
#include "../../Library/Terrain/Brush/GrassTransformingBrush.h"

#include <filesystem>
#include <Mygui.h>

// 生成時処理
void TerrainDeformer::OnCreate()
{
	ID3D11Device* device = Graphics::Instance().GetDevice();

    // リソース取得
    _deformationResource = ResourceManager::Instance().GetResourceAs<TerrainDeformationResource>();

    // マテリアルマップのコピーピクセルシェーダの読み込み
    GpuResourceManager::CreatePsFromCso(
        device,
        "./Data/Shader/HLSL/Terrain/Deform/TerrainCopyMaterialPS.cso",
        _copyMaterialPS.ReleaseAndGetAddressOf());

    // 定数バッファの作成
    GpuResourceManager::CreateConstantBuffer(
        device,
        sizeof(ConstantBuffer),
        _constantBuffer.ReleaseAndGetAddressOf());

    // マテリアルマップのコピーバッファを作成
    _copyMaterialMapFB = std::make_unique<FrameBuffer>(
        device,
        Terrain::MaterialMapSize, Terrain::MaterialMapSize, true,
        std::vector<DXGI_FORMAT>(
            { 
                DXGI_FORMAT_R8G8B8A8_UNORM,
                DXGI_FORMAT_R8G8B8A8_UNORM,
                DXGI_FORMAT_R16G16B16A16_FLOAT
            }));
	// フルスクリーンクアッドのスプライトリソースを作成
    _fullscreenQuad = std::make_unique<SpriteResource>(device,
        L"",
        "./Data/Shader/HLSL/Terrain/Deform/TerrainDeformerVS.cso",
        "./Data/Shader/HLSL/Sprite/FullscreenQuadPS.cso");

    // ブラシ設定
	RegisterBrush(std::make_shared<ColorAdditionBrush>(this));
	RegisterBrush(std::make_shared<HeightTransformingBrush>(this));
	RegisterBrush(std::make_shared<CostTransformingBrush>(this));
	RegisterBrush(std::make_shared<GrassTransformingBrush>(this));
	// 初期ブラシの選択
	_selectedBrushName = _brushes.begin()->first;
}
// 更新処理
void TerrainDeformer::Update(float elapsedTime)
{
    // ブラシのGUI描画
    DrawBrushGui();

    // ブラシ使用フラグがオフの場合は何もしない
    if (!_useBrush)
        return;
#ifdef USE_IMGUI
    //	ウィンドウにフォーカス中の場合は処理しない
    if (ImGui::IsAnyItemActive() || ImGui::IsAnyItemFocused() || ImGui::IsAnyItemHovered())
    {
        // 前フレームのGUI操作フラグをオンにする
        _wasGuiActive = true;
        return;
    }
    // 前フレームのGUI操作フラグがオンの場合は左クリックを押されるまで処理しない
    if (_wasGuiActive)
    {
        if (_INPUT_RELEASED("LeftClick"))
            _wasGuiActive = false;
        return;
    }
#endif
    // 選択中のブラシ取得
    auto selectedBrush = _brushes.find(_selectedBrushName);
    // 選択中のブラシが存在しない場合は何もしない
    if (selectedBrush == _brushes.end())
        return;

    Vector3 mousePos{};
    mousePos.x = Input::Instance().GetMouseInput()->GetCurrentCursorPosX();
    mousePos.y = Input::Instance().GetMouseInput()->GetCurrentCursorPosY();
    float screenWidth = Graphics::Instance().GetScreenWidth();
    float screenHeight = Graphics::Instance().GetScreenHeight();
    auto& view = GetActor()->GetScene()->GetMainCamera()->GetView();
    auto& projection = GetActor()->GetScene()->GetMainCamera()->GetProjection();

    // マウス座標から地形との当たり判定
    mousePos.z = 0.0f;
    Vector3 rayStart = mousePos.Unproject(screenWidth, screenHeight, view, projection);
    mousePos.z = 1.0f;
    Vector3 rayEnd = mousePos.Unproject(screenWidth, screenHeight, view, projection);
    Vector3 rayDir = (rayEnd - rayStart).Normalize();

    // 交差結果
    Vector3 intersectWorldPosition = Vector3::Zero;
    std::vector<std::shared_ptr<TerrainController>> terrainControllers;

    // レイキャストで地形との交差を検出
    float distance = _rayLength;
    Vector3 hitPosition{}, hitNormal{};
    Actor* hitActor = nullptr;
    bool isIntersect = GetActor()->GetScene()->GetCollisionManager().RayCast(
        rayStart,
        rayDir,
        &distance,
        &hitPosition,
        &hitNormal,
        &hitActor);
    if (isIntersect)
    {
        intersectWorldPosition = hitPosition;

        // 接触点からブラシの半径内のTerrainを検索
        auto actors = GetActor()->GetScene()->GetCollisionManager().OverlapSphere(hitPosition, selectedBrush->second->GetBrushRadius());

		for (auto& actor : actors)
		{
			// 接触したアクターがTerrainControllerを持っているか確認
			auto terrainController = actor->GetComponent<TerrainController>();
			if (terrainController)
			{
				// TerrainControllerを取得
				terrainControllers.push_back(terrainController);
			}
		}
    }

    // 選択中のブラシを更新
    selectedBrush->second->Update(terrainControllers, elapsedTime, &intersectWorldPosition);

	// ブラシの行列を更新    
    if (selectedBrush->second->IsDrawDebugBrush())
    {
        DirectX::XMMATRIX M{}, S{}, R{}, T{};
        S = DirectX::XMMatrixScaling(
            selectedBrush->second->GetBrushRadius() * 4.0f,
            selectedBrush->second->GetBrushRadius() * 4.0f,
            selectedBrush->second->GetBrushRadius() * 40.0f);
        R = DirectX::XMMatrixRotationX(DirectX::XM_PIDIV2) *
            DirectX::XMMatrixRotationY(-selectedBrush->second->GetBrushRotationY());
        T = DirectX::XMMatrixTranslation(
            intersectWorldPosition.x,
            intersectWorldPosition.y,
            intersectWorldPosition.z);
        M = S * R * T;
        DirectX::XMStoreFloat4x4(&_brushMatrix, M);
    }
    else
    {
		// ブラシの行列をリセット
        _brushMatrix = {};
    }
}
// 描画処理
void TerrainDeformer::Render(const RenderContext& rc)
{
    // ブラシ使用フラグがオフの場合は何もしない
    if (!_useBrush)
        return;
    // ブラシの描画
    DrawBrush();
	// タスクがなければ何もしない
	if (_taskMap.empty())
		return;
	auto deformationResource = ResourceManager::Instance().GetResourceAs<TerrainDeformationResource>();
	if (!deformationResource)
		return;

    // キャッシュの保存
    GpuResourceManager::SaveStateCache(rc.deviceContext);

    // 定数バッファ設定
    ConstantBuffer constantBufferData{};
    rc.deviceContext->VSSetConstantBuffers(ModelCBIndex, 1, _constantBuffer.GetAddressOf());
    rc.deviceContext->PSSetConstantBuffers(ModelCBIndex, 1, _constantBuffer.GetAddressOf());

    // Terrainのパラメータマップフレームバッファを編集
    for (auto& [controller, tasks] : _taskMap)
    {
        // 地形の取得
        auto terrain = controller->GetTerrain().lock();
        if (!terrain)
            return;

        // 各ステート設定
        rc.deviceContext->OMSetBlendState(rc.renderState->GetBlendState(BlendState::None), nullptr, 0xFFFFFFFF);
        rc.deviceContext->OMSetDepthStencilState(rc.renderState->GetDepthStencilState(DepthState::NoTestNoWrite), 1);
        rc.deviceContext->RSSetState(rc.renderState->GetRasterizerState(RasterizerState::SolidCullNone));

        // 現在のマテリアルマップのコピー
        _copyMaterialMapFB->ClearAndActivate(rc.deviceContext, Vector4::Zero, 1.0f);
        ID3D11ShaderResourceView* copySRVs[] = {
            terrain->GetMaterialMapFB()->GetColorSRV(Terrain::BaseColorTextureIndex).Get(),
            terrain->GetMaterialMapFB()->GetColorSRV(Terrain::NormalTextureIndex).Get(),
            terrain->GetMaterialMapFB()->GetColorSRV(Terrain::ParameterTextureIndex).Get()
        };
        GetActor()->GetScene()->GetTextureRenderer().Blit(
            rc.deviceContext,
            copySRVs,
            0, _countof(copySRVs),
            _copyMaterialPS.Get()
        );
        _copyMaterialMapFB->Deactivate(rc.deviceContext);

        // 各マップのSRV設定
        ID3D11ShaderResourceView* materialSRVs[] =
        {
            _copyMaterialMapFB->GetColorSRV(Terrain::BaseColorTextureIndex).Get(),
            _copyMaterialMapFB->GetColorSRV(Terrain::NormalTextureIndex).Get(),
            _copyMaterialMapFB->GetColorSRV(Terrain::ParameterTextureIndex).Get()
        };
        rc.deviceContext->PSSetShaderResources(0, _countof(materialSRVs), materialSRVs);

        for (auto& task : tasks)
        {
            // ブラシ取得
            auto brush = _brushes.find(task.brushName);
            // ブラシが見つからない場合はスキップ
            if (brush == _brushes.end())
                continue;

            // ブラシのUV位置を設定
            constantBufferData.brushPosition = task.brushUVPosition;
            // ブラシの半径を設定
            constantBufferData.brushRadius = task.radius;
            // ブラシの強度を設定
            constantBufferData.brushStrength = task.strength;
            // テクスチャタイリング係数を設定
            constantBufferData.textureTillingScale = _textureTillingScale;
            // ブラシのY軸回転を設定
            constantBufferData.brushRotationY = task.brushRotationY;
            // パディングを設定
            constantBufferData.padding = task.padding;

            // 定数バッファ更新
            rc.deviceContext->UpdateSubresource(_constantBuffer.Get(), 0, 0, &constantBufferData, 0, 0);

            // ペイントテクスチャのSRVを設定
            auto& textureData = deformationResource->GetPaintTextures()[task.paintTextureIndex];
            rc.deviceContext->PSSetShaderResources(PaintBaseColorTextureIndex, 1, textureData.baseColorSRV.GetAddressOf());
            rc.deviceContext->PSSetShaderResources(PaintNormalTextureIndex, 1, textureData.normalSRV.GetAddressOf());
            rc.deviceContext->PSSetShaderResources(PaintHeightTextureIndex, 1, textureData.heightSRV.GetAddressOf());
            // ブラシのSRVを設定
            auto& brushTextureData = deformationResource->GetBrushTextures()[task.brushTextureIndex];
            rc.deviceContext->PSSetShaderResources(BrushTextureIndex, 1, brushTextureData.textureSRV.GetAddressOf());

            // ブラシの描画処理
            terrain->GetMaterialMapFB()->Activate(rc.deviceContext);
            brush->second->Render(
                _fullscreenQuad.get(),
                terrain,
                rc,
                materialSRVs,
                0, _countof(materialSRVs)
            );
            terrain->GetMaterialMapFB()->Deactivate(rc.deviceContext);
        }
    }
	// 変形タスクをクリア
    _taskMap.clear();

    // キャッシュの復元
    GpuResourceManager::RestoreStateCache(rc.deviceContext);
}
// GUI描画
void TerrainDeformer::DrawGui()
{
    if (ImGui::Checkbox(u8"ブラシ使用", &_useBrush))
        GetActor()->SetIsUsingGuizmo(!_useBrush);
    if (ImGui::Button(u8"Terrainの追加"))
    {
        std::string actorName = "Stage";
        for (int i = 0; i < 100; ++i)
        {
            std::string tempName = actorName + std::to_string(i);
            if (GetActor()->GetScene()->GetActorManager().FindByName(tempName) == nullptr)
            {
                actorName = tempName;
                break;
            }
        }
        auto terrainActor = GetActor()->GetScene()->RegisterActor<TerrainActor>(actorName, ActorTag::Stage);
    }
}
// ブラシの追加
void TerrainDeformer::RegisterBrush(std::shared_ptr<TerrainDeformerBrush> brush)
{
    _brushes[brush->GetName()] = brush;
}
// ブラシの描画
void TerrainDeformer::DrawBrush()
{
    auto deformationResource = ResourceManager::Instance().GetResourceAs<TerrainDeformationResource>();
    if (!deformationResource)
        return;
	if (!_useBrush || deformationResource->GetBrushTextures().empty() || _brushTextureIndex >= deformationResource->GetBrushTextures().size())
		return;
    ID3D11ShaderResourceView* nullsrvs[] = { nullptr };
    GetActor()->GetScene()->GetDecalRenderer().Draw(
		"TerrainBrush",
        deformationResource->GetBrushTextures()[_brushTextureIndex].textureSRV.GetAddressOf(),
        nullptr,
        _brushMatrix,
        Vector4(1.0f, 1.0f, 1.0f, 0.1f));
}
// ペイントテクスチャのGUI描画
void TerrainDeformer::DrawPaintTextureGui()
{
    auto deformationResource = ResourceManager::Instance().GetResourceAs<TerrainDeformationResource>();
    if (!deformationResource)
        return;

	auto& paintTextures = deformationResource->GetPaintTextures();
    for (size_t i = 0; i < paintTextures.size(); ++i)
    {
        if (ImGui::RadioButton(ToString(paintTextures[i].baseColorPath).c_str(), _paintTextureIndex == i))
            _paintTextureIndex = i;

        // ベースカラーテクスチャ
        ImGui::Image(paintTextures[i].baseColorSRV.Get(), { 128.0f, 128.0f });
        ImGui::SameLine();
		// 法線マップの編集
        ImGui::Image(paintTextures[i].normalSRV.Get(), { 128.0f, 128.0f });
        ImGui::SameLine();
		// 高さマップの編集
        ImGui::Image(paintTextures[i].heightSRV.Get(), { 128.0f, 128.0f });
		ImGui::Separator();
    }
}
// ブラシテクスチャのGUI描画
void TerrainDeformer::DrawBrushTextureGui()
{
    auto deformationResource = ResourceManager::Instance().GetResourceAs<TerrainDeformationResource>();
    if (!deformationResource)
        return;

	auto& brushTextures = deformationResource->GetBrushTextures();
    for (size_t i = 0; i < brushTextures.size(); ++i)
    {
        if (ImGui::RadioButton(ToString(brushTextures[i].path).c_str(), _brushTextureIndex == i))
            _brushTextureIndex = i;

        ImGui::Image(brushTextures[i].textureSRV.Get(), { 128.0f, 128.0f });
    }
}
// ブラシの選択GUI描画
void TerrainDeformer::DrawBrushSelectionGui()
{
    static std::vector<const char*> brushNames;
    if (brushNames.size() != _brushes.size())
    {
        brushNames.clear();
        for (const auto& [name, brush] : _brushes)
        {
            brushNames.push_back(name.c_str());
        }
    }
    ImGui::ComboString(u8"ブラシ選択", &_selectedBrushName, brushNames);
}
// ブラシのGUI描画
void TerrainDeformer::DrawBrushGui()
{
    if (_useBrush)
    {
        if (ImGui::Begin(u8"TerrainDeformer"))
        {
            // ブラシの選択GUI描画
            DrawBrushSelectionGui();
            ImGui::Separator();

            // ペイントテクスチャのGUI表示
            DrawPaintTextureGui();
            ImGui::Separator();

            ImGui::SliderFloat(u8"タイリング係数", &_textureTillingScale, 1.0f, 20.0f, "%.0f", 1.0f);
            ImGui::Separator();

            // ブラシテクスチャのGUI表示
            DrawBrushTextureGui();
        }
        ImGui::End();

        // ブラシ取得
        auto brush = _brushes.find(_selectedBrushName);
        // ブラシがあるならGUI描画
        if (brush != _brushes.end())
        {
            if (ImGui::Begin(u8"ブラシ"))
            {
                brush->second->DrawGui();
            }
            ImGui::End();
        }
    }
}

#pragma region TerrainDeformerBrush
// 更新処理
void TerrainDeformerBrush::Update(std::vector<std::shared_ptr<TerrainController>>& terrainControllers,
    float elapsedTime,
    Vector3* intersectWorldPosition)
{
    for (auto& terrainController : terrainControllers)
    {
        // 地形に接触していて左クリックしたら編集
        if (terrainController && intersectWorldPosition && _INPUT_PRESSED("LeftClick"))
        {
            Vector3 uv = intersectWorldPosition->TransformCoord(terrainController->GetActor()->GetTransform().GetMatrixInverse());
            Vector2 intersectUVPosition{};
            //intersectUVPosition.x = (uv.x + 1.0f) / 2.0f;
            //intersectUVPosition.y = (-uv.z + 1.0f) / 2.0f;
            intersectUVPosition.x = uv.x / TerrainRenderer::TerrainLength;
            intersectUVPosition.y = 1.0f - uv.z / TerrainRenderer::TerrainLength;
            float radius = _brushRadius / terrainController->GetActor()->GetTransform().GetScale().x / TerrainRenderer::TerrainLength;

            RegisterTask(terrainController,
                intersectUVPosition,
                radius,
                _brushStrength * elapsedTime);
        }
    }
}
// 描画処理
void TerrainDeformerBrush::Render(SpriteResource* fullscreenQuad, std::shared_ptr<Terrain> terrain, const RenderContext& rc, ID3D11ShaderResourceView** srv, uint32_t startSlot, uint32_t numViews)
{
    fullscreenQuad->Blit(
        rc.deviceContext,
        srv,
        startSlot, numViews,
        _pixelShader.Get()
    );
}
// GUI描画
void TerrainDeformerBrush::DrawGui()
{
    ImGui::DragFloat(u8"ブラシ半径", &_brushRadius, 0.01f, 0.0f, 100.0f);
    ImGui::DragFloat(u8"ブラシY軸回転(ラジアン)", &_brushRotationY, 0.01f, -DirectX::XM_PI, DirectX::XM_PI);
    ImGui::DragFloat(u8"ブラシ強度", &_brushStrength, 0.01f, -10.0f, 10.0f);
}
// タスクを登録
void TerrainDeformerBrush::RegisterTask(std::weak_ptr<TerrainController> terrainController, const Vector2& uvPosition, float radius, float strength)
{
    TerrainDeformer::Task task;
    task.brushName = GetName();
    task.brushUVPosition = uvPosition;
    task.paintTextureIndex = _deformer->GetPaintTextureIndex();
    task.brushTextureIndex = _deformer->GetBrushTextureIndex();
    task.radius = radius;
    task.strength = strength;
    task.brushRotationY = _brushRotationY;
    task.padding = _brushPadding;
    _deformer->AddTask(terrainController.lock().get(), task);
}
#pragma endregion
