#pragma once

#include "../../Library/Component/Terrain/TerrainDeformer.h"

class ObjectLayoutBrush : public TerrainDeformerBrush
{
public:
	enum class State
	{
		None,
		CreateObject,
		MoveObject,
		EditObject,
	};

	enum class IntersectState
	{
		None,
		Terrain,
		Object,
	};
public:
	ObjectLayoutBrush(TerrainDeformer* deformer);
	~ObjectLayoutBrush() override = default;
	// 名前取得
	const char* GetName() const override { return u8"環境物設置"; }
	// 更新処理
	void Update(std::shared_ptr<Terrain> terrain, float elapsedTime) override;
	// GUI描画
	void DrawGui(std::shared_ptr<Terrain> terrain) override;
	// 描画処理
	void Render(std::shared_ptr<Terrain> terrain,
		const RenderContext& rc,
		ID3D11ShaderResourceView** srv,
		uint32_t startSlot,
		uint32_t numViews) override;
private:
	// 状態変更
	void ChangeState(State newState);

#pragma region 各状態の更新処理
	void UpdateNone(std::shared_ptr<Terrain> terrain, float elapsedTime);
	void UpdateCreateObject(std::shared_ptr<Terrain> terrain, float elapsedTime);
	void UpdateMoveObject(std::shared_ptr<Terrain> terrain, float elapsedTime);
	void UpdateEditObject(std::shared_ptr<Terrain> terrain, float elapsedTime);
#pragma endregion
private:
	// 選択中のモデル
	std::unique_ptr<Model> _selectingModel = nullptr;
	// 選択中のモデルのパス
	std::string _selectingModelPath = "";

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

	// 生成するオブジェクトの更新タイプ
	TerrainObjectLayout::UpdateType _objectUpdateType = TerrainObjectLayout::UpdateType::None;
	// 生成するオブジェクトの当たり判定属性
	TerrainObjectLayout::CollisionType _objectCollisionType = TerrainObjectLayout::CollisionType::None;
	// 生成するオブジェクトのスケール
	Vector3 _objectScale = Vector3::One;
	// 生成するオブジェクトの回転(ラジアン)
	Vector3 _objectAngle = Vector3::Zero;
	// 生成するオブジェクトの当たり判定オフセット
	Vector3 _objectCollisionOffset = Vector3::Zero;
	// 生成するオブジェクトの当たり判定パラメータ
	Vector4 _objectCollisionParameter = Vector4::One;
};