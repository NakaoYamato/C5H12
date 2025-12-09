#pragma once

#include "../../Library/Math/Vector.h"
#include "../../Library/Exporter/Exporter.h"

#include <functional>
#include <deque>

// 透明壁
class TransparentWall
{
public:
	struct Wall
	{
		// 透明壁の下部分の頂点(ローカル)
		std::vector<Vector3> vertices;
		// 透明壁の高さ
		float height = 5.0f;
		// 書き出し
		void Export(const char* label, nlohmann::json* jsonData) const;
		// 読み込み
		void Import(const char* label, const nlohmann::json& jsonData);
	};

public:
	TransparentWall() = default;
	~TransparentWall() = default;

	// GUI描画処理
	void DrawGui();

#pragma region UndoRedo
	// 元に戻す
	void Undo();
	// やり直し
	void Redo();
	// Undoが可能か
	bool CanUndo() const { return !_undoStack.empty(); }
	// Redoが可能か
	bool CanRedo() const { return !_redoStack.empty(); }
	// Undo時のコールバック設定
	void SetOnUndoCallback(std::function<void()> callback) { _onUnDoCallback = callback; }
	// Redo時のコールバック設定
	void SetOnRedoCallback(std::function<void()> callback) { _onReDoCallback = callback; }
#pragma endregion

#pragma region 入出力
	// ファイル読み込み
	bool LoadFromFile(std::string filename);
	// ファイル保存
	bool SaveToFile(std::string filename);
#pragma endregion

	// 透明壁群を取得
	const std::vector<Wall>& GetWalls() const { return _walls; }
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
	// 頂点の移動
	void MoveVertex(int wallIndex, int vertexIndex, const Vector3& newPosition);
	// 透明壁を削除
	void RemoveWall(int wallIndex);
	// 透明壁の頂点を削除
	void RemoveVertex(int wallIndex, int vertexIndex);

private:
	// 変更前の状態を保存する
	void SaveState();

private:
	// 透明壁群
	std::vector<Wall> _walls;
	
	// Undo/Redo用の履歴 (最大数は必要に応じて制限可)
	std::deque<std::vector<Wall>> _undoStack;
	std::deque<std::vector<Wall>> _redoStack;
	std::function<void()> _onUnDoCallback;
	std::function<void()> _onReDoCallback;
	// 履歴の最大保持数
	const size_t MAX_HISTORY = 50;
};