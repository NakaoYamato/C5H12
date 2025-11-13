#include "SpriteRenderer.h"

#include "../../Library/Algorithm/Converter.h"
#include "../../Library/Actor/UI/UIActor.h"
#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Collision/CollisionMath.h"
#include "../../Library/Scene/Scene.h"

#include <Mygui.h>

// 生成時処理
void SpriteRenderer::OnCreate()
{
	// 親がUIActorであればRectTransformを取得
	UIActor* uiActor = dynamic_cast<UIActor*>(GetActor().get());
	if (uiActor)
	{
		_myRectTransform = &uiActor->GetRectTransform();
	}
}

// 更新処理
void SpriteRenderer::Update(float elapsedTime)
{
	for (auto& [name, spriteData] : _sprites)
	{
		spriteData.UpdateTransform(_myRectTransform);
	}
}

// 3D描画後の描画処理
void SpriteRenderer::DelayedRender(const RenderContext& rc)
{
	// ステンシルをクリア
	rc.deviceContext->ClearDepthStencilView(rc.depthStencilView, D3D11_CLEAR_STENCIL, 1.0f, 0);

	for (const auto& spriteName : _spriteDrawOrder)
	{
		if (_sprites.find(spriteName) == _sprites.end())
            continue;
        auto& spriteData = _sprites.at(spriteName);

		SpriteRender(spriteName, rc);

		if (GetActor()->IsDrawingDebug() && !Debug::Input::IsActive(DebugInput::BTN_F7))
		{
			// 中心をデバッグ描画
			GetActor()->GetScene()->GetPrimitive()->Circle(rc.deviceContext,
				spriteData.GetRectTransform().GetWorldPosition(),
				5.0f);
		}
    }
}
// GUI描画
void SpriteRenderer::DrawGui()
{
	std::string resultPath = "";
	if (ImGui::OpenDialogBotton(u8"画像読み込み", &resultPath, ImGui::TextureFilter))
	{
		LoadTexture(resultPath.c_str(), ToWString(resultPath).c_str(), Sprite::CenterCenter);
	}

	for (auto& [name, spriteData] : _sprites)
	{
		if (ImGui::TreeNode(name.c_str()))
		{
			spriteData.DrawGui();
			ImGui::TreePop();
		}
	}
}
// 画像読み込み
void SpriteRenderer::LoadTexture(const std::string& spriteName,
	const wchar_t* filename,
	Sprite::CenterAlignment alignment)
{
	_sprites[spriteName].LoadTexture(filename, alignment);
    _spriteDrawOrder.push_back(spriteName);
}
// 画像との当たり判定
bool SpriteRenderer::IsHit(const std::string& name, const Vector2& pos) const
{
    if (_sprites.find(name) != _sprites.end())
    {
        const auto& spriteData = _sprites.at(name);
		return spriteData.IsHit(pos);
    }
	return false;
}
// スプライト描画
void SpriteRenderer::SpriteRender(const std::string& spriteName, 
	const RenderContext& rc)
{
	auto& textureRenderer = GetActor()->GetScene()->GetTextureRenderer();
	_sprites[spriteName].Render(rc, textureRenderer);
}
