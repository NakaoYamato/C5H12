#include "PointLightController.h"

#include "../../Library/Scene/Scene.h"
#include "../../DebugSupporter/DebugSupporter.h"

#include <imgui.h>

std::mutex PointLightController::_renderContextMutex;

// 更新処理
void PointLightController::Update(float elapsedTime)
{
    Vector4 pos{};
    pos.x = GetActor()->GetTransform().GetWorldPosition().x;
    pos.y = GetActor()->GetTransform().GetWorldPosition().y;
    pos.z = GetActor()->GetTransform().GetWorldPosition().z;
	_light.position = pos;
    {
		std::lock_guard<std::mutex> lock(_renderContextMutex);
        GetActor()->GetScene()->GetRenderContext().pointLights.push_back(&_light);
    }
}

// デバッグ表示
void PointLightController::DebugRender(const RenderContext& rc)
{
    Debug::Renderer::DrawSphere(_light.position.Vec3(), _light.range, _light.color);
}

// GUI描画
void PointLightController::DrawGui()
{
    bool useFlag = _light.isAlive == 1;
    ImGui::Checkbox(u8"使用フラグ", &useFlag);
    _light.isAlive = useFlag == true ? 1 : 0;
    ImGui::ColorEdit4("color", &_light.color.x);
    ImGui::DragFloat("range", &_light.range, 0.1f);
}
