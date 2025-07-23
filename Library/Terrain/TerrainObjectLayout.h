#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

#include "../../Library/Math/Vector.h"
#include "../../Library/Model/Model.h"
#include "../../Library/Exporter/Exporter.h"

class TerrainObjectLayout
{
public:
	enum class CollisionType
	{
		None,       // 衝突なし
		Box,		// ボックス
		BoxTrigger, // ボックストリガー
		Sphere,     // 球
		SphereTrigger, // 球トリガー
		Capsule,    // カプセル
		CapsuleTrigger, // カプストリガー
		Mesh,		// メッシュ
		MeshTrigger // メッシュトリガー
	};
	// 配置情報
	struct LayoutData
	{
		// モデルの参照パス
		std::string modelPath = "";
		// 衝突判定の種類
		CollisionType collisionType = CollisionType::None;
		// 配置位置(Terrainのローカル空間)
		Vector3 localPosition = Vector3::Zero;
		// 配置回転
		Vector3 rotation = Vector3::Zero;
		// 配置サイズ
		Vector3 size = Vector3::One;
		// 衝突判定のオフセット値
		Vector3 collisionOffset = Vector3::Zero;
		// 衝突判定のパラメータ
		// ボックスなら(x : 半辺長.x, y : 半辺長.y, z : 半辺長.z)
		// 球なら(x : 半径)
		// カプセルなら(x : 終点.x, y : 終点.y, z : 終点.z, w : 半径)
		Vector4 collisionParameter = Vector4::One;
		// 書き出し
		void Export(const char* label, nlohmann::json* jsonData);
		// 読み込み
		void Import(const char* label, const nlohmann::json& jsonData);
	};
public:
	TerrainObjectLayout() = default;
	~TerrainObjectLayout() = default;

	// GUI描画処理
	void DrawGui();
	// モデルを追加
	void AddModel(ID3D11Device* device, const std::string& filepath);
	// 配置情報を追加
	int AddLayout(const std::string& modelPath, 
		CollisionType collisionType,
		const Vector3& position,
		const Vector3& rotation,
		const Vector3& size,
		const Vector3& collisionOffset,
		const Vector4& collisionParameter);
	// モデルデータを取得
	const std::unordered_map<std::string, std::shared_ptr<Model>>& GetModels() const { return _models; }
	// モデルデータを取得
	std::weak_ptr<Model> GetModel(const std::string& path) { return _models[path]; }
	// 配置データを取得
	const std::unordered_map<int, LayoutData>& GetLayouts() const { return _layouts; }
	// モデルが登録されているか確認
	bool HasModel(const std::string& path) const
	{
		return _models.find(path) != _models.end();
	}
	// 配置情報が登録されているか確認
	const LayoutData* FindLayout(int index) const;
	// モデルの削除
	void RemoveModel(const std::string& path);
	// 書き出し
	void Export(nlohmann::json* jsonData);
	// 読み込み
	void Import(ID3D11Device* device, const nlohmann::json& jsonData);
private:
	// 登録しているモデルデータ
	std::unordered_map<std::string, std::shared_ptr<Model>> _models;
	// モデルの配置情報
	std::unordered_map<int, LayoutData> _layouts;
	// 現在のモデルのインデックス
	int _currentModelIndex = 0;
};