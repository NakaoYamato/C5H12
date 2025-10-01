#include "LocusRenderer.h"

#include "../../Library/Scene/Scene.h"
#include "../../Library/Graphics/Graphics.h"
#include <imgui.h>

// 開始処理
void LocusRenderer::Start()
{
	auto device = Graphics::Instance().GetDevice();
	// ピクセルシェーダ読み込み
	_pixelShader.Load(device, "./Data/Shader/HLSL/PrimitiveRenderer/Locus/PlayerSwordLocusPS.cso");
	// 定数バッファ生成
	_constantBuffer.Create(device, sizeof(CBPrimitive));
	// ノイズテクスチャの読み込み
	GpuResourceManager::LoadTextureFromFile(
		device,
		L"./Data/Texture/Noise/Noise002.png",
		_colorSRV.ReleaseAndGetAddressOf(),
		nullptr);
	// 距離テクスチャの読み込み
	GpuResourceManager::LoadTextureFromFile(
		device,
		L"./Data/Texture/Noise/SwordTrail000.png",
		_parameterSRV.ReleaseAndGetAddressOf(),
		nullptr);
}

// 更新処理
void LocusRenderer::Update(float elapsedTime)
{
	if (_trailPositions.empty())
		return;

	// 頂点をずらすかどうか
    bool isShift = false;

	// 各頂点の更新
	for (auto& pos : _trailPositions)
	{
		// 追加した頂点が消える場合はずらす
		pos.lifeTime -= elapsedTime;
		if (pos.lifeTime <= 0.0f)
			isShift = true;
        // 速度更新
        //const Vector3 movement = pos.velocity * elapsedTime;
        //pos.rootPosition += movement;
        //pos.tipPosition += movement;
        //// 軌跡の外側に広げる
        //const Vector3 vec = (pos.tipPosition - pos.rootPosition).Normalize() * elapsedTime;
        //pos.rootPosition += vec * 2.0f;
        //pos.tipPosition += vec;
	}

    // ずらす必要がない場合は終了
    if (!isShift) return;
    // 保存していた頂点バッファを1つずらす
    //_trailPositions.erase(_trailPositions.begin());
}
// GUI描画
void LocusRenderer::DrawGui()
{
    ImGui::Checkbox(u8"スプライン補間を使用するか", &_splineInterpolation);
    ImGui::SliderFloat("Catmull-Rom", &_catmullRom, 1.0f, 10.0f, "%.1f");
	ImGui::ColorEdit4("rootColor", &_rootColor.x);
	ImGui::ColorEdit4("tipColor", &_tipColor.x);
}
// 描画処理
void LocusRenderer::Render(const RenderContext& rc)
{
    auto& primitiveRenderer = GetActor()->GetScene()->GetPrimitiveRenderer();

	PrimitiveRenderer::RenderInfo primitiveInfo{};
	primitiveInfo.colorSRV = _colorSRV.GetAddressOf();
	primitiveInfo.parameterSRV = _parameterSRV.GetAddressOf();
	primitiveInfo.pixelShader = &_pixelShader;
	primitiveInfo.constantBuffer = &_constantBuffer;
	if (_splineInterpolation)
	{
		// スプライン補間を使用する場合
		int numPoints = static_cast<int>(_trailPositions.size());
		for (int i = 0; i < numPoints; ++i)
		{
			primitiveInfo.vertices.push_back({ _trailPositions[i].rootPosition, _rootColor });
			primitiveInfo.vertices.push_back({ _trailPositions[i].tipPosition, _tipColor });

			// 最初と最後2つでなければ処理
			if (0 < i && i < numPoints - 2)
			{
				for (float t = 1.0f; t <= _catmullRom; ++t)
				{
					// root
					DirectX::XMFLOAT3 lerpPos{};
					DirectX::XMStoreFloat3(&lerpPos,
						DirectX::XMVectorCatmullRom(
							DirectX::XMLoadFloat3(&_trailPositions[static_cast<size_t>(i - 1)].rootPosition),
							DirectX::XMLoadFloat3(&_trailPositions[static_cast<size_t>(i + 0)].rootPosition),
							DirectX::XMLoadFloat3(&_trailPositions[static_cast<size_t>(i + 1)].rootPosition),
							DirectX::XMLoadFloat3(&_trailPositions[static_cast<size_t>(i + 2)].rootPosition),
							t / _catmullRom));
					primitiveInfo.vertices.push_back({ lerpPos, _rootColor });

					// tip
					DirectX::XMStoreFloat3(&lerpPos,
						DirectX::XMVectorCatmullRom(
							DirectX::XMLoadFloat3(&_trailPositions[static_cast<size_t>(i - 1)].tipPosition),
							DirectX::XMLoadFloat3(&_trailPositions[static_cast<size_t>(i + 0)].tipPosition),
							DirectX::XMLoadFloat3(&_trailPositions[static_cast<size_t>(i + 1)].tipPosition),
							DirectX::XMLoadFloat3(&_trailPositions[static_cast<size_t>(i + 2)].tipPosition),
							t / _catmullRom));
					primitiveInfo.vertices.push_back({ lerpPos, _tipColor });
				}
			}
        }
	}
	else
	{
		for (auto& pos : _trailPositions)
		{
			primitiveInfo.vertices.push_back({ pos.rootPosition, _rootColor });
			primitiveInfo.vertices.push_back({ pos.tipPosition, _tipColor });
		}
	}
	// 定数バッファ更新
	CBPrimitive cb{};
	cb.vertexCount = static_cast<UINT>(primitiveInfo.vertices.size());
	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	rc.deviceContext->RSGetViewports(&numViewports, &viewport);
	cb.viewportSize = Vector2(
		static_cast<float>(viewport.Width),
		static_cast<float>(viewport.Height));
	_constantBuffer.Update(rc.deviceContext, &cb);

	GetActor()->GetScene()->GetPrimitiveRenderer().Draw(primitiveInfo);
}
// 軌跡追加
void LocusRenderer::PushFrontVertex(const Vector3& rootWP, 
	const Vector3& tipWP,
	const Vector3& velocity,
	float lifeTime)
{
	auto& locusPoint = _trailPositions.emplace_back();
	locusPoint.rootPosition = rootWP;
	locusPoint.tipPosition = tipWP;
	locusPoint.velocity = velocity;
    locusPoint.lifeTime = lifeTime;
}
