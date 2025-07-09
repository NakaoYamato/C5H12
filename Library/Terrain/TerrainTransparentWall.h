#pragma once

#include "../../Library/Math/Vector.h"
#include "../../Library/Exporter/Exporter.h"

class TerrainTransparentWall
{
public:
	struct Wall
	{
		// 透明壁の下部分の頂点(ローカル)
		std::vector<Vector3> vertices;
		// 透明壁の高さ
		float height = 5.0f;
		// GUI描画処理
		void DrawGui();
		// 書き出し
		void Export(const char* label, nlohmann::json* jsonData);
		// 読み込み
		void Inport(const char* label, const nlohmann::json& jsonData);
	};
public:
	TerrainTransparentWall() = default;
	~TerrainTransparentWall() = default;

	// GUI描画処理
	void DrawGui();
	// 書き出し
	void Export(nlohmann::json* jsonData);
	// 読み込み
	void Inport(const nlohmann::json& jsonData);
	// 透明壁群を取得
	std::vector<Wall>& GetWalls() { return _walls; }
	// 透明壁の頂点を取得
	std::vector<Vector3>& GetVertices(int wallIndex)
	{
		if (wallIndex < 0 || wallIndex >= static_cast<int>(_walls.size()))
			return _walls[0].vertices; // 無効なインデックスの場合は最初の壁の頂点を返す
		return _walls[wallIndex].vertices;
	}
	// 透明壁を追加
	// 戻り値：追加した壁のインデックス
	int AddWall();
	// 頂点追加（先頭）
	// 戻り値：追加した頂点のインデックス
	int AddVertexToFront(int wallIndex, const Vector3& vertex);
	// 頂点追加（末尾）
	// 戻り値：追加した頂点のインデックス
	int AddVertexToBack(int wallIndex, const Vector3& vertex);
	// 頂点追加（指定番号）
	// 戻り値：追加した頂点のインデックス
	int AddVertexToIndex(int wallIndex, int vertexIndex, const Vector3& vertex);
	// 透明壁を削除
	void RemoveWall(int wallIndex);
	// 透明壁の頂点を削除
	void RemoveVertex(int wallIndex, int vertexIndex);
private:
	// 透明壁群
	std::vector<Wall> _walls;
};