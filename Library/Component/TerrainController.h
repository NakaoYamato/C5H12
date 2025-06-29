#pragma once

#include "Component.h"
#include "../../Library/Terrain/Terrain.h"

class TerrainController : public Component
{
public:
    TerrainController() = default;
    ~TerrainController() override = default;
    // 名前取得
    const char* GetName() const override { return "TerrainController"; }
    // 開始処理
    void Start() override;
    // 更新処理
    void Update(float elapsedTime) override;
    // 描画処理
    void Render(const RenderContext& rc) override;
    // GUI描画
    void DrawGui() override;
    // 地形取得
    std::weak_ptr<Terrain> GetTerrain() const { return _terrain; }

private:
    std::shared_ptr<Terrain> _terrain = nullptr;
};