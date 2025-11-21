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
		std::string actionName = "";
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
	void Draw(const std::string& actionName,
		bool isActive,
		const Vector2& position, 
		const Vector2& scale = Vector2::One,
		const Vector4& color = Vector4::White);

private:
	const float SpriteWidth = 128.0f;
	Sprite _gamePadSprite;
	Sprite _keybordSprite;

	std::vector<DrawInfo> _drawInfos;

	std::unordered_map<int, SprData> _gamePadSprData;
	std::unordered_map<int, SprData> _keybordSprData;

	bool _showGui = false;
};