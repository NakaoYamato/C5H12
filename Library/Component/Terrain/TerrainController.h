#pragma once

#include "../Component.h"
#include "../../Library/Terrain/Terrain.h"

class TerrainController : public Component
{
public:
    TerrainController(const std::string& serializePath = "./Data/Terrain/Save/Test000.json");
    ~TerrainController() override = default;
    // 名前取得
    const char* GetName() const override { return "TerrainController"; }
    // 生成時処理
    void OnCreate() override;
    // 遅延更新処理
    void LateUpdate(float elapsedTime) override;
    // 描画処理
    void Render(const RenderContext& rc) override;
    // デバッグ描画
    void DebugRender(const RenderContext& rc) override;
    // GUI描画
    void DrawGui() override;
#pragma region アクセサ
    // 地形取得
    std::weak_ptr<Terrain> GetTerrain() const { return _terrain; }
    // 地形の編集フラグを取得
    bool IsEditing() const { return _isEditing; }
    // 地形の編集フラグを設定
    void SetEditing(bool editing) { _isEditing = editing; }
#pragma endregion

private:
    std::shared_ptr<Terrain> _terrain = nullptr;

    // ストリームアウトデータ描画フラグ
    bool _drawStreamOut = false;
    // 透明壁描画フラグ
    bool _drawTransparentWall = false;
    // 編集フラグ
	bool _isEditing = false;
};