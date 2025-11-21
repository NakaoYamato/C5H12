#include "Canvas.h"

#include "../../Library/2D/SpriteResource.h"

#include "../../Library/Exporter/Exporter.h"
#include "../../Library/Algorithm/Converter.h"
#include "../HRTrace.h"

#include <imgui.h>

Canvas::Canvas(ID3D11Device* device, 
	const DirectX::XMUINT2& canvasScale,
	const Vector2& textureScale) :
	_canvasScale(canvasScale),
	_textureScale(textureScale)
{
	_canvasBuffer = std::make_unique<FrameBuffer>(device,
		_canvasScale.x, _canvasScale.y);

	// ベタ塗り＆カリングなし
	{
		D3D11_RASTERIZER_DESC desc{};
		desc.FrontCounterClockwise = TRUE;
		desc.DepthBias = 0;
		desc.DepthBiasClamp = 0;
		desc.SlopeScaledDepthBias = 0;
		desc.DepthClipEnable = TRUE;
		desc.ScissorEnable = FALSE;
		desc.MultisampleEnable = FALSE;
		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_NONE;
		desc.AntialiasedLineEnable = FALSE;
		HRESULT hr = device->CreateRasterizerState(&desc,
			_rasterizerState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}
}

/// テクスチャのロード
Canvas::TextureData Canvas::Load(ID3D11Device* device,
	ID3D11DeviceContext* dc,
	const wchar_t* filename, 
	DirectX::XMUINT2 split)
{
	// テクスチャがすでにロードされているか確認
	auto it = _textureMap.find(filename);
	if (it != _textureMap.end())
	{
		// すでにロードされている場合はそのまま返す
		return it->second;
	}
	// 現在使用しているステートのキャッシュを保存
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>  cachedRS;
	dc->RSGetState(cachedRS.GetAddressOf());

	dc->RSSetState(_rasterizerState.Get());
	// テクスチャのロード
	SpriteResource sprite(device, filename);

	// フレームバッファ開始
	if (_textureMap.empty())
	{
		// 最初のテクスチャなら透過処理
		_canvasBuffer->Clear(dc, Vector4::Zero);
	}
	_canvasBuffer->Activate(dc);

	// テクスチャ描画
	Vector2 scale = _isUseOriginalTextureScale ?
		Vector2::One :
		Vector2(
			_textureScale.x / sprite.GetTextureSize().x,
			_textureScale.y / sprite.GetTextureSize().y);
	sprite.Render(dc, _nextTexPos, scale);

	//　フレームバッファ停止
	_canvasBuffer->Deactivate(dc);

	TextureData textureData;
	// テクスチャの位置とサイズを設定
	textureData.texPosition = _nextTexPos;
	textureData.texSize = _isUseOriginalTextureScale ? sprite.GetTextureSize() : _textureScale;
	textureData.texSplit = split;

	// 次のテクスチャの位置を更新
	if (_isUseOriginalTextureScale)
	{
		_nextTexPos.y += textureData.texSize.y;
	}
	else
	{
		if (_nextTexPos.x + _textureScale.x >= static_cast<float>(_canvasScale.x))
		{
			_nextTexPos.x = 0.0f;
			_nextTexPos.y += sprite.GetTextureSize().y;
		}
		else
		{
			_nextTexPos.x += _textureScale.x;
		}
	}

	// ロードしたテクスチャ情報を保存
	_textureMap[filename] = textureData;

	// 保存してあるステートのキャッシュを復元
	dc->RSSetState(cachedRS.Get());

	return textureData;
}

/// GUI描画
void Canvas::DrawGui(ID3D11Device* device, ID3D11DeviceContext* dc)
{
	if (ImGui::TreeNode(u8"キャンバス"))
	{
		ImGui::Checkbox(u8"オリジナルテクスチャサイズを使用", &_isUseOriginalTextureScale);

		for (auto& [name, data] : _textureMap)
		{
			if (ImGui::TreeNode(ToString(name).c_str()))
			{
				ImGui::Text("Position: (%.1f, %.1f)", data.texPosition.x, data.texPosition.y);
				ImGui::Text("Size: (%.1f, %.1f)", data.texSize.x, data.texSize.y);
				ImGui::Text("Split: (%d, %d)", data.texSplit.x, data.texSplit.y);
				ImGui::TreePop();
			}
		}

		ImGui::InputText(u8"エクスポート先", &_filepath);
		if (ImGui::Button(u8"書き出し"))
		{
			Serialize(
				device,
				dc);
		}
		ImGui::DragFloat2(u8"次のテクスチャ位置", &_nextTexPos.x, 1.0f, 0.0f, static_cast<float>(_canvasScale.x));

		static float textureSize = 512.0f;
		float aspectRatio = static_cast<float>(_canvasScale.x) / static_cast<float>(_canvasScale.y);
		ImGui::DragFloat("TextureSize", &textureSize);
		ImGui::Image(_canvasBuffer->GetColorSRV().Get(),
			{ textureSize ,textureSize / aspectRatio });

		ImGui::TreePop();
	}
}

/// データ書き出し
bool Canvas::Serialize(ID3D11Device* device,
	ID3D11DeviceContext* dc)
{
	return Exporter::SavePngFile(
		device,
		dc,
		_canvasBuffer->GetColorSRV().Get(),
		ToWString(_filepath).c_str());
}

/// データ読み込み
bool Canvas::Deserialize(ID3D11Device* device,
	ID3D11DeviceContext* dc)
{
	SpriteResource sprite(device, ToWString(_filepath).c_str());

	// 現在使用しているステートのキャッシュを保存
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>  cachedRS;
	dc->RSGetState(cachedRS.GetAddressOf());

	dc->RSSetState(_rasterizerState.Get());
	// フレームバッファ開始
	_canvasBuffer->Clear(dc, Vector4::Zero);
	_canvasBuffer->Activate(dc);
	// テクスチャ描画
	sprite.Render(dc, Vector2::Zero);
	//　フレームバッファ停止
	_canvasBuffer->Deactivate(dc);
	// 保存してあるステートのキャッシュを復元
	dc->RSSetState(cachedRS.Get());

	return true;
}
