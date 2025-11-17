#pragma once

#include "../../External/DirectXTK-main/SpriteBatch.h"
#include "../../External/DirectXTK-main/SpriteFont.h"

#include "../../Library/Math/Vector.h"

#include <memory>
#include <vector>
#include <string>

enum class FontType
{
	MSGothic,		

	FontTypeMax,
};

/// <summary>
/// テキスト描画
/// </summary>
class TextRenderer
{
public:
	struct TextDrawData
	{
		FontType		type		= FontType::MSGothic;	// フォントの種類
		std::wstring	text		= L"";					// 文字列
		Vector2			position	= Vector2::One;			// 描画位置（スクリーン座標）
		Vector4			color		= Vector4::White;		// 色
		float			rotation	= 0.0f;					// 回転量
		Vector2			origin		= Vector2::Zero;		// 原点位置
		Vector2			scale		= Vector2::One;			// スケール
	};
	struct Text3DDrawData
	{
		FontType		type		= FontType::MSGothic;	// フォントの種類
		std::string		text		= "";					// 文字列
		Vector3			position	= Vector3::Zero;		// 描画位置（ワールド座標）
		Vector4			color		= Vector4::White;		// 色
		float			rotation	= 0.0f;					// 回転量
		Vector2			origin		= Vector2::Zero;		// 原点位置
		Vector2			scale		= Vector2::One;			// スケール
	};

public:
	TextRenderer() {}
	~TextRenderer() = default;

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="device"></param>
	/// <param name="dc"></param>
	void Initialize(ID3D11Device* device, ID3D11DeviceContext* dc);
	/// <summary>
	/// テキスト描画
	/// </summary>
	/// <param name="type">フォントの種類</param>
	/// <param name="text">文字列</param>
	/// <param name="position">描画位置（スクリーン座標）</param>
	/// <param name="color">色</param>
	/// <param name="rotation">回転量</param>
	/// <param name="origin"></param>
	/// <param name="scale"></param>
	void Draw(FontType type,
		const wchar_t* text,
		const Vector2& position,
		const Vector4& color	= Vector4::White,
		float rotation			= 0.0f, 
		const Vector2& origin	= Vector2::Zero,
		const Vector2& scale	= Vector2::One);
	/// <summary>
	/// テキスト描画
	/// </summary>
	/// <param name="type">フォントの種類</param>
	/// <param name="text">文字列</param>
	/// <param name="position">描画位置（スクリーン座標）</param>
	/// <param name="color">色</param>
	/// <param name="rotation">回転量</param>
	/// <param name="origin">0~1</param>
	/// <param name="scale"></param>
	void Draw(FontType type,
		const char* text,
		const Vector2& position,
		const Vector4& color	= Vector4::White,
		float rotation			= 0.0f,
		const Vector2& origin	= Vector2::Zero,
		const Vector2& scale	= Vector2::One);
	/// <summary>
	/// 3Dテキスト描画
	/// </summary>
	/// <param name="type"></param>
	/// <param name="text"></param>
	/// <param name="position"></param>
	/// <param name="color"></param>
	/// <param name="rotation"></param>
	/// <param name="origin">0~1</param>
	/// <param name="scale"></param>
	void Draw3D(FontType type,
		const wchar_t* text,
		const Vector3& position,
		const Vector4& color	= Vector4::White,
		float rotation			= 0.0f,
		const Vector2& origin	= Vector2::Zero,
		const Vector2& scale	= Vector2::One);
	/// <summary>
	/// 3Dテキスト描画
	/// </summary>
	/// <param name="type"></param>
	/// <param name="text"></param>
	/// <param name="position"></param>
	/// <param name="color"></param>
	/// <param name="rotation"></param>
	/// <param name="origin"></param>
	/// <param name="scale"></param>
	void Draw3D(FontType type,
		const char* text,
		const Vector3& position,
		const Vector4& color	= Vector4::White,
		float rotation			= 0.0f,
		const Vector2& origin	= Vector2::Zero,
		const Vector2& scale	= Vector2::One);
	/// <summary>
	/// 描画実行
	/// </summary>
	void Render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection, 
		float viewportWidth, float viewportHeight);

private:
	std::unique_ptr<DirectX::SpriteFont>	_spriteFonts[static_cast<int>(FontType::FontTypeMax)];
	std::unique_ptr<DirectX::SpriteBatch>	_spriteBatch;
	// 描画するテキストデータ
	std::vector<TextDrawData> _textDrawDatas;
	// 描画する3Dテキストデータ
	std::vector<Text3DDrawData> _text3DDrawDatas;
	// 各フォントの大きさ
	float _fontScale[static_cast<int>(FontType::FontTypeMax)]{};
};