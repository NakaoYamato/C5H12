#include "TerrainDeformer.h"

#include "../../Library/Input/Input.h"
#include "../../Library/Collision/CollisionMath.h"
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

    // 地形のハイトマップを格納するフレームバッファを作成
    _parameterMapFB = std::make_unique<FrameBuffer>(
        Graphics::Instance().GetDevice(),
        Terrain::ParameterMapSize, Terrain::ParameterMapSize, true);
	// 地形のデータマップを格納するフレームバッファを作成
	_dataMapFB = std::make_unique<FrameBuffer>(
		Graphics::Instance().GetDevice(),
		Terrain::ParameterMapSize, Terrain::ParameterMapSize, true);
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
#ifdef USE_IMGUI
	//	ウィンドウにフォーカス中の場合は何もしない
	if (ImGui::IsAnyItemFocused() || ImGui::IsAnyItemHovered())
		return;
#endif
	auto terrain = _terrainController.lock()->GetTerrain().lock();
    if (!terrain)
        return;
    auto& streamOutData = terrain->GetStreamOutData();
    if (streamOutData.empty())
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

    // 交差したかどうか
    bool isIntersect = false;
	// UV座標を格納する変数
	Vector2 intersectUVPosition = Vector2::Zero;

    DirectX::XMVECTOR RayStart = DirectX::XMLoadFloat3(&rayStart);
    DirectX::XMVECTOR RayDir = DirectX::XMLoadFloat3(&rayDir);
    float rayLength = 1000.0f; // レイの長さ
    // 地形との交差判定
    for (size_t i = 0; i < streamOutData.size(); i += 3)
    {
        const auto& v1 = streamOutData[i + 0];
        const auto& v2 = streamOutData[i + 1];
        const auto& v3 = streamOutData[i + 2];
        DirectX::XMVECTOR triangleVerts[3] = {
            DirectX::XMLoadFloat3(&v1.worldPosition),
            DirectX::XMLoadFloat3(&v2.worldPosition),
            DirectX::XMLoadFloat3(&v3.worldPosition)
        };
        HitResult hitResult;
        if (Collision3D::IntersectRayVsTriangle(
            RayStart, RayDir, rayLength,
            triangleVerts,
            hitResult))
        {
            // 交差点を記録
            _intersectionWorldPoint = hitResult.position;
            Vector3 uv = _intersectionWorldPoint.TransformCoord(
                GetActor()->GetTransform().GetMatrixInverse());
            intersectUVPosition.x = (uv.x + 1.0f) / 2.0f;
            intersectUVPosition.y = (-uv.z + 1.0f) / 2.0f;
            isIntersect = true;
            break;
        }
    }

	// 交差していて、かつブラシの入力が押されている場合は変形タスクを追加
    if (isIntersect && _INPUT_PRESSED("OK"))
    {
		Task task;
		task.mode = _brushMode;
		task.brushUVPosition = intersectUVPosition;
        // ブラシ半径をトランスフォームのサイズに影響されるようにする
		task.radius = brushRadius / GetActor()->GetTransform().GetScale().x;
        // ブラシの強度は経過時間によって減衰させる
		task.strength = brushStrength * elapsedTime;
		task.brushColor = _brushColor;
		task.heightScale = _brushHeightScale;
		AddTask(task);
    }

    // ブラシの表示
    Debug::Renderer::DrawSphere(
        _intersectionWorldPoint, brushRadius,
        Vector4::GetOpaque(_brushColor));
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

    // 現在のパラメータマップフレームバッファの色情報を取得
    _parameterMapFB->ClearAndActivate(rc.deviceContext, Vector4::Zero, 1.0f);
    GetActor()->GetScene()->GetTextureRenderer().Blit(
        rc.deviceContext,
        terrain->GetParameterMapFB()->GetColorSRV().GetAddressOf(),
        0, 1
    );
    _parameterMapFB->Deactivate(rc.deviceContext);
	// 現在のデータマップフレームバッファの色情報を取得
	_dataMapFB->ClearAndActivate(rc.deviceContext, Vector4::Zero, 1.0f);
	GetActor()->GetScene()->GetTextureRenderer().Blit(
		rc.deviceContext,
		terrain->GetDataMapFB()->GetColorSRV().GetAddressOf(),
		0, 1
	);
	_dataMapFB->Deactivate(rc.deviceContext);

    // 定数バッファ設定
    ConstantBuffer constantBufferData{};
    rc.deviceContext->PSSetConstantBuffers(ModelCBIndex, 1, _constantBuffer.GetAddressOf());

    // Terrainのパラメータマップフレームバッファを編集
    for (auto& task : _tasks)
    {
        // ブラシのUV位置を設定
        constantBufferData.brushPosition = task.brushUVPosition;
        // ブラシの半径を設定
        constantBufferData.brushRadius = task.radius;
        // ブラシの強度を設定
        constantBufferData.brushStrength = task.strength;
        // ブラシの色を設定
        constantBufferData.brushColor = task.brushColor;
        // 高さ変形スケールを設定
        constantBufferData.heightScale = task.heightScale;

        // 定数バッファ更新
        rc.deviceContext->UpdateSubresource(_constantBuffer.Get(), 0, 0, &constantBufferData, 0, 0);

        // ブラシの種類によって処理を切り替える
        switch (_brushMode)
        {
        case BrushMode::Add:
            terrain->GetParameterMapFB()->Activate(rc.deviceContext);
            GetActor()->GetScene()->GetTextureRenderer().Blit(
                rc.deviceContext,
                _parameterMapFB->GetColorSRV().GetAddressOf(),
                0, 1,
                _addBrushPS.Get()
            );
            terrain->GetParameterMapFB()->Deactivate(rc.deviceContext);
            break;
        case BrushMode::Subtract:
            terrain->GetParameterMapFB()->Activate(rc.deviceContext);
            GetActor()->GetScene()->GetTextureRenderer().Blit(
                rc.deviceContext,
                _parameterMapFB->GetColorSRV().GetAddressOf(),
                0, 1,
                _subtractBrushPS.Get()
            );
            terrain->GetParameterMapFB()->Deactivate(rc.deviceContext);
            break;
        case BrushMode::Height:
            terrain->GetParameterMapFB()->Activate(rc.deviceContext);
            GetActor()->GetScene()->GetTextureRenderer().Blit(
                rc.deviceContext,
                _parameterMapFB->GetColorSRV().GetAddressOf(),
                0, 1,
                _heightBrushPS.Get()
            );
            terrain->GetParameterMapFB()->Deactivate(rc.deviceContext);
            break;
        case BrushMode::Cost:
            terrain->GetDataMapFB()->Activate(rc.deviceContext);
            GetActor()->GetScene()->GetTextureRenderer().Blit(
                rc.deviceContext,
                _dataMapFB->GetColorSRV().GetAddressOf(),
                0, 1,
                _costBrushPS.Get()
            );
            terrain->GetDataMapFB()->Deactivate(rc.deviceContext);
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

    ImGui::Checkbox(u8"ブラシ使用", &_useBrush);

	ImGui::Combo(u8"ブラシモード", reinterpret_cast<int*>(&_brushMode), u8"加算\0減算\0高さ変形\0コスト変形\0");

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
        ImGui::ColorEdit3(u8"ブラシ色", &_brushColor.x);
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
            _terrainController.lock()->GetTerrain().lock()->SaveDataMap(
                Graphics::Instance().GetDevice(),
                Graphics::Instance().GetDeviceContext(),
                ToWString(path).c_str());
        }
    }
}
