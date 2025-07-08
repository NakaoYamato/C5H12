#include "TerrainEnvironmentController.h"

#include <imgui.h>

// 更新処理
void TerrainEnvironmentController::Update(float elapsedTime)
{
	// 地形が取得できていない場合は自身を削除
	if (_terrain.lock() == nullptr)
	{
		GetActor()->Remove();
		return;
	}
	// 自身のIDが地形の配置情報に存在しないなら自身を削除
	//auto iter = _terrain.lock()->GetEnvironmentObjects().find(_objectID);
	//if (iter == _terrain.lock()->GetEnvironmentObjects().end())
	//{
	//	GetActor()->Remove();
	//	return;
	//}
	//// 地形の配置情報からトランスフォーム情報更新
	//if (_overwrite)
	//{
	//	Transform& transform = GetActor()->GetTransform();
	//	transform.SetPosition(iter->second.position);
	//	transform.SetRotation(iter->second.rotation);
	//	transform.SetScale(iter->second.scale);
	//}
}
// GUI描画
void TerrainEnvironmentController::DrawGui()
{
	ImGui::Text(u8"配置番号: %d", _objectID);
	ImGui::Checkbox(u8"地形の配置情報で上書きするか", &_overwrite);
}
