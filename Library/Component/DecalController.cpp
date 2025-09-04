#include "DecalController.h"

#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Scene/Scene.h"
#include "../../Library/DebugSupporter/DebugSupporter.h"

#include <imgui.h>

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
			_shaderName,
			_decal.get(),
			GetActor()->GetTransform().GetMatrix());
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
	static std::vector<const char*> ShaderNames;
	if (ShaderNames.empty())
	{
		ShaderNames = GetActor()->GetScene()->GetDecalRenderer().GetShaderNames();
	}

	if (_decal)
	{
		for (auto& shaderName : ShaderNames)
		{
			if (ImGui::RadioButton(shaderName, _shaderName == shaderName))
			{
				_shaderName = shaderName;
			}
		}

		ImGui::Separator();
		_decal->DrawGui(Graphics::Instance().GetDevice());
	}
}
