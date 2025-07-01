#include "TerrainDeformer.h"

#include "../../Library/Input/Input.h"
#include "../../Library/Collision/CollisionMath.h"
#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Graphics/GpuResourceManager.h"
#include "../../Library/Scene/Scene.h"
#include "../../Library/DebugSupporter/DebugSupporter.h"

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

    // 定数バッファの作成
    GpuResourceManager::CreateConstantBuffer(
        Graphics::Instance().GetDevice(),
        sizeof(ConstantBuffer),
        _constantBuffer.ReleaseAndGetAddressOf());

    // 地形のハイトマップを格納するフレームバッファを作成
    _parameterMapFB = std::make_unique<FrameBuffer>(
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

    _isIntersect = false;
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
            _constantBufferData.brushPosition.x = (uv.x + 1.0f) / 2.0f;
            _constantBufferData.brushPosition.y = (-uv.z + 1.0f) / 2.0f;
            _isIntersect = true;
            break;
        }
    }

    // 変形中フラグをオンにする
    if (_isIntersect)
        _isDeforming = _INPUT_PRESSED("OK");

    // ブラシ半径をトランスフォームのサイズに影響されるようにする
    _constantBufferData.brushRadius = brushRadius;

    // ブラシの強度は経過時間によって減衰させる
    _constantBufferData.brushStrength = brushStrength * elapsedTime;

    // ブラシの表示
    Debug::Renderer::DrawSphere(
        _intersectionWorldPoint, _constantBufferData.brushRadius,
        Vector4::GetOpaque(_constantBufferData.brushColor));
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
    auto terrain = _terrainController.lock()->GetTerrain().lock();
    if (!terrain)
        return;
    // 変形中フラグがオフの場合は何もしない
    if (!_isDeforming)
        return;
    // 接触していなければ処理しない
    if (!_isIntersect)
        return;
    auto& streamOutData = terrain->GetStreamOutData();
    if (streamOutData.empty())
        return;
    // ブラシ半径が0以下の場合は何もしない
    if (_constantBufferData.brushRadius <= 0.0f)
        return;
    // ブラシ強度が0以下の場合は何もしない
    if (_constantBufferData.brushStrength <= 0.0f)
        return;

    // キャッシュの保存
    GpuResourceManager::SaveStateCache(rc.deviceContext);

    // 定数バッファ設定
    rc.deviceContext->UpdateSubresource(_constantBuffer.Get(), 0, 0, &_constantBufferData, 0, 0);
    rc.deviceContext->PSSetConstantBuffers(ModelCBIndex, 1, _constantBuffer.GetAddressOf());

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

    terrain->GetParameterMapFB()->Activate(rc.deviceContext);
    ID3D11ShaderResourceView* nullSRV[] = { nullptr };
	// ブラシの種類によってピクセルシェーダを切り替える
    switch (_brushMode)
    {
    case BrushMode::Add:
        GetActor()->GetScene()->GetTextureRenderer().Blit(
            rc.deviceContext,
            _parameterMapFB->GetColorSRV().GetAddressOf(),
            0, 1,
            _addBrushPS.Get()
        );
        break;
    case BrushMode::Subtract:
        GetActor()->GetScene()->GetTextureRenderer().Blit(
            rc.deviceContext,
            _parameterMapFB->GetColorSRV().GetAddressOf(),
            0, 1,
            _subtractBrushPS.Get()
        );
        break;
    case BrushMode::Height:
        GetActor()->GetScene()->GetTextureRenderer().Blit(
            rc.deviceContext,
            _parameterMapFB->GetColorSRV().GetAddressOf(),
            0, 1,
            _heightBrushPS.Get()
        );
        break;
    }
    terrain->GetParameterMapFB()->Deactivate(rc.deviceContext);
    terrain->SetStreamOut(true);

    // キャッシュの復元
    GpuResourceManager::RestoreStateCache(rc.deviceContext);
}
// GUI描画
void TerrainDeformer::DrawGui()
{
    ImGui::Checkbox(u8"ブラシ使用", &_useBrush);

	ImGui::Combo(u8"ブラシモード", reinterpret_cast<int*>(&_brushMode), u8"加算\0減算\0高さ変形\0");

    ImGui::DragFloat3(u8"ブラシワールド位置", &_intersectionWorldPoint.x, 0.01f, -100.0f, 100.0f);
    ImGui::DragFloat2(u8"ブラシUV位置", &_constantBufferData.brushPosition.x, 0.01f, -100.0f, 100.0f);
    ImGui::DragFloat(u8"ブラシ半径", &brushRadius, 0.01f, 0.0f, 100.0f);
    ImGui::DragFloat(u8"ブラシ強度", &brushStrength, 0.01f, -10.0f, 10.0f);
    ImGui::ColorEdit3(u8"ブラシ色", &_constantBufferData.brushColor.x);
	if (ImGui::Button(u8"パラメータマップ書き出し"))
	{
		if (_terrainController.lock()->GetTerrain().lock())
		{
            _terrainController.lock()->GetTerrain().lock()->SaveParameterMap(
				Graphics::Instance().GetDevice(),
				Graphics::Instance().GetDeviceContext(),
				L"./Data/Texture/Terrain/ParameterMap.dds");
		}
	}
}
