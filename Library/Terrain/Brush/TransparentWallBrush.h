#pragma once

#include "../../Library/Component/Terrain/TerrainDeformer.h"

class TransparentWallBrush : public TerrainDeformerBrush
{
public:
	enum class State
	{
		None,
		CreatePoint,
		MovePoint,
		EditPoint,
	};

	enum class IntersectState
	{
		None,
		Terrain,
		TransparentWallPoint,
	};
public:
	TransparentWallBrush(TerrainDeformer* deformer);
	~TransparentWallBrush() override = default;
	// 名前取得
	const char* GetName() const override { return u8"透明壁編集"; }
	// 更新処理
	void Update(std::vector<std::shared_ptr<TerrainController>>& terrainControllers,
		float elapsedTime,
		Vector3* intersectWorldPosition) override;
	// GUI描画
	void DrawGui() override;
	// 描画処理
	// 行わない
	void Render(SpriteResource* fullscreenQuad, 
		std::shared_ptr<Terrain> terrain,
		const RenderContext& rc,
		ID3D11ShaderResourceView** srv,
		uint32_t startSlot,
		uint32_t numViews) override {
	}

private:
	// 状態変更
	void ChangeState(State newState);

#pragma region 各状態の更新処理
	void UpdateNone(std::shared_ptr<Terrain> terrain, float elapsedTime);
	void UpdateCreatePoint(std::shared_ptr<Terrain> terrain, float elapsedTime);
	void UpdateMovePoint(std::shared_ptr<Terrain> terrain, float elapsedTime);
	void UpdateEditPoint(std::shared_ptr<Terrain> terrain, float elapsedTime);
#pragma endregion

#pragma region 各状態のGUI描画
	void DrawGuiCreatePoint(std::shared_ptr<Terrain> terrain);
	void DrawGuiEditPoint(std::shared_ptr<Terrain> terrain);
#pragma endregion

	// デバッグ描画
	void DebugRender(std::shared_ptr<Terrain> terrain);
private:
	std::shared_ptr<Terrain> _terrain = nullptr;

	// 編集状態
	State _state = State::None;
	// 交差状態
	IntersectState _intersectState = IntersectState::None;
	// 交差点
	Vector3 _intersectionWorldPoint = Vector3::Zero;
	// クリックしたスクリーン座標
	Vector2 _clickScreenPoint = Vector2::Zero;
	// GUIの表示位置
	Vector2 _guiPosition = Vector2::Zero;
	// 編集中の透明壁番号
	int _editingWallIndex = -1;
	// 編集中の透明壁ポイント番号
	int _editingPointIndex = -1;

	// レイキャストの長さ
	float _rayLength = 1000.0f;
	// 透明壁頂点の描画半径
	float _vertexRadius = 0.3f;
	// 透明壁頂点の当たり判定半径
	float _vertexCollisionRadius = 0.5f;

	// 透明壁頂点の色
	Vector4 _vertexColor = Vector4::Green;
	// 選択中の透明壁頂点の色
	Vector4 _selectedVertexColor = Vector4::Red;

	// 編集中のポイントを地面にスナップするかどうか
	bool _snapToGround = true;
};