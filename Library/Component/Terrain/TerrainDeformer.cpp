#include "TerrainDeformer.h"

#include "../../Library/Input/Input.h"
#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Graphics/GpuResourceManager.h"
#include "../../Library/Scene/Scene.h"
#include "../../Library/DebugSupporter/DebugSupporter.h"
#include "../../Library/Algorithm/Converter.h"
#include "../../Library/Exporter/Exporter.h"

#include "../../Library/Terrain/Brush/ColorAdditionBrush.h"
#include "../../Library/Terrain/Brush/HeightTransformingBrush.h"
#include "../../Library/Terrain/Brush/CostTransformingBrush.h"
#include "../../Library/Terrain/Brush/TransparentWallBrush.h"
#include "../../Library/Terrain/Brush/ObjectLayoutBrush.h"

#include <filesystem>
#include <imgui.h>
#include <Mygui.h>

// ペイントテクスチャのデフォルトデータパス
static const char* DEFAULT_PAINT_TEXTURE_PATH = "./Data/Terrain/Debug/DefaultPaintTexture.json";
// ブラシテクスチャのデフォルトデータパス
static const char* DEFAULT_BRUSH_TEXTURE_PATH = "./Data/Terrain/Debug/DefaultBrushTexture.json";
// 配置するモデルデータのデフォルトデータパス
static const char* DEFAULT_MODEL_DATA_PATH = "./Data/Terrain/Debug/DefaultModelData.json";

// 生成時処理
void TerrainDeformer::OnCreate()
{
    // マテリアルマップのコピーピクセルシェーダの読み込み
    GpuResourceManager::CreatePsFromCso(
        Graphics::Instance().GetDevice(),
        "./Data/Shader/TerrainCopyMaterialPS.cso",
        _copyMaterialPS.ReleaseAndGetAddressOf());

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

    // デフォルトペイントテクスチャデータの読み込み
    {
        nlohmann::json jsonData;
        if (Exporter::LoadJsonFile(DEFAULT_PAINT_TEXTURE_PATH, &jsonData))
        {
            size_t size = jsonData["Size"].get<std::size_t>();
			for (size_t i = 0; i < size; ++i)
			{
				std::wstring colorPath = jsonData["ColorPath" + std::to_string(i)].get<std::wstring>();
				std::wstring normalPath = jsonData["NormalPath" + std::to_string(i)].get<std::wstring>();
                AddPaintTexture(colorPath, normalPath);
			}
        }
    }
	// デフォルトブラシテクスチャデータの読み込み
	{
		nlohmann::json jsonData;
		if (Exporter::LoadJsonFile(DEFAULT_BRUSH_TEXTURE_PATH, &jsonData))
		{
			size_t size = jsonData["Size"].get<std::size_t>();
			for (size_t i = 0; i < size; ++i)
			{
				std::wstring path = jsonData["Path" + std::to_string(i)].get<std::wstring>();
				AddBrushTexture(path);
			}
		}
	}
	// デフォルトモデルデータの読み込み
	{
		nlohmann::json jsonData;
		if (Exporter::LoadJsonFile(DEFAULT_MODEL_DATA_PATH, &jsonData))
		{
			size_t size = jsonData["Size"].get<std::size_t>();
			for (size_t i = 0; i < size; ++i)
			{
				std::string modelPath = jsonData["ModelPath" + std::to_string(i)].get<std::string>();
				AddModelData(modelPath);
			}
		}
	}

    // ブラシ設定
	RegisterBrush(std::make_shared<ColorAdditionBrush>(this));
	RegisterBrush(std::make_shared<HeightTransformingBrush>(this));
	RegisterBrush(std::make_shared<CostTransformingBrush>(this));
	RegisterBrush(std::make_shared<TransparentWallBrush>(this));
	RegisterBrush(std::make_shared<ObjectLayoutBrush>(this));
	// 初期ブラシの選択
	_selectedBrushName = _brushes.begin()->first;
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

    // 選択中のブラシ取得
    auto selectedBrush = _brushes.find(_selectedBrushName);
    // 選択中のブラシが存在しない場合は何もしない
    if (selectedBrush == _brushes.end())
        return;
    // 選択中のブラシを更新
    selectedBrush->second->Update(_terrainController.lock()->GetTerrain().lock(), elapsedTime);

    // ブラシの表示
    if (selectedBrush->second->IsDrawDebugBrush())
        Debug::Renderer::DrawSphere(
            selectedBrush->second->GetBrushWorldPosition(), selectedBrush->second->GetBrushRadius(),
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

        // ブラシの描画処理
		brush->second->Render(
			terrain,
			rc,
			materialSRVs,
			0, _countof(materialSRVs)
		);
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
	if (ImGui::Button(u8"現在のデータをデフォルトに設定"))
	{
        // デフォルトペイントテクスチャデータの書き込み
        {
            nlohmann::json jsonData;
			jsonData["Size"] = _paintTextures.size();
            for (size_t i = 0; i < _paintTextures.size(); ++i)
            {
				jsonData["ColorPath" + std::to_string(i)] = _paintTextures[i].baseColorPath;
				jsonData["NormalPath" + std::to_string(i)] = _paintTextures[i].normalPath;
            }
			Exporter::SaveJsonFile(DEFAULT_PAINT_TEXTURE_PATH, jsonData);
        }
        // デフォルトブラシテクスチャデータの書き込み
        {
			nlohmann::json jsonData;
			jsonData["Size"] = _brushTextures.size();
			for (size_t i = 0; i < _brushTextures.size(); ++i)
			{
				jsonData["Path" + std::to_string(i)] = _brushTextures[i].path;
			}
			Exporter::SaveJsonFile(DEFAULT_BRUSH_TEXTURE_PATH, jsonData);
        }
        // デフォルトモデルデータの書き込み
        {
            nlohmann::json jsonData;
			jsonData["Size"] = _environmentObjects.size();
            for (size_t i = 0; i < _environmentObjects.size(); ++i)
            {
                jsonData["ModelPath" + std::to_string(i)] = _environmentObjects[i].path;
            }
			Exporter::SaveJsonFile(DEFAULT_MODEL_DATA_PATH, jsonData);
        }
	}

    if (_useBrush)
    {
        if (ImGui::Begin(u8"TerrainDeformer"))
        {
            // ペイントテクスチャのGUI表示
            DrawPaintTextureGui();
            ImGui::Separator();

			ImGui::SliderFloat(u8"タイリング係数", &_textureTillingScale, 1.0f, 20.0f, "%.0f", 1.0f);
            ImGui::Separator();

            // ブラシテクスチャのGUI表示
            DrawBrushTextureGui();
            ImGui::Separator();

			// モデル選択GUI表示
			DrawModelSelectionGui();
			ImGui::Separator();

            // ブラシの選択GUI描画
            DrawBrushSelectionGui();
            ImGui::Separator();
        }
        ImGui::End();


        // ブラシ取得
        auto brush = _brushes.find(_selectedBrushName);
        // ブラシがあるならGUI描画
        if (brush != _brushes.end())
        {
            if (ImGui::Begin(u8"ブラシ"))
            {
                brush->second->DrawGui(_terrainController.lock()->GetTerrain().lock());
            }
            ImGui::End();
        }
    }
}
// 環境物を追加
void TerrainDeformer::AddEnvironmentObject(const std::string& modelPath, 
    TerrainObjectLayout::CollisionType collisionType,
    const Vector3& position, const Vector3& rotation, const Vector3& size)
{
    // 地形コントローラーが取得できていない場合は何もしない
    if (!_terrainController.lock())
        return;
    auto terrain = _terrainController.lock()->GetTerrain().lock();
    if (!terrain)
        return;
    // 選択中のモデルが登録されているか確認
    if (!terrain->GetTerrainObjectLayout()->HasModel(modelPath))
    {
        // モデルが登録されていない場合は登録
        terrain->GetTerrainObjectLayout()->AddModel(Graphics::Instance().GetDevice(), modelPath);
    }
    // オブジェクトを配置
    int layoutID = terrain->GetTerrainObjectLayout()->AddLayout(
        modelPath, 
        collisionType,
        position.TransformCoord(GetActor()->GetTransform().GetMatrixInverse()), // 位置をローカル座標系で設定
        rotation,
        size);
    // 描画用アクター生成
	_terrainController.lock()->CreateEnvironment(layoutID);
}
// ブラシの追加
void TerrainDeformer::RegisterBrush(std::shared_ptr<TerrainDeformerBrush> brush)
{
    _brushes[brush->GetName()] = brush;
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
// 配置するモデルデータの追加
void TerrainDeformer::AddModelData(const std::string& modelPath)
{
    ModelData modelData{};
    modelData.path = modelPath;
    _environmentObjects.push_back(modelData);
}
// ペイントテクスチャのGUI描画
void TerrainDeformer::DrawPaintTextureGui()
{
    for (size_t i = 0; i < _paintTextures.size(); ++i)
    {
        if (ImGui::RadioButton(ToString(_paintTextures[i].baseColorPath).c_str(), _paintTextureIndex == i))
            _paintTextureIndex = i;

        if (ImGui::ImageEditButton(&_paintTextures[i].baseColorPath, _paintTextures[i].baseColorSRV.Get()))
        {
            // テクスチャの読み込み
            GpuResourceManager::LoadTextureFromFile(
                Graphics::Instance().GetDevice(),
                _paintTextures[i].baseColorPath.c_str(),
                _paintTextures[i].baseColorSRV.ReleaseAndGetAddressOf(),
                nullptr);
        }
        ImGui::SameLine();
        if (ImGui::ImageEditButton(&_paintTextures[i].normalPath, _paintTextures[i].normalSRV.Get()))
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
        if (ImGui::ImageEditButton(&_brushTextures[i].path, _brushTextures[i].textureSRV.Get()))
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
// モデルの選択GUI描画
void TerrainDeformer::DrawModelSelectionGui()
{
	for (size_t i = 0; i < _environmentObjects.size(); ++i)
	{
		if (ImGui::RadioButton(_environmentObjects[i].path.c_str(), _selectedModelPath == _environmentObjects[i].path.c_str()))
            _selectedModelPath = _environmentObjects[i].path.c_str();
	}
    std::string resultStr = "";
    if (ImGui::OpenDialogBotton(u8"モデル追加", &resultStr, ImGui::ModelFilter))
    {
		AddModelData(resultStr);
        // 相対パス取得
		//_terrainController.lock()->GetTerrain().lock()->GetTerrainObjectLayout()->AddModel(
		//	Graphics::Instance().GetDevice(),
		//	resultStr);
    }
}
// ブラシの選択GUI描画
void TerrainDeformer::DrawBrushSelectionGui()
{
	for (const auto& [name, brush] : _brushes)
	{
		if (ImGui::RadioButton(name.c_str(), _selectedBrushName == name))
			_selectedBrushName = name;
	}
}
// 更新処理
void TerrainDeformerBrush::Update(std::shared_ptr<Terrain> terrain, float elapsedTime)
{
    Vector3 mousePos{};
    mousePos.x = _INPUT_VALUE("MousePositionX");
    mousePos.y = _INPUT_VALUE("MousePositionY");
    float screenWidth = Graphics::Instance().GetScreenWidth();
    float screenHeight = Graphics::Instance().GetScreenHeight();
    auto& view = _deformer->GetActor()->GetScene()->GetMainCamera()->GetView();
    auto& projection = _deformer->GetActor()->GetScene()->GetMainCamera()->GetProjection();

    // マウス座標から地形との当たり判定
    mousePos.z = 0.0f;
    Vector3 rayStart = mousePos.Unproject(screenWidth, screenHeight, view, projection);
    mousePos.z = 1.0f;
    Vector3 rayEnd = mousePos.Unproject(screenWidth, screenHeight, view, projection);
    Vector3 rayDir = (rayEnd - rayStart).Normalize();

    // 交差結果
    Vector3 intersectWorldPosition = Vector3::Zero;
    Vector2 intersectUVPosition = Vector2::Zero;

    // 地形との交差判定
    bool isIntersect = terrain->Raycast(
        _deformer->GetActor()->GetTransform().GetMatrix(),
        rayStart,
        rayDir,
        _rayLength,
        &intersectWorldPosition,
        nullptr, // 法線は不要
        &intersectUVPosition);

    _brushWorldPosition = intersectWorldPosition;
    // 地形に接触していて左クリックしたら編集
    if (isIntersect && _INPUT_PRESSED("LeftClick"))
    {
        RegisterTask(intersectUVPosition,
            _brushRadius / _deformer->GetActor()->GetTransform().GetScale().x,
            _brushStrength * elapsedTime);
    }
}
// GUI描画
void TerrainDeformerBrush::DrawGui(std::shared_ptr<Terrain> terrain)
{
    ImGui::DragFloat3(u8"ブラシワールド位置", &_brushWorldPosition.x, 0.01f, -100.0f, 100.0f);
    ImGui::DragFloat(u8"ブラシ半径", &_brushRadius, 0.01f, 0.0f, 100.0f);
    ImGui::DragFloat(u8"ブラシY軸回転(ラジアン)", &_brushRotationY, 0.01f, -DirectX::XM_PI, DirectX::XM_PI);
    ImGui::DragFloat(u8"ブラシ強度", &_brushStrength, 0.01f, -10.0f, 10.0f);
    ImGui::DragFloat(u8"高さ最小値", &_brushHeightScale.x, 0.01f, -100.0f, 0.0f);
    ImGui::DragFloat(u8"高さ最大値", &_brushHeightScale.y, 0.01f, 0.0f, 100.0f);
}
// タスクを登録
void TerrainDeformerBrush::RegisterTask(const Vector2& uvPosition, float radius, float strength)
{
	TerrainDeformer::Task task;
	task.brushName = GetName();
	task.brushUVPosition = uvPosition;
	task.paintTextureIndex = _deformer->GetPaintTextureIndex();
    task.brushTextureIndex = _deformer->GetBrushTextureIndex();
	task.radius = radius;
	task.strength = strength;
	task.brushRotationY = _brushRotationY;
	task.heightScale = _brushHeightScale;
	_deformer->AddTask(task);
}
