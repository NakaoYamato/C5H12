#pragma once

#include "../../Library/2D/Sprite.h"
#include "../../Library/Input/Input.h"

class InputUI
{
public:
	// スプライトデータ
	struct SprData
	{
		Vector2 texPos{};
		Vector2 activateTexPos{};
		Vector2 texSize{};
	};

	// 描画情報
	struct DrawInfo
	{
		int keyboardKey		= 0;
		int gamePadKey		= 0;

		Vector2 position	= Vector2::Zero;
		Vector2 scale		= Vector2::One;
		Vector4 color		= Vector4::White;
		bool isActive		= false;
	};

public:
	InputUI() = default;
	~InputUI() = default;

	// 初期化
	void Initialize();
	// 描画
	void Render(const RenderContext& rc, TextureRenderer& renderer);
	// GUI描画
	void DrawGui();

	// 描画登録
	void Draw(DrawInfo drawinfo);

private:
	// スプライトサイズ
	const float SpriteWidth = 128.0f;
	Sprite _gamePadSprite;
	Sprite _keybordSprite;

	std::vector<DrawInfo> _drawInfos;
	std::vector<DrawInfo> _drawValueInfos;

	std::unordered_map<int, SprData> _gamePadSprData;
	std::unordered_map<int, SprData> _keybordSprData;

	bool _showGui = false;
};