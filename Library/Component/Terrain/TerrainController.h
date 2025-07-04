#pragma once

#include "../Component.h"
#include "../../Library/Terrain/Terrain.h"

class TerrainController : public Component
{
public:
    TerrainController() = default;
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

	// パラメータマップの読み込み
    void LoadParameterMap(const wchar_t* filePath);
	// データマップの読み込み
	void LoadDataMap(const wchar_t* filePath);
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
    // 読み込む用のパラメータマップ
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _loadParameterMapSRV;
	// 読み込む用のデータマップ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _loadDataMapSRV;

    // ストリームアウトデータ描画フラグ
    bool _drawStreamOut = false;
    // 透明壁描画フラグ
    bool _drawTransparentWall = false;
    // 編集フラグ
	bool _isEditing = false;
};