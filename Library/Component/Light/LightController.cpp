#include "LightController.h"

// 描画処理
void LightController::RenderPreprocess(RenderContext& rc)
{
    // rcの設定
    rc.lightDirection = _light.GetDirection();
    rc.lightColor = _light.GetColor();
    rc.lightAmbientColor = _light.GetAmbientColor();
}

// デバッグ表示
void LightController::DebugRender(const RenderContext& rc)
{
    _light.DebugRender();
}

// GUI描画
void LightController::DrawGui()
{
    _light.DrawGui();
}
