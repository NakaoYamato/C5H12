#pragma once

#include "TerrainController.h"

class TerrainCollisionMaker : public Component
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
	TerrainCollisionMaker() {}
	~TerrainCollisionMaker() override {}
	// 名前取得
	const char* GetName() const override { return "TerrainCollisionMaker"; }
	// 開始処理
	void Start() override;
	// 更新処理
	void Update(float elapsedTime) override;
	// GUI描画
	void DrawGui() override;
	// デバッグ描画
	void DebugRender(const RenderContext& rc) override;

private:
	// 状態変更
	void ChangeState(State newState);

private:
	// 地形コントローラーへの参照
	std::weak_ptr<TerrainController> _terrainController;

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
	// 透明壁のポイントの描画半径
	float _transparentWallPointRadius = 0.3f;
	// 透明壁のポイントの当たり判定半径
	float _transparentWallPointHitRadius = 0.5f;

	// 透明壁のポイントの色
	Vector4 _transparentWallPointColor = Vector4::Green;
	// 透明壁のポイントの選択色
	Vector4 _transparentWallPointSelectColor = Vector4::Red;

	// 前フレームのGUI操作フラグ
	bool _wasGuiActive = false;
	// 編集フラグ
	bool _isEditing = false;
	// 編集中のポイントを地面にスナップするかどうか
	bool _snapToGround = true;
};