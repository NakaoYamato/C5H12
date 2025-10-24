#pragma once

#include "Model.h"
#include "../../Library/Math/Vector.h"
#include "../../Library/Graphics/RenderContext.h"

class ModelCollision
{
public:
	struct CollisionBaseData
	{
		int tagIndex = -1; // タグインデックス
		float hitzoneFactor = 45.0f; // 肉質倍率

		// デバッグ用
		bool isSelected = false; // 選択中か
	};

	struct SphereData : public CollisionBaseData
	{
		int nodeIndex		= -1;	// ノードインデックス
		Vector3 position	= {};	// 中心座標
		float radius		= 1.0f; // 半径

		// シリアライズ
		template<class T>
		void serialize(T& archive, const std::uint32_t version);
	};
	struct CapsuleData : public CollisionBaseData
	{
		int startNodeIndex	= -1;	// 開始ノードインデックス
		int endNodeIndex	= -1;	// 終了ノードインデックス
		Vector3 start		= {};	// 開始座標
		Vector3 end			= {};	// 終了座標
		float radius		= 1.0f; // 半径

		// シリアライズ
		template<class T>
		void serialize(T& archive, const std::uint32_t version);
	};
public:
	ModelCollision() = default;
	~ModelCollision() = default;

	/// <summary>
	/// モデル情報読み込み
	/// </summary>
	/// <param name="modelResource"></param>
	void Load(std::weak_ptr<Model> model);
	/// <summary>
	/// デバッグ表示
	/// </summary>
	void DebugRender(const RenderContext& rc);
	/// <summary>
	/// GUI描画
	/// </summary>
	/// <param name="canEdit">編集可能か</param>
	void DrawGui(bool canEdit = true);

#pragma region ファイル操作
	/// <summary>
	/// データ書き出し
	/// </summary>
	/// <param name="filename"></param>
	/// <returns>失敗したらfalse</returns>
	bool Serialize(const char* filename);

	/// <summary>
	/// データ読み込み
	/// </summary>
	/// <param name="filename"></param>
	/// <returns>失敗したらfalse</returns>
	bool Deserialize(const char* filename);

	/// <summary>
	/// 要素全削除
	/// </summary>
	void Clear()
	{
		_sphereDatas.clear();
		_capsuleDatas.clear();
	}
#pragma endregion

#pragma region アクセサ
	/// <summary>
	/// 球データ取得
	/// </summary>
	const std::vector<SphereData>& GetSphereDatas() const { return _sphereDatas; }
	/// <summary>
	/// カプセルデータ取得
	/// </summary>
	const std::vector<CapsuleData>& GetCapsuleDatas() const { return _capsuleDatas; }
	/// <summary>
	/// タグ名リスト取得
	/// </summary>
	/// <returns></returns>
	const std::vector<std::string>& GetTags() const { return _tags; }
#pragma endregion
private:
	std::weak_ptr<Model> _model;
	std::vector<std::string> _tags; // タグ名リスト
	std::vector<SphereData> _sphereDatas; // 球データ
	std::vector<CapsuleData> _capsuleDatas; // カプセルデータ
	// モデルのノード名
	std::vector<const char*> _nodeNames;

	bool		_isDrowSelectedOnly = false; // 選択中の当たり判定のみ描画するか
	bool		_isUsingGuizmo = false; // GUIでGizmoを使用中か
	Vector3*	_gizmoPosition = nullptr; // Gizmoの位置
	int			_gizmoNodeIndex = -1; // Gizmoのノードインデックス
	Vector4 _colorTable[10] = {
		Vector4::Gray,
		Vector4::Red,
		Vector4::Green,
		Vector4::Blue,
		Vector4::Yellow,
		Vector4::Orange,
		Vector4::Purple,
		Vector4::Pink,
		Vector4::Cyan,
		Vector4::LightGreen,
	};
};
