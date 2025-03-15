#include "RenderingManager.h"

#include "../ResourceManager/GpuResourceManager.h"

#include <imgui.h>
RenderingManager::RenderingManager(ID3D11Device* device,
    UINT width, UINT height)
{
	// ポストエフェクト用の管理者生成
	for (size_t index = 0; index < _countof(_frameBufferes); ++index)
	{
		_frameBufferes[index] = std::make_unique<FrameBuffer>(device, width, height);
	}
	_fullscreenQuad = std::make_unique<Sprite>(device,
		L"",
		".\\Data\\Shader\\FullscreenQuadVS.cso");
	(void)GpuResourceManager::CreatePsFromCso(device, "./Data/Shader/SpritePS.cso",
		_pixelShaders[static_cast<int>(FullscreenQuadPS::EmbeddedPS)].ReleaseAndGetAddressOf());
	(void)GpuResourceManager::CreatePsFromCso(device, "./Data/Shader/CascadedShadowMapPS.cso",
		_pixelShaders[static_cast<int>(FullscreenQuadPS::CascadedPS)].ReleaseAndGetAddressOf());
	(void)GpuResourceManager::CreatePsFromCso(device, "./Data/Shader/DeferredRenderingPS.cso",
		_pixelShaders[static_cast<int>(FullscreenQuadPS::DeferredRenderingPS)].ReleaseAndGetAddressOf());

	_cascadedShadowMap = std::make_unique<CascadedShadowMap>(device,
		1024 * 4, 1024 * 4);

	_gBuffer = std::make_unique<GBuffer>(device, width, height);
}

void RenderingManager::Blit(ID3D11DeviceContext* immediateContext,
	ID3D11ShaderResourceView** shaderResourceView,
	uint32_t startSlot,
	uint32_t numViews,
	FullscreenQuadPS shaderType)
{
	RenderingManager::Blit(immediateContext,
		shaderResourceView,
		startSlot, numViews,
		_pixelShaders[static_cast<int>(shaderType)].Get());
}

void RenderingManager::Blit(ID3D11DeviceContext* immediateContext, 
	ID3D11ShaderResourceView** shaderResourceView,
	uint32_t startSlot, 
	uint32_t numViews,
	ID3D11PixelShader* pixelShader)
{
	// 描画処理
	_fullscreenQuad->Blit(immediateContext,
		shaderResourceView,
		startSlot, numViews,
		pixelShader);
}

// ImGui描画
void RenderingManager::DrawGui()
{
#if USE_IMGUI
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu(u8"描画管理"))
		{
			ImGui::Checkbox(u8"デファードレンダリング", &_renderingDeferred);
			ImGui::Checkbox(u8"シャドウマップ", &_drawCSMGui);
			ImGui::Checkbox(u8"GBuffer", &_drawGBGui);

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
	// カスケードシャドウマップGUI
	if(_drawCSMGui)
		GetCascadedShadowMap()->DrawGui();

	// GBufferGUI
	if (_drawGBGui)
		GetGBuffer()->DrawGui();
#endif
}
