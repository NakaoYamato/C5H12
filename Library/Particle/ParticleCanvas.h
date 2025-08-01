#pragma once

#include "../../Library/Math/Vector.h"
#include "../../Library/PostProcess/FrameBuffer.h"

#include <unordered_map>
#include <string>
#include <memory>

class ParticleCanvas
{
public:
	// 16384が最大
	static constexpr UINT CanvasWidth	= 8192;  // テクスチャの幅
	static constexpr UINT CanvasHeight	= 8192;  // テクスチャの高さ

	struct TextureData
	{
		Vector2				texPosition	= Vector2::Zero; // テクスチャの位置
		Vector2				texSize		= Vector2::One;      // テクスチャのサイズ
		DirectX::XMUINT2	texSplit	= DirectX::XMUINT2(); // テクスチャの分割数
	};

public:
	ParticleCanvas();
	~ParticleCanvas() = default;

	/// <summary>
	/// テクスチャのロード
	/// </summary>
	/// <param name="dc"></param>
	/// <param name="filename"></param>
	/// <param name="split"></param>
	/// <returns></returns>
	TextureData Load(ID3D11DeviceContext* dc, const wchar_t* filename, DirectX::XMUINT2	split);
	/// <summary>
	/// GUI描画
	/// </summary>
	void DrawGui();

#pragma region アクセサ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetColorSRV() { return _canvasBuffer->GetColorSRV(); }
#pragma endregion

private:
	// ロードしたテクスチャを描画するバッファ
	std::unique_ptr<FrameBuffer> _canvasBuffer;

	// ロードしたテクスチャ情報
	std::unordered_map<std::wstring, TextureData> _textureMap;

	// 次のテクスチャの描画位置
	Vector2 _nextTexPos = Vector2::Zero;

	// エクスポート先パス
	std::string _exportPath = "./Data/Texture/Particle/ParticleExport.png";
};