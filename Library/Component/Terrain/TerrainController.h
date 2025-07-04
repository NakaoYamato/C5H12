#pragma once

#include "../Component.h"
#include "../../Library/Terrain/Terrain.h"

class TerrainController : public Component
{
public:
	TerrainController(const std::string& serializePath = "./Data/Terrain/Save/Test000.json") :
		_serializePath(serializePath)
	{
	}
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
	// Terrainからシリアライズデータを読み込む
    void LoadTerrainTextures();

private:
    std::shared_ptr<Terrain> _terrain = nullptr;
    // シリアライズパス
	std::string _serializePath;
    // Terrainに設定する基本色テクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _baseColorTextureSRV;
    // Terrainに設定する法線テクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _normalTextureSRV;
    // Terrainに設定するパラメータテクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _parameterTextureSRV;

    // テクスチャの割り当てたかどうか
	bool _assignTextures = false;
    // ストリームアウトデータ描画フラグ
    bool _drawStreamOut = false;
    // 透明壁描画フラグ
    bool _drawTransparentWall = false;
    // 編集フラグ
	bool _isEditing = false;
};