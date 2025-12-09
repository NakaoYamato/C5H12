#pragma once

//#include "TerrainController.h"
//#include "../InstancingModelRenderer.h"
//
//class TerrainEnvironmentController : public Component
//{
//public:
//	TerrainEnvironmentController(std::weak_ptr<Terrain> terrain, int id, const DirectX::XMFLOAT4X4& terrainTransform) :
//		_terrain(terrain),
//		_layoutID(id),
//		_terrainTransform(terrainTransform)
//	{
//	}
//	~TerrainEnvironmentController() override = default;
//	// 名前取得
//	const char* GetName() const override { return "TerrainEnvironmentController"; }
//	// 生成時処理
//	void OnCreate() override;
//	// 開始処理
//	void Start() override;
//	// 更新処理
//	void Update(float elapsedTime) override;
//	// GUI描画
//	void DrawGui() override;
//
//private:
//	// インスタンシングモデルレンダラーへの参照
//	std::weak_ptr<InstancingModelRenderer> _renderer;
//	// 地形への参照
//	std::weak_ptr<Terrain> _terrain;
//	// 地形の変形情報
//	DirectX::XMFLOAT4X4 _terrainTransform = DirectX::XMFLOAT4X4();
//	// 自身の配置番号
//	int _layoutID = -1;
//	// 地形の配置情報で上書きするか
//	bool _overwrite = false;
//	// 更新タイプ
//	TerrainObjectLayout::UpdateType _updateType = TerrainObjectLayout::UpdateType::None;
//};