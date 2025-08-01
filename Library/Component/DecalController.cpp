#include "DecalController.h"

#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Scene/Scene.h"
#include "../../Library/DebugSupporter/DebugSupporter.h"

// 開始処理
void DecalController::Start()
{
	// デカールの初期化
	_decal = std::make_unique<Decal>(Graphics::Instance().GetDevice(), _colorTextureFilename.c_str(), _normalTextureFilename.c_str());
}
// 描画処理
void DecalController::Render(const RenderContext& rc)
{
	// デカールの描画
	if (_decal)
	{
		GetActor()->GetScene()->GetDecalRenderer().Draw(
			_decal.get(),
			GetActor()->GetTransform().GetMatrix(),
			_color);
	}
}
// デバッグ表示
void DecalController::DebugRender(const RenderContext& rc)
{
	Debug::Renderer::DrawHalfBox(GetActor()->GetTransform().GetMatrix(), Vector4::Green);
}
// GUI描画
void DecalController::DrawGui()
{
	if (_decal)
	{
		_decal->DrawGui(Graphics::Instance().GetDevice());
	}
}
