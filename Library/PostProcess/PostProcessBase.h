#pragma once

#include <wrl.h>
#include <memory>
#include <unordered_map>
#include <string>

#include "FrameBuffer.h"
#include "../2D/SpriteResource.h"

// ポストエフェクト用基底クラス
class PostProcessBase
{
public:
	// ピクセルシェーダ、定数バッファの生成
	// width	: スクリーンの横幅
	// height	: スクリーンの縦幅
	// psName	: ピクセルシェーダのファイルパス
	// bufferSize	: 定数バッファの大きさ
	PostProcessBase(ID3D11Device* device,
		uint32_t width, uint32_t height,
		const char* psName, UINT bufferSize);
	virtual ~PostProcessBase() {}

	// 更新処理
	virtual void Update(float elapsedTime) {}

	// 描画処理
	virtual void Render(ID3D11DeviceContext* immediateContext,
		ID3D11ShaderResourceView** shaderResourceView,
		uint32_t startSlot, uint32_t numViews);

	// デバッグGui描画
	virtual void DrawGui() {}

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetColorSRV() {
		return _frameBuffer->GetColorSRV();
	}

	// フレームバッファの深度値情報を取得
	ID3D11DepthStencilView* GetDepthStencilView() {
		return _frameBuffer->GetDSV().Get();
	}

	// 初期のデータの取得
	const std::unordered_map<std::string, float>& GetStartData()const { return _startData; }
	// 現在のデータの取得
	virtual std::unordered_map<std::string, float> GetCurrentData() { return std::unordered_map<std::string, float>(); };
	// データの設定
	virtual void SetData(std::unordered_map<std::string, float>& parameter) {};
	// データの初期化
	virtual void ClearData() { SetData(_startData); };

protected:
	// 定数バッファの更新
	virtual void UpdateConstantBuffer(ID3D11DeviceContext* immediateContext, ID3D11Buffer* constantBuffer) = 0;

protected:
	std::unique_ptr<FrameBuffer> _frameBuffer;
	std::unique_ptr<SpriteResource> _fullscreenQuad;
	Microsoft::WRL::ComPtr<ID3D11Buffer> _constantBuffer;

	std::unordered_map<std::string, float> _startData;
};