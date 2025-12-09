#pragma once

#include "../Component.h"
#include "../../Library/Terrain/Terrain.h"

class TerrainController : public Component
{
public:
    enum class EditState
    {
        None,
		Editing,
        Complete,
    };

public:
    TerrainController(const std::string& serializePath = "./Data/Terrain/Save/Test000.json");
    ~TerrainController() override = default;
    // 名前取得
    const char* GetName() const override { return "TerrainController"; }
    // 生成時処理
    void OnCreate() override;
	// 更新処理
    void Update(float elapsedTime) override;
    // 遅延更新処理
    void LateUpdate(float elapsedTime) override;
    // 描画処理
    void Render(const RenderContext& rc) override;
    // 影描画
    void CastShadow(const RenderContext& rc) override;
    // デバッグ描画
    void DebugRender(const RenderContext& rc) override;
    // GUI描画
    void DrawGui() override;

    // 環境物の生成
	void CreateEnvironment(int layoutID);
#pragma region アクセサ
    // 地形取得
    std::weak_ptr<Terrain> GetTerrain() const { return _terrain; }
	// 編集状態取得
	EditState GetEditState() const { return _editState; }
	// 編集状態設定
    void SetEditState(EditState state);
#pragma endregion

private:
    std::shared_ptr<Terrain> _terrain = nullptr;
	std::vector<std::weak_ptr<Actor>> _environmentObjects;

	// 編集状態
	EditState _editState = EditState::None;
    // ストリームアウトデータ描画フラグ
    bool _drawStreamOut = false;
	// 環境物の再生成フラグ
	bool _recreateEnvironment = false;
};