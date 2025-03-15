#include "PointLightController.h"

// 更新処理
void PointLightController::Update(float elapsedTime)
{
    Vector4 pos{};
    pos.x = GetActor()->GetTransform().GetWorldPosition().x;
    pos.y = GetActor()->GetTransform().GetWorldPosition().y;
    pos.z = GetActor()->GetTransform().GetWorldPosition().z;
    _light.SetPosition(pos);
}

// 描画の前処理
void PointLightController::RenderPreprocess(RenderContext& rc)
{
    // rcの設定
    rc.pointLights.push_back(_light.GetDataPointer());
}

// デバッグ表示
void PointLightController::DebugRender(const RenderContext& rc)
{
    _light.DebugRender();
}

// GUI描画
void PointLightController::DrawGui()
{
    _light.DrawGui();
}
