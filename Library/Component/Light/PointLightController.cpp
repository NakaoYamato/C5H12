#include "PointLightController.h"

// 更新処理
void PointLightController::Update(float elapsedTime)
{
    Vector4 pos{};
    pos.x = GetActor()->GetTransform().GetWorldPosition().x;
    pos.y = GetActor()->GetTransform().GetWorldPosition().y;
    pos.z = GetActor()->GetTransform().GetWorldPosition().z;
    light_.SetPosition(pos);
}

// 描画の前処理
void PointLightController::RenderPreprocess(RenderContext& rc)
{
    // rcの設定
    rc.pointLights.push_back(light_.GetDataPointer());
}

// デバッグ表示
void PointLightController::DebugRender(const RenderContext& rc)
{
    light_.DebugRender();
}

// GUI描画
void PointLightController::DrawGui()
{
    light_.DrawGui();
}
