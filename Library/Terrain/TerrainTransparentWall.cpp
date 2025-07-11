#include "TerrainTransparentWall.h"

#include <imgui.h>

// GUI描画処理
void TerrainTransparentWall::DrawGui()
{
    size_t wallIndex = 0;
    for (auto& wall : _walls)
    {
        if (ImGui::TreeNode(("Wall:" + std::to_string(wallIndex)).c_str()))
        {
            ImGui::Text(u8"透明壁の頂点数: %zu", wall.vertices.size());
            for (size_t i = 0; i < wall.vertices.size(); i++)
            {
                ImGui::DragFloat3(std::to_string(i).c_str(), &wall.vertices[i].x, 0.1f);
                ImGui::SameLine();
                if (ImGui::Button(u8"削除"))
                {
                    // 透明壁の頂点を削除
                    wall.vertices.erase(wall.vertices.begin() + i);
                    // 削除後はループを抜ける
                    break;
                }
            }
            ImGui::DragFloat(u8"透明壁の高さ", &wall.height, 0.1f, 0.0f, 100.0f);
            if (ImGui::Button(u8"透明壁の頂点追加"))
            {
                // 透明壁の頂点を追加
                wall.vertices.push_back(Vector3{});
            }

            ImGui::TreePop();
        }
        wallIndex++;
    }
    ImGui::Separator();
    if (ImGui::Button(u8"透明壁の追加"))
    {
        // 透明壁を追加
        _walls.push_back(Wall{});
    }
}
// 書き出し
void TerrainTransparentWall::Export(nlohmann::json* jsonData)
{
    (*jsonData)["WallsSize"] = _walls.size();
    for (size_t i = 0; i < _walls.size(); ++i)
    {
        _walls[i].Export(("Wall" + std::to_string(i)).c_str(), jsonData);
    }
}
// 読み込み
void TerrainTransparentWall::Import(const nlohmann::json& jsonData)
{
    if (jsonData.contains("WallsSize"))
    {
        size_t wallSize = jsonData["WallsSize"].get<size_t>();
        _walls.resize(wallSize);
        for (size_t i = 0; i < wallSize; ++i)
        {
            _walls[i].Import(("Wall" + std::to_string(i)).c_str(), jsonData);
        }
    }
}
// 透明壁を追加
int TerrainTransparentWall::AddWall()
{
	// 新しい透明壁を追加
	_walls.push_back(Wall{});
	return static_cast<int>(_walls.size() - 1); // 追加した透明壁のインデックスを返す
}
// 頂点追加（先頭）
int TerrainTransparentWall::AddVertexToFront(int wallIndex, const Vector3& vertex)
{
	if (wallIndex < 0 || wallIndex >= static_cast<int>(_walls.size()))
		return -1; // 無効なインデックス
	// 透明壁の頂点を先頭に追加
	_walls[wallIndex].vertices.insert(_walls[wallIndex].vertices.begin(), vertex);
	return 0; // 成功
}
// 頂点追加（末尾）
int TerrainTransparentWall::AddVertexToBack(int wallIndex, const Vector3& vertex)
{
	if (wallIndex < 0 || wallIndex >= static_cast<int>(_walls.size()))
		return -1; // 無効なインデックス
	// 透明壁の頂点を末尾に追加
	_walls[wallIndex].vertices.push_back(vertex);
	return static_cast<int>(_walls[wallIndex].vertices.size() - 1); // 成功、追加した頂点のインデックスを返す
}
// 頂点追加（指定番号）
int TerrainTransparentWall::AddVertexToIndex(int wallIndex, int vertexIndex, const Vector3& vertex)
{
	if (wallIndex < 0 || wallIndex >= static_cast<int>(_walls.size()))
		return -1; // 無効なインデックス
	if (vertexIndex < 0 || vertexIndex > static_cast<int>(_walls[wallIndex].vertices.size()))
		return -1; // 無効な頂点インデックス
	// 透明壁の頂点を指定番号に追加
	_walls[wallIndex].vertices.insert(_walls[wallIndex].vertices.begin() + vertexIndex, vertex);
	return vertexIndex; // 成功、追加した頂点のインデックスを返す
}
// 透明壁を削除
void TerrainTransparentWall::RemoveWall(int wallIndex)
{
    if (wallIndex >= 0 && wallIndex < static_cast<int>(_walls.size()))
        _walls.erase(_walls.begin() + wallIndex);
}
// 透明壁の頂点を削除
void TerrainTransparentWall::RemoveVertex(int wallIndex, int vertexIndex)
{
    if (wallIndex >= 0 && wallIndex < static_cast<int>(_walls.size()))
    {
        auto& vertices = _walls[wallIndex].vertices;
        if (vertexIndex >= 0 && vertexIndex < static_cast<int>(vertices.size()))
        {
            vertices.erase(vertices.begin() + vertexIndex);
			// 頂点が削除された後、透明壁の頂点が空になった場合は透明壁自体を削除
			if (vertices.empty())
			{
				RemoveWall(wallIndex);
			}
        }
    }
}
// GUI描画処理
void TerrainTransparentWall::Wall::DrawGui()
{
    ImGui::Text(u8"透明壁の頂点数: %zu", vertices.size());
    for (size_t i = 0; i < vertices.size(); i++)
    {
        ImGui::DragFloat3(std::to_string(i).c_str(), &vertices[i].x, 0.1f);
        ImGui::SameLine();
        if (ImGui::Button(u8"削除"))
        {
            // 透明壁の頂点を削除
            vertices.erase(vertices.begin() + i);
            // 削除後はループを抜ける
            break;
        }
    }
    ImGui::DragFloat(u8"透明壁の高さ", &height, 0.1f, 0.0f, 100.0f);
    if (ImGui::Button(u8"透明壁の頂点追加"))
    {
        // 透明壁の頂点を追加
        vertices.push_back(Vector3{});
    }
}
// 書き出し
void TerrainTransparentWall::Wall::Export(const char* label, nlohmann::json* jsonData)
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
void TerrainTransparentWall::Wall::Import(const char* label, const nlohmann::json& jsonData)
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
