#pragma once

#include "MeshCollider.h"
#include "../../Library/Stage/TransparentWall.h"

class TransparentWallCollider : public MeshCollider
{
public:
	enum class EditState
	{
		None,
		CreatePoint,
		MovePoint,
		EditPoint,
	};

	enum IntersectState : int
	{
		NoneIntersect	= 0b0000,
		Stage			= 0b0001,
		WallPoint		= 0b0010,
	};

	enum EditInputState
	{
		NoneInput,
		LeftClick,
		LeftDoubleClick,
		RightClick,
	};

public:
	TransparentWallCollider() {}
	~TransparentWallCollider() override {}
	// 名前取得
	const char* GetName() const override { return "TransparentWallCollider"; }
	// 生成時処理
	void OnCreate() override;
	// 更新処理
	void Update(float elapsedTime) override;
	// デバッグ描画処理
	void DebugRender(const RenderContext& rc) override;
	// GUI描画
	void DrawGui() override;
	// コリジョンメッシュの再計算
	CollisionMesh RecalculateCollisionMesh(Model* model) const override;

	// 透明壁取得
	TransparentWall& GetTransparentWall() { return _transparentWall; }

#pragma region 入出力
	// ファイル読み込み
	bool LoadFromFile() override;
	// ファイル保存
	bool SaveToFile() override;
#pragma endregion

private:
	// 編集処理
	void EditingUpdate(float elapsedTime);
	// 状態変更
	void ChangeState(EditState newState);
	// 編集中のデバッグ描画
	void EditingDebugRender(const RenderContext& rc);
#pragma region 各状態の更新処理
	void UpdateNone(float elapsedTime);
	void UpdateCreatePoint(float elapsedTime);
	void UpdateMovePoint(float elapsedTime);
	void UpdateEditPoint(float elapsedTime);
#pragma endregion

#pragma region 各状態のGUI描画
	void DrawGuiCreatePoint();
	void DrawGuiEditPoint();
#pragma endregion

private:
	// 透明壁
	TransparentWall _transparentWall;

#pragma region 編集用変数
	// 編集フラグ
	bool _isEditing = false;
	// デバッグ表示
	bool _debugRender = false;
	// 編集状態
	EditState _state = EditState::None;
	// 交差状態
	int _intersectState = IntersectState::NoneIntersect;
	// 入力状態
	EditInputState _inputState = EditInputState::NoneInput;
	float _leftClickTime = 0.0f;
	// ダブルクリック判定時間
	const float _doubleClickThreshold = 0.3f;

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
	// 編集中の頂点情報
	Vector3 _editingVertex = Vector3::Zero;

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
	// マウスの当たり判定を行うか
	bool _enableMouseCollision = true;
#pragma endregion
};