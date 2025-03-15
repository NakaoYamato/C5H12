#include "PostProcessBase.h"
#include "../../ResourceManager/GpuResourceManager.h"

PostProcessBase::PostProcessBase(ID3D11Device* device,
	uint32_t width, uint32_t height,
	const char* psName, UINT bufferSize)
{
	_frameBuffer = std::make_unique<FrameBuffer>(device,
		width, height);
	_fullscreenQuad = std::make_unique<Sprite>(device,
		L"",
		".\\Data\\Shader\\FullscreenQuadVS.cso",
		psName);

	// 定数バッファ作成
	(void)GpuResourceManager::CreateConstantBuffer(device,
		bufferSize,
		_constantBuffer.ReleaseAndGetAddressOf());
}

// 描画処理
void PostProcessBase::Render(ID3D11DeviceContext* immediateContext,
	ID3D11ShaderResourceView** shaderResourceView,
	uint32_t startSlot, uint32_t numViews)
{
	// フレームバッファ開始
	_frameBuffer->ClearAndActivate(immediateContext);

	// 定数バッファの更新
	UpdateConstantBuffer(immediateContext, _constantBuffer.Get());
	immediateContext->PSSetConstantBuffers(1, 1, _constantBuffer.GetAddressOf());

	// 描画処理
	_fullscreenQuad->Blit(immediateContext,
		shaderResourceView,
		startSlot, numViews);

	//　フレームバッファ停止
	_frameBuffer->Deactivate(immediateContext);
}
