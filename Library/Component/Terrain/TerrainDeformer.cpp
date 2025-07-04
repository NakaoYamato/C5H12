#include "TerrainDeformer.h"

#include "../../Library/Input/Input.h"
#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Graphics/GpuResourceManager.h"
#include "../../Library/Scene/Scene.h"
#include "../../Library/DebugSupporter/DebugSupporter.h"
#include "../../Library/Algorithm/Converter.h"

#include <filesystem>
#include <imgui.h>

// 開始処理
void TerrainDeformer::Start()
{
    // 地形コントローラーを取得
    _terrainController = GetActor()->GetComponent<TerrainController>();

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
	// 減算ブラシピクセルシェーダの読み込み
	GpuResourceManager::CreatePsFromCso(
		Graphics::Instance().GetDevice(),
		"./Data/Shader/TerrainDeformSubtractPS.cso",
		_subtractBrushPS.ReleaseAndGetAddressOf());
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

	// テクスチャデータの読み込み
	LoadTextureData(
        L"./Data/Terrain/Texture/Material/001_COLOR.png",
		L"./Data/Terrain/Texture/Material/001_NORMAL.png");
	LoadTextureData(
        L"./Data/Terrain/Texture/Material/002_COLOR.png",
		L"./Data/Terrain/Texture/Material/002_NORMAL.png");
	LoadTextureData(
        L"./Data/Terrain/Texture/Material/003_COLOR.jpg",
		L"./Data/Terrain/Texture/Material/003_NORMAL.png");
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
	if (_textureDatas.empty())
		return;
#ifdef USE_IMGUI
	//	ウィンドウにフォーカス中の場合は何もしない
	if (ImGui::IsAnyItemFocused() || ImGui::IsAnyItemHovered())
		return;
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
        task.textureIndex = _textureIndex;
		task.brushUVPosition = intersectUVPosition;
        // ブラシ半径をトランスフォームのサイズに影響されるようにする
		task.radius = brushRadius / GetActor()->GetTransform().GetScale().x;
        // ブラシの強度は経過時間によって減衰させる
		task.strength = brushStrength * elapsedTime;
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

        // 定数バッファ更新
        rc.deviceContext->UpdateSubresource(_constantBuffer.Get(), 0, 0, &constantBufferData, 0, 0);

		// ブラシのSRVを設定
		auto& textureData = _textureDatas[task.textureIndex];
		rc.deviceContext->PSSetShaderResources(_countof(materialSRVs) + 0, 1, textureData.baseColorSRV.GetAddressOf());
		rc.deviceContext->PSSetShaderResources(_countof(materialSRVs) + 1, 1, textureData.normalSRV.GetAddressOf());

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
        case BrushMode::Subtract:
            terrain->GetMaterialMapFB()->Activate(rc.deviceContext);
            GetActor()->GetScene()->GetTextureRenderer().Blit(
                rc.deviceContext,
                materialSRVs,
                0, _countof(materialSRVs),
                _subtractBrushPS.Get()
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

	ImGui::Combo(u8"ブラシモード", reinterpret_cast<int*>(&_brushMode), u8"加算\0減算\0高さ変形\0コスト変形\0");
	for (size_t i = 0; i < _textureDatas.size(); ++i)
	{
		if (ImGui::RadioButton(ToString(_textureDatas[i].baseColorPath).c_str(), _textureIndex == i))
            _textureIndex = i;
        ImGui::Image(_textureDatas[i].baseColorSRV.Get(), ImVec2(128.0f, 128.0f));
        ImGui::SameLine();
        ImGui::Image(_textureDatas[i].normalSRV.Get(), ImVec2(128.0f, 128.0f));
	}
    ImGui::DragFloat3(u8"ブラシワールド位置", &_intersectionWorldPoint.x, 0.01f, -100.0f, 100.0f);
    ImGui::DragFloat(u8"ブラシ半径", &brushRadius, 0.01f, 0.0f, 100.0f);
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
	if (ImGui::Button(u8"パラメータマップ書き出し"))
	{
        // ダイアログを開く
        char filename[256] = { 0 };
        const char* filter = "Texture Files(*.dds)\0*.dds;\0All Files(*.*)\0*.*;\0\0";
        Debug::Dialog::DialogResult result = Debug::Dialog::SaveFileName(filename,
            sizeof(filename),
            filter,
            nullptr,
            "dds");
        // ファイルを選択したら
        if (result == Debug::Dialog::DialogResult::Yes || result == Debug::Dialog::DialogResult::OK)
        {
            std::string path = filename;
            _terrainController.lock()->GetTerrain().lock()->SaveParameterMap(
                Graphics::Instance().GetDevice(),
                Graphics::Instance().GetDeviceContext(),
                ToWString(path).c_str());
        }
	}
    if (ImGui::Button(u8"データマップ書き出し"))
    {
        // ダイアログを開く
        char filename[256] = { 0 };
        const char* filter = "Texture Files(*.dds)\0*.dds;\0All Files(*.*)\0*.*;\0\0";
        Debug::Dialog::DialogResult result = Debug::Dialog::SaveFileName(filename,
            sizeof(filename),
            filter,
            nullptr,
            "dds");
        // ファイルを選択したら
        if (result == Debug::Dialog::DialogResult::Yes || result == Debug::Dialog::DialogResult::OK)
        {
            std::string path = filename;
            //_terrainController.lock()->GetTerrain().lock()->SaveDataMap(
            //    Graphics::Instance().GetDevice(),
            //    Graphics::Instance().GetDeviceContext(),
            //    ToWString(path).c_str());
        }
    }

    ImGui::Image(_copyMaterialMapFB->GetColorSRV(0).Get(), ImVec2(128.0f, 128.0f));
    ImGui::Image(_copyMaterialMapFB->GetColorSRV(1).Get(), ImVec2(128.0f, 128.0f));
}
// 書き込みテクスチャの読み込み
void TerrainDeformer::LoadTextureData(const std::wstring& baseColorPath, const std::wstring& normalPath)
{
	// ベースカラーのSRVを作成
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> baseColorSRV;
	GpuResourceManager::LoadTextureFromFile(
		Graphics::Instance().GetDevice(),
		baseColorPath.c_str(),
		baseColorSRV.ReleaseAndGetAddressOf(),
        nullptr);
	// 法線マップのSRVを作成
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normalSRV;
	GpuResourceManager::LoadTextureFromFile(
		Graphics::Instance().GetDevice(),
		normalPath.c_str(),
		normalSRV.ReleaseAndGetAddressOf(),
        nullptr);
	// テクスチャデータを追加
	_textureDatas.push_back({ baseColorPath, baseColorSRV, normalPath, normalSRV });
}
