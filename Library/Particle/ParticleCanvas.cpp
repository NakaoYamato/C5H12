#include "ParticleCanvas.h"

#include "../../Library/Graphics/Graphics.h"
#include "../../Library/2D/SpriteResource.h"

#include "../../Library/Exporter/Exporter.h"
#include "../../Library/Algorithm/Converter.h"
#include <imgui.h>

ParticleCanvas::ParticleCanvas()
{
	_canvasBuffer = std::make_unique<FrameBuffer>(Graphics::Instance().GetDevice(),
		CanvasWidth, CanvasHeight);
}

ParticleCanvas::TextureData ParticleCanvas::Load(const wchar_t* filename, DirectX::XMUINT2	split)
{
	// テクスチャがすでにロードされているか確認
	auto it = _textureMap.find(filename);
	if (it != _textureMap.end())
	{
		// すでにロードされている場合はそのまま返す
		return it->second;
	}
	// テクスチャのロード
	SpriteResource sprite(Graphics::Instance().GetDevice(), filename);

	{
		// スレッドセーフ
		std::lock_guard<std::mutex> lock(Graphics::Instance().GetMutex());

		// フレームバッファ開始
		if (_textureMap.empty())
		{
			// 最初のテクスチャなら透過処理
			_canvasBuffer->Clear(Graphics::Instance().GetDeviceContext(), Vector4::Zero);
		}
		_canvasBuffer->Activate(Graphics::Instance().GetDeviceContext());

		// テクスチャ描画
		sprite.Render(Graphics::Instance().GetDeviceContext(), _nextTexPos);

		//　フレームバッファ停止
		_canvasBuffer->Deactivate(Graphics::Instance().GetDeviceContext());
	}

	TextureData textureData;
	// テクスチャの位置とサイズを設定
	textureData.texPosition = _nextTexPos;
	textureData.texSize = sprite.GetTextureSize();
	textureData.texSplit = split;

	// 次のテクスチャの位置を更新
	_nextTexPos.y += textureData.texSize.y;

	// ロードしたテクスチャ情報を保存
	_textureMap[filename] = textureData;

	return textureData;
}

/// GUI描画
void ParticleCanvas::DrawGui()
{
	if (ImGui::TreeNode(u8"キャンバス"))
	{
		ImGui::InputText(u8"エクスポート先", &_exportPath);
		if (ImGui::Button(u8"書き出し"))
		{
			Exporter::SavePngFile(
				Graphics::Instance().GetDevice(),
				Graphics::Instance().GetDeviceContext(),
				_canvasBuffer->GetColorSRV().Get(),
				ToWString(_exportPath));
		}
		static float textureSize = 512.0f;
		ImGui::DragFloat("TextureSize", &textureSize);
		ImGui::Image(_canvasBuffer->GetColorSRV().Get(),
			{ textureSize ,textureSize });

		ImGui::TreePop();
	}
}
