#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl.h>

#include "../Math/Vector.h"

class Sprite
{
public:
	/// <summary>
	/// コンストラクタ（テクスチャ生成）
	/// </summary>
	/// <param name="device"></param>
	/// <param name="filename">テクスチャのパス</param>
	/// <param name="vsShaderFilename"></param>
	/// <param name="psShaderFilename"></param>
	Sprite(ID3D11Device* device, 
		const wchar_t* filename,
		const char* vsShaderFilename = ".\\Data\\Shader\\SpriteVS.cso",
		const char* psShaderFilename = ".\\Data\\Shader\\SpritePS.cso");
	/// <summary>
	/// コンストラクタ（SRVを直接受け取る）
	/// </summary>
	/// <param name="device"></param>
	/// <param name="srv">受け取るSRV</param>
	/// <param name="vsShaderFilename"></param>
	/// <param name="psShaderFilename"></param>
	Sprite(ID3D11Device* device,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv,
		const char* vsShaderFilename = ".\\Data\\Shader\\SpriteVS.cso",
		const char* psShaderFilename = ".\\Data\\Shader\\SpritePS.cso");

	Sprite() = delete;
	virtual ~Sprite();

	/// <summary>
	/// 描画処理
	/// </summary>
	/// <param name="immediateContext"></param>
	/// <param name="position">描画座標</param>
	/// <param name="scale">描画スケール</param>
	/// <param name="texPos">切り取り座標</param>
	/// <param name="texSize">切り取りサイズ((0.0f, 0.0f)でsrvの大きさで描画)</param>
	/// <param name="center">中心座標</param>
	/// <param name="angle">角度(ラジアン)</param>
	/// <param name="color">描画色</param>
	void Render(ID3D11DeviceContext* immediateContext,
		const Vector2& position,
		const Vector2& scale = Vector2(1.0f, 1.0f),
		const Vector2& texPos = Vector2(0.0f, 0.0f),
		const Vector2& texSize = Vector2(0.0f, 0.0f),
		const Vector2& center = Vector2(0.0f, 0.0f), 
		float angle = 0.0f,
		const Vector4& color = Vector4(1.0f, 1.0f, 1.0f, 1.0f)) const;

	/// <summary>
	/// FullscreenQuadの代わり
	/// </summary>
	/// <param name="immediateContext"></param>
	/// <param name="shaderResourceView">srv</param>
	/// <param name="startSlot">srvをセットする位置</param>
	/// <param name="numViews"></param>
	void Blit(ID3D11DeviceContext* immediateContext,
		ID3D11ShaderResourceView** shaderResourceView,
		uint32_t startSlot, uint32_t numViews,
		ID3D11PixelShader* pixelShader = nullptr);

	/// <summary>
	/// SRVの大きさ取得
	/// </summary>
	/// <returns></returns>
	const Vector2 GetTextureSize()
	{
		return Vector2(static_cast<float>(_texture2dDesc.Width), static_cast<float>(_texture2dDesc.Height));
	}
private:
	bool _isLoadFile = true;

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> _vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> _pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> _inputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> _vertexBuffer;

	struct Vertex
	{
		Vector3 position{};
		Vector4 color{};
		Vector2 texcoord{};
	};

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _srv;
	D3D11_TEXTURE2D_DESC _texture2dDesc{};
};