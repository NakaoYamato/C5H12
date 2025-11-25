#pragma once

#include "../../Library/2D/Sprite.h"
#include "../../Library/Input/Input.h"

class InputUI
{
public:
	struct SprData
	{
		Vector2 texPos{};
		Vector2 activateTexPos{};
		Vector2 texSize{};
	};

	struct DrawInfo
	{
		int keyboardKey		= 0;
		int gamePadKey		= 0;

		Vector2 position{};
		Vector2 scale{};
		Vector4 color{};
		bool isActive = false;
	};

public:
	InputUI() = default;
	~InputUI() = default;

	// èâä˙âª
	void Initialize();
	// ï`âÊ
	void Render(const RenderContext& rc, TextureRenderer& renderer);
	// GUIï`âÊ
	void DrawGui();

	// ï`âÊìoò^
	void Draw(DrawInfo drawinfo);

private:
	const float SpriteWidth = 128.0f;
	Sprite _gamePadSprite;
	Sprite _keybordSprite;

	std::vector<DrawInfo> _drawInfos;
	std::vector<DrawInfo> _drawValueInfos;

	std::unordered_map<int, SprData> _gamePadSprData;
	std::unordered_map<int, SprData> _keybordSprData;

	bool _showGui = false;
};