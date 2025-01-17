#include "LightController.h"

// 描画処理
void LightController::RenderPreprocess(RenderContext& rc)
{
    // rcの設定
    rc.lightDirection = light_.GetDirection();
    rc.lightColor = light_.GetColor();
    rc.lightAmbientColor = light_.GetAmbientColor();
}

// デバッグ表示
void LightController::DebugRender(const RenderContext& rc)
{
    light_.DebugRender();
}

// GUI描画
void LightController::DrawGui()
{
    light_.DrawGui();
}
