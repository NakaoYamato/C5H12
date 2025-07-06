#include "TerrainDeformer.h"

#include "../../Library/Input/Input.h"
#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Graphics/GpuResourceManager.h"
#include "../../Library/Scene/Scene.h"
#include "../../Library/DebugSupporter/DebugSupporter.h"
#include "../../Library/Algorithm/Converter.h"

#include <filesystem>
#include <imgui.h>

// 生成時処理
void TerrainDeformer::OnCreate()
{
    // マテリアルマップのコピーピクセルシェーダの読み込み
    GpuResourceManager::CreatePsFromCso(
        Graphics::Instance().GetDevice(),
        "./Data/Shader/TerrainCopyMaterialPS.cso",
        _copyMaterialPS.ReleaseAndGetAddressOf());
    // 加算ブラシピクセルシェーダの読み込み
    GpuResourceManager::CreatePsFromCso(
        Graphics::Instance().GetDevice(),
        "./Data/Shader/TerrainDeformAddPS.cso",
        _addBrushPS.ReleaseAndGetAddressOf());
    // 高さ変形ブラシピクセルシェーダの読み込み
    GpuResourceManager::CreatePsFromCso(
        Graphics::Instance().GetDevice(),
        "./Data/Shader/TerrainDeformHeightPS.cso",
        _heightBrushPS.ReleaseAndGetAddressOf());
    // コスト変形ブラシピクセルシェーダの読み込み
    GpuResourceManager::CreatePsFromCso(
        Graphics::Instance().GetDevice(),
        "./Data/Shader/TerrainDeformCostPS.cso",
        _costBrushPS.ReleaseAndGetAddressOf());

    // 定数バッファの作成
    GpuResourceManager::CreateConstantBuffer(
        Graphics::Instance().GetDevice(),
        sizeof(ConstantBuffer),
        _constantBuffer.ReleaseAndGetAddressOf());

    // マテリアルマップのコピーバッファを作成
    _copyMaterialMapFB = std::make_unique<FrameBuffer>(
        Graphics::Instance().GetDevice(),
        Terrain::MaterialMapSize, Terrain::MaterialMapSize, true,
        std::vector<DXGI_FORMAT>({ DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT }));
    // 地形のハイトマップを格納するフレームバッファを作成
    _copyParameterMapFB = std::make_unique<FrameBuffer>(
        Graphics::Instance().GetDevice(),
        Terrain::ParameterMapSize, Terrain::ParameterMapSize, true);

    // デフォルトテクスチャデータの読み込み
    AddPaintTexture(
        L"./Data/Terrain/Paint/001_COLOR.png",
        L"./Data/Terrain/Paint/001_NORMAL.png");
    AddPaintTexture(
        L"./Data/Terrain/Paint/002_COLOR.png",
        L"./Data/Terrain/Paint/002_NORMAL.png");
    AddPaintTexture(
        L"./Data/Terrain/Paint/003_COLOR.jpg",
        L"./Data/Terrain/Paint/003_NORMAL.png");
    // デフォルトブラシテクスチャの読み込み
    AddBrushTexture(L"./Data/Terrain/Brush/Brush000.png");
    AddBrushTexture(L"./Data/Terrain/Brush/Brush001.png");
    AddBrushTexture(L"./Data/Terrain/Brush/Brush002.png");
}
// 開始処理
void TerrainDeformer::Start()
{
    // 地形コントローラーを取得
    _terrainController = GetActor()->GetComponent<TerrainController>();
}
// 更新処理
void TerrainDeformer::Update(float elapsedTime)
{
    // 地形コントローラーが取得できていない場合は何もしない
	if (!_terrainController.lock())
        return;
    // ブラシ使用フラグがオフの場合は何もしない
    if (!_useBrush)
        return;
	// テクスチャデータが空の場合は何もしない
	if (_paintTextures.empty())
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
	auto terrain = _terrainController.lock()->GetTerrain().lock();
    if (!terrain)
        return;

	Vector3 mousePos{};
	mousePos.x = _INPUT_VALUE("MousePositionX");
	mousePos.y = _INPUT_VALUE("MousePositionY");
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

	// UV座標を格納する変数
	Vector2 intersectUVPosition = Vector2::Zero;

    // 地形との交差判定
    bool isIntersect = terrain->Raycast(
        GetActor()->GetTransform().GetMatrix(),
        rayStart,
        rayDir,
        1000.0f, // レイの長さ
        &_intersectionWorldPoint,
		nullptr, // 法線は不要
        &intersectUVPosition);

	// 交差していて、かつブラシの入力が押されている場合は変形タスクを追加
    if (isIntersect && _INPUT_PRESSED("OK"))
    {
		Task task;
		task.mode = _brushMode;
        task.paintTextureIndex = _paintTextureIndex;
        task.brushTextureIndex = _brushTextureIndex;
		task.brushUVPosition = intersectUVPosition;
        // ブラシ半径をトランスフォームのサイズに影響されるようにする
		task.radius = brushRadius / GetActor()->GetTransform().GetScale().x;
        // ブラシの強度は経過時間によって減衰させる
		task.strength = brushStrength * elapsedTime;
        task.brushRotationY = _brushRotationY;
		task.heightScale = _brushHeightScale;
		AddTask(task);
    }

    // ブラシの表示
    Debug::Renderer::DrawSphere(
        _intersectionWorldPoint, brushRadius,
        Vector4::White);
}
// 描画処理
void TerrainDeformer::Render(const RenderContext& rc)
{
    // 地形コントローラーが取得できていない場合は何もしない
    if (!_terrainController.lock())
        return;
    // ブラシ使用フラグがオフの場合は何もしない
    if (!_useBrush)
        return;
	// タスクがなければ何もしない
	if (_tasks.empty())
		return;
    auto terrain = _terrainController.lock()->GetTerrain().lock();
    if (!terrain)
        return;
    auto& streamOutData = terrain->GetStreamOutData();
    if (streamOutData.empty())
        return;

    // キャッシュの保存
    GpuResourceManager::SaveStateCache(rc.deviceContext);

    // 各ステート設定
    rc.deviceContext->OMSetBlendState(rc.renderState->GetBlendState(BlendState::None), nullptr, 0xFFFFFFFF);
    rc.deviceContext->OMSetDepthStencilState(rc.renderState->GetDepthStencilState(DepthState::NoTestNoWrite), 1);
    rc.deviceContext->RSSetState(rc.renderState->GetRasterizerState(RasterizerState::SolidCullNone));

    // 現在のマテリアルマップのコピー
    _copyMaterialMapFB->ClearAndActivate(rc.deviceContext, Vector4::Zero, 1.0f);
	ID3D11ShaderResourceView* copySRVs[] = {
		terrain->GetMaterialMapFB()->GetColorSRV(0).Get(),
		terrain->GetMaterialMapFB()->GetColorSRV(1).Get()
	};
    GetActor()->GetScene()->GetTextureRenderer().Blit(
        rc.deviceContext,
        copySRVs,
        0, _countof(copySRVs),
		_copyMaterialPS.Get()
    );
    _copyMaterialMapFB->Deactivate(rc.deviceContext);
    // 現在のパラメータマップのコピー
    _copyParameterMapFB->ClearAndActivate(rc.deviceContext, Vector4::Zero, 1.0f);
    GetActor()->GetScene()->GetTextureRenderer().Blit(
        rc.deviceContext,
        terrain->GetParameterMapFB()->GetColorSRV().GetAddressOf(),
        0, 1
    );
    _copyParameterMapFB->Deactivate(rc.deviceContext);

    // 定数バッファ設定
    ConstantBuffer constantBufferData{};
    rc.deviceContext->PSSetConstantBuffers(ModelCBIndex, 1, _constantBuffer.GetAddressOf());

    // 各マップのSRV設定
	ID3D11ShaderResourceView* materialSRVs[] =
	{
		_copyMaterialMapFB->GetColorSRV(0).Get(),
		_copyMaterialMapFB->GetColorSRV(1).Get(),
		_copyParameterMapFB->GetColorSRV().Get(),
	};
    rc.deviceContext->PSSetShaderResources(0, _countof(materialSRVs), materialSRVs);

    // Terrainのパラメータマップフレームバッファを編集
    for (auto& task : _tasks)
    {
        // ブラシのUV位置を設定
        constantBufferData.brushPosition = task.brushUVPosition;
        // ブラシの半径を設定
        constantBufferData.brushRadius = task.radius;
        // ブラシの強度を設定
        constantBufferData.brushStrength = task.strength;
        // 高さ変形スケールを設定
        constantBufferData.heightScale = task.heightScale;
        // ブラシのY軸回転を設定
        constantBufferData.brushRotationY = task.brushRotationY;

        // 定数バッファ更新
        rc.deviceContext->UpdateSubresource(_constantBuffer.Get(), 0, 0, &constantBufferData, 0, 0);

		// ペイントテクスチャのSRVを設定
		auto& textureData = _paintTextures[task.paintTextureIndex];
		rc.deviceContext->PSSetShaderResources(PaintBaseColorTextureIndex, 1, textureData.baseColorSRV.GetAddressOf());
		rc.deviceContext->PSSetShaderResources(PaintNormalTextureIndex, 1, textureData.normalSRV.GetAddressOf());
		// ブラシのSRVを設定
        auto& brushTextureData = _brushTextures[task.brushTextureIndex];
		rc.deviceContext->PSSetShaderResources(BrushTextureIndex, 1, brushTextureData.textureSRV.GetAddressOf());

        // ブラシの種類によって処理を切り替える
        switch (_brushMode)
        {
        case BrushMode::Add:
            terrain->GetMaterialMapFB()->Activate(rc.deviceContext);
            GetActor()->GetScene()->GetTextureRenderer().Blit(
                rc.deviceContext,
                materialSRVs,
                0, _countof(materialSRVs),
                _addBrushPS.Get()
            );
            terrain->GetMaterialMapFB()->Deactivate(rc.deviceContext);
            break;
        case BrushMode::Height:
            terrain->GetParameterMapFB()->Activate(rc.deviceContext);
            GetActor()->GetScene()->GetTextureRenderer().Blit(
                rc.deviceContext,
                materialSRVs,
                0, _countof(materialSRVs),
                _heightBrushPS.Get()
            );
            terrain->GetParameterMapFB()->Deactivate(rc.deviceContext);
            break;
        case BrushMode::Cost:
            terrain->GetParameterMapFB()->Activate(rc.deviceContext);
            GetActor()->GetScene()->GetTextureRenderer().Blit(
                rc.deviceContext,
                materialSRVs,
                0, _countof(materialSRVs),
                _costBrushPS.Get()
            );
            terrain->GetParameterMapFB()->Deactivate(rc.deviceContext);
            break;
        }
    }
	// 変形タスクをクリア
	_tasks.clear();
    // 地形のストリームアウトを有効にする
    terrain->SetStreamOut(true);

    // キャッシュの復元
    GpuResourceManager::RestoreStateCache(rc.deviceContext);
}
// GUI描画
void TerrainDeformer::DrawGui()
{
    if (!_terrainController.lock()->GetTerrain().lock())
		return;

    if (ImGui::Checkbox(u8"ブラシ使用", &_useBrush))
        GetActor()->SetUseGuizmoFlag(!_useBrush);

    if (_useBrush)
    {
        if (ImGui::Begin(u8"TerrainDeformer"))
        {
            // ペイントテクスチャのGUI表示
            DrawPaintTextureGui();
            ImGui::Separator();

            // ブラシテクスチャのGUI表示
            DrawBrushTextureGui();
            ImGui::Separator();

            ImGui::Combo(u8"ブラシモード", reinterpret_cast<int*>(&_brushMode), u8"加算\0高さ\0コスト\0");
            ImGui::DragFloat3(u8"ブラシワールド位置", &_intersectionWorldPoint.x, 0.01f, -100.0f, 100.0f);
            ImGui::DragFloat(u8"ブラシ半径", &brushRadius, 0.01f, 0.0f, 100.0f);
            ImGui::DragFloat(u8"ブラシY軸回転(ラジアン)", &_brushRotationY, 0.01f, -DirectX::XM_PI, DirectX::XM_PI);
            if (_brushMode == BrushMode::Height)
            {
                ImGui::DragFloat(u8"ブラシ強度", &brushStrength, 0.01f, -10.0f, 10.0f);
                ImGui::DragFloat(u8"高さ最小値", &_brushHeightScale.x, 0.01f, -100.0f, 0.0f);
                ImGui::DragFloat(u8"高さ最大値", &_brushHeightScale.y, 0.01f, 0.0f, 100.0f);
            }
            else
            {
                ImGui::DragFloat(u8"ブラシ強度", &brushStrength, 0.01f, -10.0f, 10.0f);
            }
        }
        ImGui::End();
    }
}
// テクスチャ読み込み
void TerrainDeformer::LoadTexture(const std::wstring& path, ID3D11ShaderResourceView** srv)
{
    GpuResourceManager::LoadTextureFromFile(
        Graphics::Instance().GetDevice(),
        path.c_str(),
        srv,
        nullptr);
}
// ペイントテクスチャの追加
void TerrainDeformer::AddPaintTexture(const std::wstring& baseColorPath, const std::wstring& normalPath)
{
    PaintTexture& tex = _paintTextures.emplace_back();
    tex.baseColorPath = baseColorPath;
    tex.normalPath = normalPath;
    LoadTexture(baseColorPath, tex.baseColorSRV.GetAddressOf());
    LoadTexture(normalPath, tex.normalSRV.GetAddressOf());
}
// ブラシテクスチャの追加
void TerrainDeformer::AddBrushTexture(const std::wstring& path)
{
    BrushTexture& tex = _brushTextures.emplace_back();
    tex.path = path;
    LoadTexture(path, tex.textureSRV.GetAddressOf());
}
// SRVを表示して、クリックされたらパスを更新するダイアログを開く
bool TerrainDeformer::ShowAndEditImage(std::wstring* path, ID3D11ShaderResourceView* srv)
{
    if (ImGui::ImageButton(srv, ImVec2(128.0f, 128.0f)))
    {
        // ダイアログを開く
        std::string filepath;
        std::string currentDirectory;
        const char* filter = "Texture Files(*.dds;*.png;*.tga;*.jpg;*.tif)\0*.dds;*.png;*.tga;*.jpg;*.tif;\0All Files(*.*)\0*.*;\0\0";
        Debug::Dialog::DialogResult result = Debug::Dialog::OpenFileName(filepath, currentDirectory, filter);
        // ファイルを選択したら
        if (result == Debug::Dialog::DialogResult::Yes || result == Debug::Dialog::DialogResult::OK)
        {
            // 相対パス取得
            std::filesystem::path relativePath = std::filesystem::relative(filepath, currentDirectory);
            // パスを更新
            *path = relativePath.wstring();
            return true; // 画像がクリックされた
        }
    }
    return false; // 画像がクリックされなかった
}
// ペイントテクスチャのGUI描画
void TerrainDeformer::DrawPaintTextureGui()
{
    for (size_t i = 0; i < _paintTextures.size(); ++i)
    {
        if (ImGui::RadioButton(ToString(_paintTextures[i].baseColorPath).c_str(), _paintTextureIndex == i))
            _paintTextureIndex = i;

        if (ShowAndEditImage(&_paintTextures[i].baseColorPath, _paintTextures[i].baseColorSRV.Get()))
        {
            // テクスチャの読み込み
            GpuResourceManager::LoadTextureFromFile(
                Graphics::Instance().GetDevice(),
                _paintTextures[i].baseColorPath.c_str(),
                _paintTextures[i].baseColorSRV.ReleaseAndGetAddressOf(),
                nullptr);
        }
        ImGui::SameLine();
        if (ShowAndEditImage(&_paintTextures[i].normalPath, _paintTextures[i].normalSRV.Get()))
        {
            // テクスチャの読み込み
            GpuResourceManager::LoadTextureFromFile(
                Graphics::Instance().GetDevice(),
                _paintTextures[i].normalPath.c_str(),
                _paintTextures[i].normalSRV.ReleaseAndGetAddressOf(),
                nullptr);
        }
    }
    if (ImGui::Button(u8"ペイントテクスチャ追加"))
    {
        const char* filter = "Texture Files(*.dds;*.png;*.tga;*.jpg;*.tif)\0*.dds;*.png;*.tga;*.jpg;*.tif;\0All Files(*.*)\0*.*;\0\0";
        // ベースカラーのパスを取得するダイアログを開く
        std::string filepath;
        std::string currentDirectory;
        Debug::Dialog::DialogResult result = Debug::Dialog::OpenFileName(filepath, currentDirectory, filter);
        if (result == Debug::Dialog::DialogResult::Yes || result == Debug::Dialog::DialogResult::OK)
        {
            // 相対パス取得
            std::filesystem::path baseColorRelativePath = std::filesystem::relative(filepath, currentDirectory);
            // 法線マップのパスを取得するダイアログを開く
            result = Debug::Dialog::OpenFileName(filepath, currentDirectory, filter);
            if (result == Debug::Dialog::DialogResult::Yes || result == Debug::Dialog::DialogResult::OK)
            {
                // 相対パス取得
                std::filesystem::path normalRelativePath = std::filesystem::relative(filepath, currentDirectory);
                AddPaintTexture(baseColorRelativePath.wstring(), normalRelativePath.wstring());
            }
        }
    }
}
// ブラシテクスチャのGUI描画
void TerrainDeformer::DrawBrushTextureGui()
{
    for (size_t i = 0; i < _brushTextures.size(); ++i)
    {
        if (ImGui::RadioButton(ToString(_brushTextures[i].path).c_str(), _brushTextureIndex == i))
            _brushTextureIndex = i;
        if (ShowAndEditImage(&_brushTextures[i].path, _brushTextures[i].textureSRV.Get()))
        {
            // テクスチャの読み込み
            LoadTexture(_brushTextures[i].path, _brushTextures[i].textureSRV.ReleaseAndGetAddressOf());
        }
    }
    if (ImGui::Button(u8"ブラシテクスチャ追加"))
    {
        const char* filter = "Texture Files(*.dds;*.png;*.tga;*.jpg;*.tif)\0*.dds;*.png;*.tga;*.jpg;*.tif;\0All Files(*.*)\0*.*;\0\0";
        // ブラシテクスチャのパスを取得するダイアログを開く
        std::string filepath;
        std::string currentDirectory;
        Debug::Dialog::DialogResult result = Debug::Dialog::OpenFileName(filepath, currentDirectory, filter);
        if (result == Debug::Dialog::DialogResult::Yes || result == Debug::Dialog::DialogResult::OK)
        {
            // 相対パス取得
            std::filesystem::path relativePath = std::filesystem::relative(filepath, currentDirectory);
            BrushTexture& newBrushTexture = _brushTextures.emplace_back();
            newBrushTexture.path = relativePath.wstring();
            // テクスチャの読み込み
            LoadTexture(newBrushTexture.path, newBrushTexture.textureSRV.ReleaseAndGetAddressOf());
        }
    }
}
