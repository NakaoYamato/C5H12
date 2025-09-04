#include "SpriteRenderer.h"

#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Collision/CollisionMath.h"

#include <imgui.h>

// 3D•`‰æŒã‚Ì•`‰æˆ—
void SpriteRenderer::DelayedRender(const RenderContext& rc)
{
	for (auto& [name, spriteData] : _sprites)
	{
		SpriteRender(name, rc, Vector2::Zero, Vector2(1.0f, 1.0f));
	}
}
// GUI•`‰æ
void SpriteRenderer::DrawGui()
{
	for (auto& [name, spriteData] : _sprites)
	{
		if (ImGui::TreeNode(name.c_str()))
		{
			spriteData.DrawGui();
			ImGui::TreePop();
		}
	}
}
// ‰æ‘œ“Ç‚İ‚İ
void SpriteRenderer::LoadTexture(const std::string& spriteName,
	const wchar_t* filename,
	Sprite::CenterAlignment alignment)
{
	_sprites[spriteName].LoadTexture(filename, alignment);
}
// ‰æ‘œ‚Æ‚Ì“–‚½‚è”»’è
bool SpriteRenderer::IsHit(const std::string& name, const Vector2& pos) const
{
    if (_sprites.find(name) != _sprites.end())
    {
        const auto& spriteData = _sprites.at(name);
		return spriteData.IsHit(pos);
    }
	return false;
}
// ƒXƒvƒ‰ƒCƒg•`‰æ
void SpriteRenderer::SpriteRender(const std::string& spriteName, 
	const RenderContext& rc, 
	const Vector2& offset,
	const Vector2& offsetScale)
{
	_sprites[spriteName].Render(rc,
		offset,
		offsetScale);
}
