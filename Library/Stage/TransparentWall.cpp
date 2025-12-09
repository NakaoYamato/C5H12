#include "TransparentWall.h"

#include <imgui.h>

#pragma region TransparentWall
// 書き出し
void TransparentWall::Wall::Export(const char* label, nlohmann::json* jsonData) const
{
    (*jsonData)[label]["verticesSize"] = vertices.size();
    for (size_t i = 0; i < vertices.size(); ++i)
    {
        // Vector3をJSONに変換
        nlohmann::json pointJson = {
            {"x", vertices[i].x},
            {"y", vertices[i].y},
            {"z", vertices[i].z}
        };
        // JSONに追加
        (*jsonData)[label][std::to_string(i)] = pointJson;
    }
    (*jsonData)[label]["height"] = height;
}

// 読み込み
void TransparentWall::Wall::Import(const char* label, const nlohmann::json& jsonData)
{
    if (jsonData.contains(label))
    {
        if (jsonData[label].contains("verticesSize"))
        {
            size_t pointSize = jsonData[label]["verticesSize"].get<size_t>();
            vertices.resize(pointSize);
            for (size_t i = 0; i < pointSize; ++i)
            {
                if (jsonData[label].contains(std::to_string(i)))
                {
                    auto& pointData = jsonData[label][std::to_string(i)];
                    vertices[i].x = pointData.value("x", vertices[i].x);
                    vertices[i].y = pointData.value("y", vertices[i].y);
                    vertices[i].z = pointData.value("z", vertices[i].z);
                }
            }
        }
        height = jsonData[label].value("height", height);
    }
}
#pragma endregion

// GUI描画処理
void TransparentWall::DrawGui()
{
    // Undo/Redoボタンの表示
    if (ImGui::Button("Undo")) Undo();
    ImGui::SameLine();
    if (ImGui::Button("Redo")) Redo();
    ImGui::Separator();

    size_t wallIndex = 0;
    // 削除フラグ管理用
    int removeWallIndex = -1;

    for (auto& wall : _walls)
    {
        ImGui::PushID(static_cast<int>(wallIndex));

        if (ImGui::TreeNode(("Wall:" + std::to_string(wallIndex)).c_str()))
        {
            ImGui::Text(u8"透明壁の頂点数: %zu", wall.vertices.size());

            for (size_t i = 0; i < wall.vertices.size(); i++)
            {
                ImGui::PushID(static_cast<int>(i));

                // 頂点の座標編集
                ImGui::DragFloat3("", &wall.vertices[i].x, 0.1f);
                // ドラッグ開始時に状態保存
                if (ImGui::IsItemActivated()) SaveState();

                ImGui::SameLine();
                if (ImGui::Button(u8"削除"))
                {
                    SaveState(); // 削除前に保存
                    // 透明壁の頂点を削除
                    wall.vertices.erase(wall.vertices.begin() + i);
                    ImGui::PopID();
                    break;
                }
                ImGui::PopID();
            }

            // 高さの編集
            ImGui::DragFloat(u8"高さ", &wall.height, 0.1f, 0.0f, 100.0f);
            // ドラッグ開始時に状態保存
            if (ImGui::IsItemActivated()) SaveState();

            if (ImGui::Button(u8"頂点追加"))
            {
                SaveState(); // 追加前に保存
                // 透明壁の頂点を追加
                wall.vertices.push_back(Vector3{});
            }

            // 壁自体の削除ボタン
            if (ImGui::Button(u8"この壁を削除"))
            {
                removeWallIndex = static_cast<int>(wallIndex);
            }

            ImGui::TreePop();
        }
        ImGui::PopID();
        wallIndex++;
    }

    // ループ外で壁の削除処理
    if (removeWallIndex != -1)
    {
        RemoveWall(removeWallIndex);
    }

    ImGui::Separator();
    if (ImGui::Button(u8"透明壁の追加"))
    {
        AddWall();
    }
}

#pragma region UndoRedo
void TransparentWall::Undo()
{
    if (_undoStack.empty()) return;

    // 現在の状態をRedoスタックに退避
    _redoStack.push_back(_walls);

    // Undoスタックから復元
    _walls = _undoStack.back();
    _undoStack.pop_back();

	// Undoコールバックの呼び出し
	if (_onUnDoCallback)
	{
		_onUnDoCallback();
	}
}

void TransparentWall::Redo()
{
    if (_redoStack.empty()) return;

    // 現在の状態をUndoスタックに退避
    _undoStack.push_back(_walls);

    // Redoスタックから復元
    _walls = _redoStack.back();
    _redoStack.pop_back();

	// Redoコールバックの呼び出し
	if (_onReDoCallback)
	{
		_onReDoCallback();
	}
}
#pragma endregion

#pragma region 入出力
// ファイル読み込み
bool TransparentWall::LoadFromFile(std::string filename)
{
    nlohmann::json jsonData;
    if (!Exporter::LoadJsonFile(filename.c_str(), &jsonData))
        return false;
	if (jsonData.contains("WallsSize"))
	{
		size_t wallSize = jsonData["WallsSize"].get<size_t>();
		_walls.resize(wallSize);
		for (size_t i = 0; i < wallSize; ++i)
		{
			_walls[i].Import(("Wall" + std::to_string(i)).c_str(), jsonData);
		}
		return true;
	}

	return false;
}

// ファイル保存
bool TransparentWall::SaveToFile(std::string filename)
{
    nlohmann::json jsonData;
	jsonData["WallsSize"] = _walls.size();
	for (size_t i = 0; i < _walls.size(); ++i)
	{
		_walls[i].Export(("Wall" + std::to_string(i)).c_str(), &jsonData);
	}
	return Exporter::SaveJsonFile(filename.c_str(), jsonData);
}
#pragma endregion

// 透明壁を追加
// 戻り値：追加した壁のインデックス
int TransparentWall::AddWall()
{
    SaveState(); // 状態保存

    // 新しい透明壁を追加
    _walls.push_back(Wall{});
    return static_cast<int>(_walls.size() - 1); // 追加した透明壁のインデックスを返す
}

// 頂点追加（先頭）
// 戻り値：追加した頂点のインデックス
int TransparentWall::AddVertexToFront(int wallIndex, const Vector3& vertex)
{
    if (wallIndex < 0 || wallIndex >= static_cast<int>(_walls.size()))
        return -1; // 無効なインデックス

    SaveState(); // 状態保存

    // 透明壁の頂点を先頭に追加
    _walls[wallIndex].vertices.insert(_walls[wallIndex].vertices.begin(), vertex);
    return 0; // 成功
}

// 頂点追加（末尾）
// 戻り値：追加した頂点のインデックス
int TransparentWall::AddVertexToBack(int wallIndex, const Vector3& vertex)
{
    if (wallIndex < 0 || wallIndex >= static_cast<int>(_walls.size()))
        return -1; // 無効なインデックス

    SaveState(); // 状態保存

    // 透明壁の頂点を末尾に追加
    _walls[wallIndex].vertices.push_back(vertex);
    return static_cast<int>(_walls[wallIndex].vertices.size() - 1); // 成功、追加した頂点のインデックスを返す
}

// 頂点追加（指定番号）
// 戻り値：追加した頂点のインデックス
int TransparentWall::AddVertexToIndex(int wallIndex, int vertexIndex, const Vector3& vertex)
{
    if (wallIndex < 0 || wallIndex >= static_cast<int>(_walls.size()))
        return -1; // 無効なインデックス
    if (vertexIndex < 0 || vertexIndex > static_cast<int>(_walls[wallIndex].vertices.size()))
        return -1; // 無効な頂点インデックス

    SaveState(); // 状態保存

    // 透明壁の頂点を指定番号に追加
    _walls[wallIndex].vertices.insert(_walls[wallIndex].vertices.begin() + vertexIndex, vertex);
    return vertexIndex; // 成功、追加した頂点のインデックスを返す
}

// 頂点の移動
void TransparentWall::MoveVertex(int wallIndex, int vertexIndex, const Vector3& newPosition)
{
	if (wallIndex >= 0 && wallIndex < static_cast<int>(_walls.size()))
	{
		auto& vertices = _walls[wallIndex].vertices;
		if (vertexIndex >= 0 && vertexIndex < static_cast<int>(vertices.size()))
		{
			SaveState(); // 状態保存
			vertices[vertexIndex] = newPosition;
		}
	}
}

// 透明壁を削除
void TransparentWall::RemoveWall(int wallIndex)
{
    if (wallIndex >= 0 && wallIndex < static_cast<int>(_walls.size()))
    {
        SaveState(); // 状態保存

        _walls.erase(_walls.begin() + wallIndex);
    }
}

// 透明壁の頂点を削除
void TransparentWall::RemoveVertex(int wallIndex, int vertexIndex)
{
    if (wallIndex >= 0 && wallIndex < static_cast<int>(_walls.size()))
    {
        auto& vertices = _walls[wallIndex].vertices;
        if (vertexIndex >= 0 && vertexIndex < static_cast<int>(vertices.size()))
        {
            SaveState(); // 状態保存

            vertices.erase(vertices.begin() + vertexIndex);
            // 頂点が削除された後、透明壁の頂点が空になった場合は透明壁自体を削除
            if (vertices.empty())
            {
                RemoveWall(wallIndex);
            }
        }
    }
}

// 変更前の状態を保存する
void TransparentWall::SaveState()
{
    // 現在の状態をUndoスタックに積む
    _undoStack.push_back(_walls);

    // 履歴制限を超えたら古いものを削除
    if (_undoStack.size() > MAX_HISTORY)
    {
        _undoStack.pop_front();
    }

    // 新しい操作をしたのでRedoスタックはクリアする
    _redoStack.clear();
}
