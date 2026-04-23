#include "ObstacleController.h"

#include "../../Library/Graphics/Graphics.h"
#include "../Scene/Scene.h"

#include <imgui.h>

// 遅延更新処理
void ObstacleController::LateUpdate(float elapsedTime)
{
	// 定数バッファマネージャーに登録
	if (ConstantBufferManager* cbManager = Graphics::Instance().GetConstantBufferManager())
	{
		auto& objectCB = cbManager->GetObjectCB();
		for (auto& obj : objectCB.objects)
		{
			if (obj.radius <= 0.0f)
			{
				obj.position = GetActor()->GetTransform().GetPosition() + _offset;
				obj.radius = _radius;
				break;
			}
		}
	}
}

// デバッグ表示
void ObstacleController::DebugRender(const RenderContext& rc)
{
	Debug::Renderer::DrawSphere(
		GetActor()->GetTransform().GetPosition() + _offset,
		_radius,
		Vector4::Red);
}

// GUI描画
void ObstacleController::DrawGui()
{
	ImGui::InputFloat3("Offset", &_offset.x);
	ImGui::InputFloat("Radius", &_radius);
}
