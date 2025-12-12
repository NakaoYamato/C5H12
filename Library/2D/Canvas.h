#pragma once

#include "../../Library/Math/Vector.h"
#include "../../Library/PostProcess/FrameBuffer.h"

#include <unordered_map>
#include <string>
#include <memory>

class Canvas
{
public:
	// テクスチャ情報
	struct TextureData
	{
		// テクスチャの位置
		Vector2				texPosition = Vector2::Zero;
		// テクスチャのサイズ
		Vector2				texSize		= Vector2::One;
		// テクスチャの分割数
		DirectX::XMUINT2	texSplit	= DirectX::XMUINT2();
	};

public:
	Canvas(ID3D11Device* device, 
		const DirectX::XMUINT2& canvasScale = DirectX::XMUINT2(8192, 8192),
		const Vector2& textureScale			= Vector2(128.0f, 128.0f));
	~Canvas() = default;

	/// <summary>
	/// テクスチャのロード
	/// </summary>
	/// <param name="device"></param>
	/// <param name="dc"></param>
	/// <param name="filename"></param>
	/// <param name="split"></param>
	/// <returns></returns>
	TextureData Load(ID3D11Device* device,
		ID3D11DeviceContext* dc,
		const wchar_t* filename,
		DirectX::XMUINT2 split);
	
	/// <summary>
	/// GUI描画
	/// </summary>
	void DrawGui(ID3D11Device* device,
		ID3D11DeviceContext* dc);

#pragma region アクセサ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetColorSRV() { return _canvasBuffer->GetColorSRV(); }
	std::string			GetFilePath() const { return _filepath; }
	DirectX::XMUINT2	GetCanvasScale() const { return _canvasScale; }
	Vector2				GetTextureScale() const { return _textureScale; }
	bool				IsUseOriginalTextureScale() const { return _isUseOriginalTextureScale; }

	void SetFilePath(const std::string& filepath) { _filepath = filepath; }
	void SetTextureScale(const Vector2& scale) { _textureScale = scale; }	
	void SetUseOriginalTextureScale(bool isUse) { _isUseOriginalTextureScale = isUse; }
#pragma endregion

#pragma region ファイル操作
	/// <summary>
	/// データ書き出し
	/// </summary>
	/// <param name="device"></param>
	/// <param name="dc"></param>
	/// <returns>失敗したらfalse</returns>
	bool Serialize(ID3D11Device* device,
		ID3D11DeviceContext* dc);

	/// <summary>
	/// データ読み込み
	/// </summary>
	/// <param name="device"></param>
	/// <param name="dc"></param>
	/// <returns>失敗したらfalse</returns>
	bool Deserialize(ID3D11Device* device,
		ID3D11DeviceContext* dc);
#pragma endregion
private:
	std::string _filepath = "./Data/Texture/CanvasData.png";

	// ロードしたテクスチャを描画するバッファ
	std::unique_ptr<FrameBuffer> _canvasBuffer;
	// ロードしたテクスチャ情報
	std::unordered_map<std::wstring, TextureData> _textureMap;
	// ラスタライザステート
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>	_rasterizerState;

	// キャンバスの最大スケール
	const UINT CanvasScaleMax = 16384;
	// キャンバスのスケール
	DirectX::XMUINT2 _canvasScale = DirectX::XMUINT2(8192, 8192);

	// テクスチャのスケール
	Vector2 _textureScale = Vector2(128.0f, 128.0f);
	// 元のテクスチャのスケールを使用するか
	bool _isUseOriginalTextureScale = false;

	// 次のテクスチャの描画位置
	Vector2 _nextTexPos = Vector2::Zero;
};